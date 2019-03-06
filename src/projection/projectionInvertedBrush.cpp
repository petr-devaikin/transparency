//
//  projectionInvertedBrush.cpp
//  transparency
//
//  Created by Petr Devaikin on 23.07.18.
//

#include "projectionInvertedBrush.hpp"


projectionInvertedBrush::projectionInvertedBrush(const string basePath, touchArea * t, ofMatrix4x4 transformCamera2Projection, ofMatrix4x4 transformImage2Projection, ofRectangle _projectionBox, float _thresholdSensitive, float expSpeed, float bluredR) : baseProjection(t) {
    timer = ofGetElapsedTimef();
    
    camera2Projection = transformCamera2Projection;
    image2Projection = transformImage2Projection;
    projectionBox = _projectionBox;
    int projectionBoxWidth = projectionBox.width;
    int projectionBoxHeight = projectionBox.height;
    
    thresholdSensitive = _thresholdSensitive;
    expansionSpeed = expSpeed;
    bluredRadius = bluredR;
    
    // init result fbo
    resultFbo.allocate(projectionBoxWidth, projectionBoxHeight, GL_RGBA);
    
    // zero block
    onesBlock = new unsigned char[projectionBoxWidth * projectionBoxHeight]; // only b-channel
    memset(onesBlock, 255, projectionBoxWidth * projectionBoxHeight);
    
    // prepare touch brush
    touchBrush.load(ofFilePath::join(basePath, "exp_brush_4001.png"));
    
    touchBrushResized.allocate(2 * projectionBoxWidth + 1, 2 * projectionBoxHeight + 1, GL_RGBA);
    touchBrushResized.begin();
    ofClear(0);
    ofSetColor(255);
    touchBrush.draw(projectionBoxWidth - round(touchBrush.getWidth() / 2) + 1, projectionBoxHeight - round(touchBrush.getHeight() / 2) + 1);
    touchBrushResized.end();
    
    // load shaders
    shaderExpansion.load(ofFilePath::join(basePath, "shadersGL3/expansion"));
    shaderExpansionAdder.load(ofFilePath::join(basePath, "shadersGL3/expansionMaskAdder"));
    shaderTransparency.load(ofFilePath::join(basePath, "shadersGL3/transparency"));
    shaderMix2Images.load(ofFilePath::join(basePath, "shadersGL3/mix2images"));
    
    /*
    ofImage img;
    img.allocate(4001, 4001, OF_IMAGE_COLOR_ALPHA);
    for (int i = 0; i < 4001; i++)
        for (int j = 0; j < 4001; j++) {
            int dist = int(round(sqrt((i - 2001) * (i - 2001) + (j - 2001) * (j - 2001))));
            img.setColor(i, j, ofColor(dist / 256, dist % 256, 0));
        }
    img.update();
    ofSaveImage(img, "exp_brush_4001.png", OF_IMAGE_QUALITY_BEST);
    */
}

projectionInvertedBrush::~projectionInvertedBrush() {
    // clean the memory
    delete[] onesBlock;
}

bool projectionInvertedBrush::start() {
    resetLayers();
    
    baseProjection::start();
}

void projectionInvertedBrush::addImage(const std::string &imgPath) {
    ofImage newImg;
    newImg.load(imgPath);
    originalImages.push_back(newImg);
    
    // transform
    ofFbo transformedImage;
    transformedImage.allocate(projectionBox.width, projectionBox.height, GL_RGB);
    transformedImage.begin();
    {
        ofSetColor(255);
        ofClear(0);
        ofPushMatrix();
        ofMultMatrix(image2Projection);
        newImg.draw(0, 0);
        ofPopMatrix();
    }
    transformedImage.end();
    
    transformedImages.push_back(transformedImage);
}

// Helpers

// calculate touch blobs on touch area
void projectionInvertedBrush::calculateTouchBlobs() {
    ofPixels touchPixels;
    // get current touch canvas
    
    // ! improve here asking for unsigned char array instead of ofImage!
    (touch->getTouch()).readToPixels(touchPixels);
    touchAreaImage.setFromPixels(touchPixels.getChannel(0));
    touchAreaImage.threshold(255 * thresholdSensitive);
    
    contourFinder.findContours(touchAreaImage, 4, projectionBox.width * projectionBox.height, 4, false);
}

