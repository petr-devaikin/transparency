//
//  projectionInvertedBrush.cpp
//  transparency
//
//  Created by Petr Devaikin on 23.07.18.
//

#include "projectionInvertedBrush.hpp"


projectionInvertedBrush::projectionInvertedBrush(touchArea * t, float t1, float expSpeed, float bluredR) : baseProjection(t) {
    timer = ofGetElapsedTimef();
    
    // set initial size from touchArea
    width = t->getResultWidth();
    height = t->getResultHeight();
    
    thresholdSensitive = t1;
    expansionSpeed = expSpeed;
    bluredRadius = bluredR;
    
    // zero block
    onesBlock = new unsigned char[width * height]; // only b-channel
    memset(onesBlock, 255, width * height);
    
    // prepare touch brush
    touchBrushResized.allocate(2 * width + 1, 2 * height + 1, GL_RGBA);
    touchBrushResized.begin();
    ofClear(0);
    ofSetColor(255);
    touchBrush.draw(width - round(touchBrush.getWidth() / 2) + 1, height - round(touchBrush.getHeight() / 2) + 1);
    touchBrushResized.end();
    
    // prepare touch area for blob search
    touchAreaImage.allocate(t->getResultWidth(), t->getResultHeight());
    
    
    // load shaders
    shaderExpansion.load("shadersGL3/expansion");
    shaderExpansionAdder.load("shadersGL3/expansionMaskAdder");
    shaderTransparency.load("shadersGL3/transparency");
    
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
    
    touchBrush.load("exp_brush_4001.png");
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
    
    // resize
    ofImage newImage;
    newImage.clone(newImg);
    newImage.resize(width, height);
    scaledImages.push_back(newImage);
}

// Helpers

// calculate touch blobs on touch area
void projectionInvertedBrush::calculateTouchBlobs() {
    ofPixels touchPixels;
    // get current touch canvas
    (touch->getTransformedTouch()).readToPixels(touchPixels);
    touchAreaImage.setFromPixels(touchPixels.getChannel(0));
    touchAreaImage.threshold(255 * thresholdSensitive);
    
    contourFinder.findContours(touchAreaImage, 4, width * height, 4, false);
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
    
    // compare new blobs with previous touches
    vector<layerTouch> newTouches;
    for (int i = 0; i < contourFinder.nBlobs; i++) {
        ofxCvBlob blob = contourFinder.blobs[i];
        bool previousTouchDetected = false;
        for (int j = 0; j < currentTouches.size(); j++) {
            if (blob.boundingRect.intersects(currentTouches[j].area)) {
                // same gesture. Copy layer from the previous touch
                layerTouch newTouch(currentTouches[j].layer, blob.boundingRect);
                newTouches.push_back(newTouch);
                
                // touch the layer
                newTouch.layer->addTouch(blob.centroid, bluredRadius);
                
                previousTouchDetected = true;
                break;
            }
        }
        
        if (!previousTouchDetected) {
            // new touch, detect layer
            int touchedLayerIndex = 0; // only one layer a time – for Jei's project
            //int touchedLayerIndex = detectLayerIndex(blob.centroid);
            cout << "Layer touched: " << touchedLayerIndex << "\n";
            
            if (touchedLayerIndex == layers.size() - 1) {
                int newLayerImageIndex = (layers[touchedLayerIndex].getImageIndex() + 1) % originalImages.size();
                addImageToLayers(newLayerImageIndex);
                cout << "New layer " << newLayerImageIndex << " added. Number of layers: " << layers.size() << "\n";
            }
            
            layerTouch newTouch(&(layers[touchedLayerIndex + 1]), blob.boundingRect);
            newTouches.push_back(newTouch);
            
            // touch the layer
            newTouch.layer->addTouch(blob.centroid, bluredRadius);
        }
    }
    currentTouches = newTouches;
}

void projectionInvertedBrush::draw() {
    if (!started) return; // not started yet
    
    ofPushMatrix();
    ofMultMatrix(transform);
    
    // draw touch canvas
    //(touch->getTransformedTouch()).draw(0, 0);
    //return;
    
    ofSetColor(255);
    shaderTransparency.begin();
    shaderTransparency.setUniform1i("radius", bluredRadius);
    
    for (int i = 0; i < layers.size(); i++) {
        shaderTransparency.setUniformTexture("mask", layers[i].getMask().getTexture(), 1);
        layers[i].getImage().draw(0, 0);
    }
    
    shaderTransparency.end();
    
    ofPopMatrix();
}

void projectionInvertedBrush::resetLayers() {
    layers.clear(); // need to clean memory?
    addImageToLayers(0);
    layers[0].setFull();
    
    cout << "Reset layers. Number of layers: " << layers.size() << "\n";
}

void projectionInvertedBrush::addImageToLayers(int i) {
    layerWithMask newLayer(scaledImages[i], i, onesBlock, shaderExpansion, shaderExpansionAdder, touchBrushResized);
    layers.push_back(newLayer);
}
