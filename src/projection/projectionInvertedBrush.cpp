//
//  projectionInvertedBrush.cpp
//  transparency
//
//  Created by Petr Devaikin on 23.07.18.
//

#include "projectionInvertedBrush.hpp"

projectionInvertedBrush::projectionInvertedBrush(float t1, float t2) {
    thresholdSensetive = t1;
    thresholdBrush = t2;
    
    touchStarted = false;
    
    // zero block
    zeroAllocated = false;
    
    // load shaders
    shaderThreshold.load("shadersGL3/threshold");
}

bool projectionInvertedBrush::setSize(int width, int height) {
    if (projection::setSize(width, height)) {
        // change buffer size
        resultFbo.allocate(width, height, GL_RGBA);
        resultFbo.begin();
        ofClear(0);
        resultFbo.end();
        
        workFbo.allocate(width, height, GL_RGBA);
        workFbo.begin();
        ofClear(0);
        workFbo.end();
        
        gestureFbo.allocate(width, height, GL_RGBA);
        gestureFbo.begin();
        ofClear(0);
        gestureFbo.end();
        
        // allocate zero bytes for check function
        if (zeroAllocated)
            delete[] zeroBlock;
        zeroAllocated = true;
        zeroBlock = new unsigned char[4 * width * height]; // rgba * width * height
        
        return true;
    }
    else
        return false;
}

projectionInvertedBrush::~projectionInvertedBrush() {
    // clean the memory
    if (zeroAllocated)
        delete[] zeroBlock;
}

// Helpers

bool projectionInvertedBrush::checkIfEmpty(ofFbo * fbo) {
    ofPixels pixels;
    fbo->readToPixels(pixels);
    
    return memcmp(pixels.getData(), zeroBlock, sizeof(*zeroBlock));
}

void projectionInvertedBrush::applyThresholdToWorkFbo(float threshold) {
    workFbo.begin();
    shaderThreshold.begin();
    shaderThreshold.setUniform1f("threshold", threshold);
    (touch->getDepth()).draw(0, 0);
    shaderThreshold.end();
    workFbo.end();
}

bool projectionInvertedBrush::detectColor(ofFbo * mask) {
    // detects color of the result fbo in mask area
    ofPixels maskPixels;
    ofPixels resultPixels;
    
    resultFbo.readToPixels(resultPixels);
    mask->readToPixels(maskPixels);
    
    unsigned char * maskPointer = resultPixels.getData();
    unsigned char * resultPointer = maskPixels.getData();
    
    int zeroCount = 0;
    int oneCount = 0;
    
    
    for (int i = 0; i < size[0] * size[1]; i++) { // go through pixels and count 0's and 1's
        if (* (maskPointer + 4 * i)) { // if mask pixel is not 0
            if (* (resultPointer + 4 * i)) // if result pixel is not 0
                oneCount++;
            else
                zeroCount++;
        }
    }
    
    return oneCount > zeroCount;
}

// Update

void projectionInvertedBrush::update() {
    // Check if the canvas is touched right now
    applyThresholdToWorkFbo(thresholdSensetive);
    bool isCurrentlyTouched = !checkIfEmpty(&workFbo);
    
    if (isCurrentlyTouched) { // canvas is touched
        // detect real brush area
        applyThresholdToWorkFbo(thresholdBrush);
        
        // check if it's the first touch
        if (!touchStarted) {
            touchStarted = true;
            resultColor = !detectColor(&workFbo);
            cleanGestureBuffer();
        }
        //addTouchToBuffer(&workFbo);
    }
    else if (touchStarted) { // gesture ended
        applyGesture();
    }
}

void projectionInvertedBrush::draw() {
    ofSetColor(255);
    resultFbo.draw(position[0], position[1]);
    
    // try to draw brush
    // gestureFbo.draw(0, 0);
}

// gesture

void projectionInvertedBrush::cleanGestureBuffer() {
    // Clean the gesture buffer
    gestureFbo.begin();
    ofClear(0, 0, 0, 0);
    gestureFbo.end();
}

void projectionInvertedBrush::addTouchToGesture(ofFbo * touch) {
    gestureFbo.begin();
    touch->draw(0, 0);
    gestureFbo.end();
}

void projectionInvertedBrush::applyGesture() {
    resultFbo.begin();
    if (resultColor)
        ofEnableBlendMode(OF_BLENDMODE_ADD);
    else
        ofEnableBlendMode(OF_BLENDMODE_SUBTRACT);
    gestureFbo.draw(0, 0);
    ofDisableBlendMode();
    resultFbo.end();
}
