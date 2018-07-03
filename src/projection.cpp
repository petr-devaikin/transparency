//
//  projection.cpp
//  transparency
//
//  Created by Petr Devaikin on 10.02.18.
//

#include "projection.h"

projection::projection() {
    size.set(0, 0);
    position.set(0, 0);
    
    // load shaders
    maxShader.load("shadersGL3/max_shader");
    finalShader.load("shadersGL3/shader");
}

void projection::init(const string& img_1_path, const string& img_2_path) {
    lastMaskUpdate = ofGetElapsedTimef();
    
    // load images
    image1_original.load(img_1_path);
    image2_original.load(img_2_path);
}

void projection::setPosition(int x, int y) {
    position.set(x, y);
}

void projection::setSize(int width, int height) {
    if ((width != size[0] && width > 0) || (height != size[1] && height > 0)) {
        size.set(width, height);
        
        // upload image again!
        image1.clone(image1_original);
        image2.clone(image2_original);
        image1.resize(width, height);
        image2.resize(width, height);
        
        // reallocate buffer
        resultFbo.allocate(width, height, GL_RGBA);
        resultFbo.begin();
        {
            ofClear(0, 0, 0, 255);
        }
        resultFbo.end();
        
        maskFbo.allocate(width, height, GL_RGBA);
        maskFbo.begin();
        {
            ofClear(0, 0, 0, 255);
        }
        maskFbo.end();
        
        tempFbo.allocate(width, height, GL_RGBA);
        tempFbo.begin();
        {
            ofClear(0, 0, 0, 255);
        }
        tempFbo.end();
    }
}

void projection::setX(int x) {
    setPosition(x, position[1]);
}

void projection::setY(int y) {
    setPosition(position[0], y);
}

void projection::setWidth(int width) {
    setSize(width, size[1]);
}

void projection::setHeight(int height) {
    setSize(size[0], height);
}

int projection::getWidth() {
    return size[0];
}

int projection::getHeight() {
    return size[1];
}

ofVec2f projection::getPosition() {
    return position;
}

ofVec2f projection::getSize() {
    return size;
}

void projection::setTouchArea(touchArea * t) {
    touch = t;
}

void projection::drawBorder() {
    ofSetColor(255, 255, 255);
    ofNoFill();
    ofDrawRectangle(position[0], position[1], size[0], size[1]);
}

void projection::update() {
    // fading old mask
    float currentTime = ofGetElapsedTimef();
    float timeDiff = currentTime - lastMaskUpdate;
    float fadeLevel = timeDiff / FADE_PERIOD;
    lastMaskUpdate = currentTime;
    
    tempFbo.begin();
    {
        ofClear(0, 0, 0, 255);
        ofSetColor(255);
        maskFbo.draw(0, 0);
    }
    tempFbo.end();
    
    // add current touch to mask
    maskFbo.begin();
    {
        ofClear(0, 0, 0, 255);
        ofSetColor(255);
        maxShader.begin();
        {
            maxShader.setUniformTexture("oldTex", tempFbo.getTexture(), 1);
            maxShader.setUniform1f("fadeLevel", fadeLevel);
            (touch->getDepth()).draw(0, 0);
        }
        maxShader.end();
    }
    maskFbo.end();
    
    // update images // why???
    image1.update();
    image2.update();
    

    // final drawing
    resultFbo.begin();
    {
        ofClear(0, 0, 0, 0);
        ofSetColor(255);
        
        //maskFbo.draw(0, 0); // to test how mask is transformed
        
        finalShader.begin();
        {
            finalShader.setUniformTexture("bgTex", image1.getTexture(), 1);
            finalShader.setUniformTexture("maskTex", maskFbo.getTexture(), 2);
            image2.draw(0, 0);
        }
        finalShader.end();
    }
    resultFbo.end();
}

void projection::draw() {
    ofSetColor(255);
    resultFbo.draw(position[0], position[1]);
}
