//
//  projectionInvertedBrush.cpp
//  transparency
//
//  Created by Petr Devaikin on 23.07.18.
//

#include "projectionInvertedBrush.hpp"


projectionInvertedBrush::projectionInvertedBrush(cameraManager * camera, touchArea * t, calibrator * calib, bool liveAnimation, float expSpeed, float bluredR) : baseProjection(t) {
    timer = ofGetElapsedTimef();
    
    this->camera = camera;
    this->calib = calib;
    this->liveAnimation = liveAnimation;
    this->expansionSpeed = expSpeed;
    this->bluredRadius = bluredR;
    
    int projectionWidth = calib->getProjectionBox().getWidth();
    int projectionHeight = calib->getProjectionBox().getHeight();
    
    // init result fbo
    resultFbo.allocate(projectionWidth, projectionHeight, GL_RGBA);
    
    // if live animation is activated, init transformed mask
    if (liveAnimation) {
        transformedMask.allocate(projectionWidth, projectionHeight, GL_RGBA);
        tempFbo.allocate(projectionWidth, projectionHeight, GL_RGBA);
    }
    
    // zero block
    onesBlock = new unsigned char[projectionWidth * projectionHeight]; // only b-channel
    memset(onesBlock, 255, projectionWidth * projectionHeight);
    
    
    prepareBrush(); // prepare touch brush
    loadShaders(); // load shaders
}

projectionInvertedBrush::~projectionInvertedBrush() {
    // clean the memory
    delete[] onesBlock;
}

void projectionInvertedBrush::loadShaders() {
    shaderExpansion.load("shadersGL3/expansion");
    shaderExpansionAdder.load("shadersGL3/expansionMaskAdder");
    shaderMix2Images.load("shadersGL3/mix2images");
    shaderSaturationTuner.load("shadersGL3/brightnessTuner");
}

void projectionInvertedBrush::prepareBrush() {
    int projectionWidth = calib->getProjectionBox().getWidth();
    int projectionHeight = calib->getProjectionBox().getHeight();
    
    touchBrush.load("exp_brush_4001.png");
    
    touchBrushResized.allocate(2 * projectionWidth + 1, 2 * projectionHeight + 1, GL_RGBA);
    touchBrushResized.begin();
    ofClear(0);
    ofSetColor(255);
    touchBrush.draw(projectionWidth - round(touchBrush.getWidth() / 2) + 1, projectionHeight - round(touchBrush.getHeight() / 2) + 1);
    touchBrushResized.end();
}

void projectionInvertedBrush::generateBrush() {
     ofImage img;
     img.allocate(4001, 4001, OF_IMAGE_COLOR_ALPHA);
     for (int i = 0; i < 4001; i++)
     for (int j = 0; j < 4001; j++) {
     int dist = int(round(sqrt((i - 2001) * (i - 2001) + (j - 2001) * (j - 2001))));
     img.setColor(i, j, ofColor(dist / 256, dist % 256, 0));
     }
     img.update();
     ofSaveImage(img, "exp_brush_4001.png", OF_IMAGE_QUALITY_BEST);
}

bool projectionInvertedBrush::start() {
    resetLayers();
    return baseProjection::start();
}

void projectionInvertedBrush::addImage(const std::string &imgPath) {
    ofImage newImg;
    newImg.load(imgPath);
    newImg.mirror(false, true);
    originalImages.push_back(newImg);
    
    // transform
    ofFbo transformedImage;
    transformedImage.allocate(calib->getProjectionBox().getWidth(), calib->getProjectionBox().getHeight(), GL_RGB);
    transformedImage.begin();
    {
        ofSetColor(255);
        ofClear(0);
        ofPushMatrix();
        ofMultMatrix(calib->getImage2ProjectionTransform());
        newImg.draw(0, 0);
        ofPopMatrix();
    }
    transformedImage.end();
    
    transformedImages.push_back(transformedImage);
}

// Helpers

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
    
    // transform mask
    
    if (liveAnimation) {
        transformedMask.begin();
        ofSetColor(255);
        ofClear(0);
        ofPushMatrix();
        ofMultMatrix(calib->getCamera2ProjectionTransform());
        (camera->getSubstractedImage()).draw(0, 0);
        ofPopMatrix();
        transformedMask.end();
    }
    
    // work with layers
    
    updateAllLayers();
    
    // check if we can remove the last layer
    if (layers.size() > 1 && layers[1].isFull()) {
        layers.erase(layers.begin());
        cout << "Layer removed. Number of layers: " << layers.size() << "\n";
    }
    
    // Check if the canvas is touched right now and calculate blobs
    vector<ofPoint> touches = touch->detectTouch();
    
    for (int i = 0; i < touches.size(); i++) {
        // transform touch centroid from camera space to projection space
        ofVec3f transformedCentroid = touches[i] * calib->getCamera2ProjectionTransform();
        
        if (transformedCentroid[0] < 0 || transformedCentroid[0] >= calib->getProjectionBox().getWidth() || transformedCentroid[1] < 0 || transformedCentroid[1] >= calib->getProjectionBox().getHeight()) {
            cout << "transformed touch centroid out of projection area " << transformedCentroid << "\n";
            continue;
        }
        
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
    
    resultFbo.end();
    
    // if live animation is on apply another shader
    if (liveAnimation) {
        tempFbo.begin();
        resultFbo.draw(0, 0);
        tempFbo.end();
        
        resultFbo.begin();
        
        shaderSaturationTuner.begin();
        shaderSaturationTuner.setUniform1f("k", 1 / touch->getThreshold() / 3);
        shaderSaturationTuner.setUniformTexture("mask", transformedMask.getTexture(), 1);
        
        tempFbo.draw(0, 0);
        
        shaderSaturationTuner.end();
        
        resultFbo.end();
    }
}

void projectionInvertedBrush::draw() {
    if (!started) return; // not started yet
    
    ofSetColor(255);
    
    resultFbo.draw(calib->getProjectionBox());
    
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