int projectionInvertedBrush::detectLayerIndex(ofPoint point) {
    // check all layers starting from the top one
    int touchedLayer = 0;
    for (touchedLayer = layers.size() - 1; touchedLayer > 0; touchedLayer--) {
        if (layers[touchedLayer].checkIfTouched(point)) break;
    }
    
    return touchedLayer;
}

// Update

void projectionInvertedBrush::updateAllLayers() {
    // expand touched areas on all layers
    float currentTime = ofGetElapsedTimef();
    float expansionRadius = expansionSpeed * (currentTime - timer);
    
    // if there was enough time to make the smallest possible step to expand touch
    if (expansionRadius >= 1) {
        float floorRadius = floor(expansionRadius);
        timer = currentTime - (expansionRadius - floorRadius) / expansionSpeed;
        
        for (int i = 0; i < layers.size(); i++) {
            layers[i].expand(floorRadius);
        }
    }
}

void projectionInvertedBrush::update() {
    if (!started) return; // not started yet
    
    updateAllLayers(); // update all areas;
    
    // check if we can remove the last layer
    if (layers.size() > 1 && layers[1].isFull()) {
        layers.erase(layers.begin());
        cout << "Layer removed. Number of layers: " << layers.size() << "\n";
    }
    
    calculateTouchBlobs(); // Check if the canvas is touched right now and calculate blobs
    
    for (int i = 0; i < contourFinder.nBlobs; i++) {
        ofxCvBlob blob = contourFinder.blobs[i];
        
        // transform touch centroid from camera space to projection space
        ofVec3f transformedCentroid = ofVec3f(blob.centroid[0], blob.centroid[1]) * camera2Projection;
        
        cout << "Layer touched.\n";
        
        // check if only background exists and add new layer
        if (layers.size() == 1) {
            int newLayerImageIndex = (layers[0].getImageIndex() + 1) % transformedImages.size();
            addImageToLayers(newLayerImageIndex);
            cout << "New layer " << newLayerImageIndex << " added. Number of layers: " << layers.size() << "\n";
        }
            
        layers[1].addTouch(transformedCentroid, bluredRadius);
    }
    
    drawLayers();
}

void projectionInvertedBrush::drawLayers() {
    resultFbo.begin();
    
    ofSetColor(255);
    
    if (layers.size() == 1) {
        layers[0].getImage().draw(0, 0);
    }
    else {
        shaderMix2Images.begin();
        shaderMix2Images.setUniform1i("radius", bluredRadius);
        shaderMix2Images.setUniformTexture("background", layers[0].getImage().getTexture(), 1);
        
        shaderMix2Images.setUniformTexture("mask", layers[1].getMask().getTexture(), 2);
        layers[1].getImage().draw(0, 0);
        
        shaderMix2Images.end();
    }
    
    /*
    shaderTransparency.begin();
    shaderTransparency.setUniform1i("radius", bluredRadius);
    
    for (int i = 0; i < layers.size(); i++) {
        shaderTransparency.setUniformTexture("mask", layers[i].getMask().getTexture(), 1);
        layers[i].getImage().draw(0, 0);
    }
    
    shaderTransparency.end();
     */
    
    resultFbo.end();
}

void projectionInvertedBrush::draw() {
    if (!started) return; // not started yet
    
    ofSetColor(255);
    
    resultFbo.draw(projectionBox.x, projectionBox.y);
    
    // draw touch canvas
    //(touch->getTransformedTouch()).draw(0, 0);
    //return;
     
    //touch->getTransformedTouch().draw(0, 0);
    
    // show masks
    /*
    if (layers.size() > 1)
        layers[1].getMask().draw(projectionBox.x, projectionBox.y);
     */
    /*
    // show touch blobs
    for (int i = 0; i < contourFinder.nBlobs; i++) {
        ofRectangle rect = contourFinder.blobs.at(i).boundingRect;
        ofSetColor(255, 0, 0);
        ofDrawRectangle(rect.getLeft(), rect.getTop(), rect.width, rect.height);
    }
     */
}

void projectionInvertedBrush::resetLayers() {
    layers.clear(); // need to clean memory?
    addImageToLayers(0);
    layers[0].setFull();
    
    cout << "Reset layers. Number of layers: " << layers.size() << "\n";
}

void projectionInvertedBrush::addImageToLayers(int i) {
    layerWithMask newLayer(transformedImages[i], i, onesBlock, shaderExpansion, shaderExpansionAdder, touchBrushResized);
    layers.push_back(newLayer);
}
