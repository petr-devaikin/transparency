//
//  projection.cpp
//  transparency
//
//  Created by Petr Devaikin on 10.02.18.
//

#include "projectionFlowers.h"

Flower::Flower(float _maxSize, float _growSpeed, float _decreaseSpeed) {
    position = ofVec2f(0, 0);
    currentSize = 0;
    targetSize = 0;
    
    maxSize = _maxSize;
    growSpeed = _growSpeed;
    decreaseSpeed = _decreaseSpeed;
    
    lastUpdateTime = ofGetElapsedTimef();
}

void Flower::setPosition(float x, float y) {
    position.set(x, y);
}

void Flower::update(ofPixels image) {
    float currentTime = ofGetElapsedTimef();
    
    if (currentTime == lastUpdateTime) return; // can it ever happen?
    
    // update target size from image
    targetSize = maxSize * image.getColor(position[0], position[1]).r / 255.0;
    
    // animate the flower
    float sizeDifference = targetSize - currentSize;
    float speed = sizeDifference > 0 ? growSpeed : -decreaseSpeed;
    
    if (speed > 0) {
        currentSize += speed * (currentTime - lastUpdateTime);
        if (currentSize < 0) currentSize = 0;
        if (currentSize > targetSize) currentSize = targetSize;
    }
    else
        currentSize = targetSize;
    
    lastUpdateTime = currentTime;
}

void Flower::draw() {
    ofSetColor(255, 255, 255, 150);
    ofFill();
    ofDrawCircle(position[0], position[1], currentSize / 2);
}


EmojiFlower::EmojiFlower(float _maxSize, float _growSpeed, float _decreaseSpeed) : Flower::Flower(_maxSize, _growSpeed, _decreaseSpeed) {
    img.load("cherry.png");
}

void EmojiFlower::draw() {
    if (currentSize <= 0) return;
    
    ofSetColor(255, 255, 255, 255);
    ofFill();
    img.draw(position[0] - currentSize / 2,
             position[1] - currentSize / 2,
             currentSize,
             currentSize);
}

// ------------



projectionFlowers::projectionFlowers(int flowerNumber) {
    for (int i = 0; i < flowerNumber; i++) {
        flowers.push_back(EmojiFlower(100, 0, 4)); // flowers, max 100px, growth: 0 pix/sec, decrease: 4 px/sec
    }
}

void projectionFlowers::setSize(int width, int height) {
    if ((width != size[0] && width > 0) || (height != size[1] && height > 0)) {
        // relocate flowers
        for (int i = 0; i < flowers.size(); i++)
            flowers[i].setPosition(ofRandom(width), ofRandom(height));
        
        // reallocate buffer // forgot to do it in the beginning!!
        resultFbo.allocate(width, height, GL_RGBA);
        resultFbo.begin();
        {
            ofClear(100, 0, 0, 255);
        }
        resultFbo.end();
    }
    
    projectionFading::setSize(width, height);
}


void projectionFlowers::update() {
    projectionFading::update();
    
    ofPixels maskImage;
    maskFbo.readToPixels(maskImage);
    
    // final drawing
    resultFbo.begin();
    {
        ofClear(0, 0, 0, 0);
        ofSetColor(255);
        
        for (int i = 0; i < flowers.size(); i++) {
            // update flowers
            flowers[i].update(maskImage);
            flowers[i].draw();
        }
    }
    resultFbo.end();
}

void projectionFlowers::draw() {
    ofSetColor(255);
    resultFbo.draw(position[0], position[1]);
}
