//
//  projectionInvertedBrush.cpp
//  transparency
//
//  Created by Petr Devaikin on 23.07.18.
//

#include "projectionInvertedBrush.hpp"


projectionInvertedBrush::projectionInvertedBrush(touchArea * t, float t1, float t2, float expSpeed) : baseProjection(t) {
    timer = ofGetElapsedTimef();
    
    thresholdSensetive = t1;
    thresholdBrush = t2;
    expansionSpeed = expSpeed;
    
    touchStarted = false;
    currentlyTouchedLayerIndex = 0;
    
    // zero block
    onesAndZerosAllocated = false;
    
    // load shaders
    shaderThreshold.load("shadersGL3/threshold");
    shaderExpansion.load("shadersGL3/expansion");
    
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
    if (onesAndZerosAllocated) {
        delete[] onesBlock;
        delete[] zerosBlock;
    }
}

void projectionInvertedBrush::addImage(const std::string &imgPath) {
    ofImage newImg;
    newImg.load(imgPath);
    originalImages.push_back(newImg);
}

bool projectionInvertedBrush::setSize(int width, int height) {
    if (baseProjection::setSize(width, height)) {
        // resize images
        scaledImages.clear();
        for (int i = 0; i < originalImages.size(); i++) {
            ofImage newImage;
            newImage.clone(originalImages[i]);
            newImage.resize(width, height);
            scaledImages.push_back(newImage);
        }
        
        // allocate one bytes for check function
        if (onesAndZerosAllocated) {
            delete[] onesBlock;
            delete[] zerosBlock;
        }
        onesAndZerosAllocated = true;
        onesBlock = new unsigned char[4 * width * height]; // rgba * width * height
        zerosBlock = new unsigned char[4 * width * height]; // rgba * width * height
        memset(onesBlock, 255, 4 * width * height);
        memset(zerosBlock, 0, 4 * width * height);
        
        // change buffer size
        workFbo.allocate(width, height, GL_RGBA);
        workFbo.begin();
        ofClear(0);
        workFbo.end();
        
        // prepare touch brush
        touchBrushResized.allocate(2 * width + 1, 2 * height + 1, GL_RGBA);
        touchBrushResized.begin();
        ofClear(0);
        ofSetColor(255);
        touchBrush.draw(width - round(touchBrush.getWidth() / 2) + 1, height - round(touchBrush.getHeight() / 2) + 1);
        touchBrushResized.end();
        
        // reset layers
        resetLayers();
        
        return true;
    }
    else
        return false;
}

// Helpers

ofFbo * projectionInvertedBrush::applyThresholdToTouchFbo(float threshold) {
    // why?
    workFbo.begin();
    ofSetColor(255);
    ofClear(0, 0, 0, 0);
    shaderThreshold.begin();
    shaderThreshold.setUniform1f("threshold", threshold);
    (touch->getDepth()).draw(0, 0);
    shaderThreshold.end();
    workFbo.end();
    return &workFbo;
}

int projectionInvertedBrush::detectLayerIndex(ofFbo * checkArea) {
    // check all layers starting from the top one
    int touchedLayer = 0;
    for (touchedLayer = layers.size() - 1; touchedLayer > 0; touchedLayer--) {
        if (layers[touchedLayer].checkIfTouched(checkArea)) break;
    }
    
    return touchedLayer;
}

bool projectionInvertedBrush::checkIfEmpty(ofFbo * fbo) {
    ofPixels pixels;
    fbo->readToPixels(pixels);
    
    // compare with zeros-array
    return memcmp(pixels.getData(), zerosBlock, 4 * fbo->getWidth() * fbo->getHeight()) == 0;
}

// Update

void projectionInvertedBrush::update() {
    if (!onesAndZerosAllocated) return; // size is not set. not ready
    
    // expand touched areas on all layers
    float currentTime = ofGetElapsedTimef();
    float expansionRadius = expansionSpeed * (currentTime - timer);
    if (expansionRadius >= 1) {
        timer = currentTime;
        
        for (int i = 0; i < layers.size(); i++) {
            layers[i].expand(expansionRadius);
        }
    }
    
    // check if we can remove the last layer
    if (layers.size() > 1 && layers[1].isFull()) {
        layers.erase(layers.begin());
        cout << "Layer removed. Number of layers: " << layers.size() << "\n";
    }
    
    // Check if the canvas is touched right now
    ofFbo * sensitiveAreaFbo = applyThresholdToTouchFbo(thresholdSensetive);
    bool isCurrentlyTouched = !checkIfEmpty(sensitiveAreaFbo);
    
    if (isCurrentlyTouched) { // canvas is touched
        // detect real brush area
        ofFbo * brushAreaFbo = applyThresholdToTouchFbo(thresholdBrush);
        
        // check if it's the first touch
        if (!touchStarted) {
            touchStarted = true;
            currentlyTouchedLayerIndex = detectLayerIndex(brushAreaFbo);
            cout << "Layer touched: " << currentlyTouchedLayerIndex << "\n";
            
            // if touched the top layer, need to add new empty layer
            if (currentlyTouchedLayerIndex == layers.size() - 1) {
                int newLayerImageIndex = (layers[currentlyTouchedLayerIndex].getImageIndex() + 1) % originalImages.size();
                addImageToLayers(newLayerImageIndex);
                cout << "New layer " << newLayerImageIndex << " added. Number of layers: " << layers.size() << "\n";
            }
        }
        
        layers[currentlyTouchedLayerIndex + 1].addTouch(brushAreaFbo);
    }
    else if (touchStarted) { // gesture ended
        touchStarted = false;
    }
}

void projectionInvertedBrush::draw() {
    if (!onesAndZerosAllocated) return; // size is not set. not ready
    
    ofSetColor(255);
    
    for (int i = 0; i < layers.size(); i++) {
        layers[i].draw(position[0], position[1]); // draw with transparency
    }
    
    //touch->getDepth().draw(position[0], position[1]);
    workFbo.draw(position[0], position[1]);
}

void projectionInvertedBrush::resetLayers() {
    layers.clear(); // need to clean memory?
    addImageToLayers(0);
    layers[0].setFull();
    
    cout << "Reset layers. Number of layers: " << layers.size() << "\n";
}

void projectionInvertedBrush::addImageToLayers(int i) {
    layerWithMask newLayer(scaledImages[i], i, onesBlock, shaderExpansion, touchBrushResized);
    layers.push_back(newLayer);
}
