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
    transpShader.load("shadersGL3/shader");
}

void projectionInvertedBrush::init(const string& img_1_path, const string& img_2_path) {
    // load images
    image1_original.load(img_1_path);
    image2_original.load(img_2_path);
}

bool projectionInvertedBrush::setSize(int width, int height) {
    if (baseProjection::setSize(width, height)) {
        // update image
        image1.clone(image1_original);
        image2.clone(image2_original);
        image1.resize(width, height);
        image2.resize(width, height);
        
        // change buffer size
        resultFbo.allocate(width, height, GL_RGBA);
        resultFbo.begin();
        ofClear(0);
        resultFbo.end();
        
        workFbo.allocate(width, height, GL_RGBA);
        workFbo.begin();
        ofClear(0);
        workFbo.end();
        
        // allocate zero bytes for check function
        if (zeroAllocated)
            delete[] zeroBlock;
        zeroAllocated = true;
        zeroSize = 4 * width * height;
        zeroBlock = new unsigned char[zeroSize]; // rgba * width * height
        memset(zeroBlock, 0, zeroSize);
        
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

    return memcmp(pixels.getData(), zeroBlock, zeroSize) == 0;
}

void projectionInvertedBrush::applyThresholdToWorkFbo(float threshold) {
    workFbo.begin();
    ofSetColor(255);
    ofClear(0, 0, 0, 0);
    shaderThreshold.begin();
    shaderThreshold.setUniform1f("threshold", threshold);
    (touch->getDepth()).draw(0, 0);
    shaderThreshold.end();
    workFbo.end();
}

bool lastColor = true;
bool projectionInvertedBrush::detectColor(ofFbo * mask) {
    lastColor = !lastColor;
    //return lastColor;
    // detects color of the result fbo in mask area
    ofPixels maskPixels;
    ofPixels resultPixels;
    
    resultFbo.readToPixels(resultPixels);
    mask->readToPixels(maskPixels);
    
    unsigned char * maskPointer = maskPixels.getData();
    unsigned char * resultPointer = resultPixels.getData();
    
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
            cout << "delected " << resultColor << "\n";
        }
        applyTouch(&workFbo);
    }
    else if (touchStarted) { // gesture ended
        touchStarted = false;
    }
}

void projectionInvertedBrush::draw() {
    ofSetColor(255);
    
    transpShader.begin();
    transpShader.setUniformTexture("bgTex", image1.getTexture(), 1);
    transpShader.setUniformTexture("maskTex", resultFbo.getTexture(), 2);
    
    image2.draw(position[0], position[1]);
    
    transpShader.end();
    
    // try to draw brush
    // gestureFbo.draw(0, 0);
}

// gesture

void projectionInvertedBrush::applyTouch(ofFbo * touch) {
    resultFbo.begin();
    if (resultColor) {
        ofEnableBlendMode(OF_BLENDMODE_ADD);
        //cout << "substract\n";
    }
    else {
        ofEnableBlendMode(OF_BLENDMODE_SUBTRACT);
        //cout << "substract\n";
    }
    touch->draw(0, 0);
    ofDisableBlendMode();
    resultFbo.end();
}
