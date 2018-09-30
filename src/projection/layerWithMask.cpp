//
//  layerWithMask.cpp
//  transparency
//
//  Created by Petr Devaikin on 30.09.18.
//

#include "layerWithMask.hpp"

layerWithMask::layerWithMask(ofImage img, int imageIndex, unsigned char * oneBlock, ofShader shdr) {
    ones = oneBlock;
    image = img;
    _isFull = false;
    _imageIndex = imageIndex;
    
    mask.allocate(img.getWidth(), img.getHeight(), GL_RGBA);
    tempFbo.allocate(img.getWidth(), img.getHeight(), GL_RGBA);
    
    shaderExp = shdr;
}

void layerWithMask::setFull() {
    _isFull = true; // not necessary to really set mask to 1, because this value is checked first in isFull();
}

bool layerWithMask::isFull() {
    if (_isFull) return true;
    
    ofPixels pixels;
    mask.readToPixels(pixels);
    
    // compare with ones-array
    _isFull = memcmp(pixels.getData(), ones, 4 * mask.getWidth() * 4 * mask.getHeight()) == 0;
    return _isFull;
}

bool layerWithMask::checkIfTouched(ofFbo * checkArea) {
    // detects color of the mask fbo in check area
    ofPixels maskPixels;
    ofPixels checkAreaPixels;
    
    mask.readToPixels(maskPixels);
    checkArea->readToPixels(checkAreaPixels);
    
    unsigned char * checkAreaPointer = checkAreaPixels.getData();
    unsigned char * maskPointer = maskPixels.getData();
    
    int zeroCount = 0;
    int oneCount = 0;
    
    for (int i = 0; i < mask.getWidth() * mask.getHeight(); i++) { // go through pixels and count 0's and 1's
        if (* (checkAreaPointer + 4 * i)) { // if check area pixel is not 0
            if (* (maskPointer + 4 * i)) // if mask pixel is not 0
                oneCount++;
            else
                zeroCount++;
        }
    }
    
    return oneCount > zeroCount;
}

void layerWithMask::addTouch(ofFbo *touch) {
    mask.begin();
    ofEnableBlendMode(OF_BLENDMODE_ADD);
    touch->draw(0, 0);
    ofDisableBlendMode();
    mask.end();
}

int layerWithMask::getImageIndex() {
    return _imageIndex;
}

void layerWithMask::expand(float radius) {
    if (isFull()) return; // if layer is already full, don't need this step
    
    tempFbo.begin();
    shaderExp.begin();
    shaderExp.setUniform1f("expansionRadius", radius);
    mask.draw(0, 0);
    shaderExp.end();
    tempFbo.end();
    
    mask.begin();
    tempFbo.draw(0, 0);
    mask.end();
}

void layerWithMask::draw(float x, float y) {
    //image.getTexture().setAlphaMask(mask.getTexture());
    image.draw(x, y);
}
