//
//  layerWithMask.cpp
//  transparency
//
//  Created by Petr Devaikin on 30.09.18.
//

#include "layerWithMask.hpp"

layerWithMask::layerWithMask(ofImage img, int imageIndex, unsigned char * oneBlock, ofShader shdrE, ofShader shdrA, ofFbo touchBr) {
    ones = oneBlock;
    image = img;
    _isFull = false;
    _imageIndex = imageIndex;
    
    mask.allocate(img.getWidth(), img.getHeight(), GL_RGBA);
    mask.begin();
    ofClear(255, 255, 0, 255);
    // r and g - luminosity
    // b - if active
    // a - not used
    mask.end();
    
    tempFbo.allocate(img.getWidth(), img.getHeight(), GL_RGBA);
    
    touchBrush = touchBr;
    shaderExp = shdrE;
    shaderExpAdder = shdrA;
}

void layerWithMask::setFull() {
    _isFull = true; // not necessary to really set mask to 1, because this value is checked first in isFull();
}

bool layerWithMask::isFull() {
    if (_isFull) return true;
    
    ofPixels pixels;
    mask.readToPixels(pixels);
    
    // compare with ones-array
    _isFull = memcmp(pixels.getChannel(2).getData(), ones, mask.getWidth() * mask.getHeight()) == 0;
    return _isFull;
}

bool layerWithMask::checkIfTouched(ofPoint point) {
    // detects color of the mask fbo in check area
    ofPixels maskPixels;
    
    mask.readToPixels(maskPixels);
    return maskPixels.getColor(point[0], point[1]).b == 255;
}

void layerWithMask::addTouch(ofPoint point) {
    tempFbo.begin();
    ofClear(0, 0, 0, 0);
    ofSetColor(255, 255, 255, 255);
    
    shaderExpAdder.begin();
    shaderExpAdder.setUniformTexture("mask", mask.getTexture(), 1);
    shaderExpAdder.setUniform2f("touchPoint", round(point[0] - touchBrush.getWidth() / 2), round(point[1] - touchBrush.getHeight() / 2));
    touchBrush.draw(0, 0);
    
    shaderExpAdder.end();
    tempFbo.end();
    
    mask.begin();
    ofClear(0, 0, 0, 0);
    tempFbo.draw(0, 0);
    mask.end();
}

int layerWithMask::getImageIndex() {
    return _imageIndex;
}

void layerWithMask::expand(float radius) {
    if (isFull()) return; // if layer is already full, don't need this step
    
    tempFbo.begin();
    ofClear(0, 0, 0, 0);
    shaderExp.begin();
    shaderExp.setUniform1f("expansionRadius", radius);
    ofSetColor(255);
    mask.draw(0, 0);
    shaderExp.end();
    tempFbo.end();
    
    mask.begin();
    ofClear(0, 0, 0, 0);
    ofSetColor(255);
    tempFbo.draw(0, 0);
    mask.end();
}

void layerWithMask::draw(float x, float y) {
    if (!isFull())
        image.getTexture().setAlphaMask(mask.getTexture());
    image.draw(x, y);
    
    mask.draw(x + 300, y);
}