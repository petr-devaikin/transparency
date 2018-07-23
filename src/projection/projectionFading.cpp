//
//  projection.cpp
//  transparency
//
//  Created by Petr Devaikin on 10.02.18.
//

#include "projectionFading.h"

projectionFading::projectionFading(float _blur) {
    lastMaskUpdate = ofGetElapsedTimef();
    
    blur = _blur;
    
    size.set(0, 0);
    position.set(0, 0);
    
    // load shaders
    maxShader.load("shadersGL3/max_shader");
    finalShader.load("shadersGL3/shader");
    
    if (blur > 0) {
        shaderBlurX.load("shadersGL3/shaderBlurX");
        shaderBlurY.load("shadersGL3/shaderBlurY");
    }
}

void projectionFading::setPosition(int x, int y) {
    position.set(x, y);
}

bool projectionFading::setSize(int width, int height) {
    if ((width != size[0] && width > 0) || (height != size[1] && height > 0)) {
        size.set(width, height);
        
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
        
        return true;
    }
    else
        return false;
}

void projectionFading::setX(int x) {
    setPosition(x, position[1]);
}

void projectionFading::setY(int y) {
    setPosition(position[0], y);
}

void projectionFading::setWidth(int width) {
    setSize(width, size[1]);
}

void projectionFading::setHeight(int height) {
    setSize(size[0], height);
}

int projectionFading::getWidth() {
    return size[0];
}

int projectionFading::getHeight() {
    return size[1];
}

ofVec2f projectionFading::getPosition() {
    return position;
}

ofVec2f projectionFading::getSize() {
    return size;
}

void projectionFading::setTouchArea(touchArea * t) {
    touch = t;
}

void projectionFading::drawBorder() {
    ofSetColor(255, 0, 0);
    ofNoFill();
    ofDrawRectangle(position[0], position[1], size[0], size[1]);
}

void projectionFading::update() {
    // fading old mask
    float currentTime = ofGetElapsedTimef();
    float timeDiff = currentTime - lastMaskUpdate;
    float fadeLevel = timeDiff / FADE_PERIOD;
    lastMaskUpdate = currentTime;
    
    tempFbo.begin();
    {
        ofClear(0, 0, 0, 255);
        ofSetColor(255);
        
        if (blur > 0) {
            shaderBlurX.begin();
            shaderBlurX.setUniform1f("blurAmnt", blur);
        }
        maskFbo.draw(0, 0);
        if (blur > 0) {
            shaderBlurX.end();
        }
    }
    tempFbo.end();
    
    // do vertical blur if needed
    if (blur > 0) {
        maskFbo.begin();
        {
            shaderBlurY.begin();
            shaderBlurY.setUniform1f("blurAmnt", blur);
            
            tempFbo.draw(0, 0);
            
            shaderBlurY.end();
        }
        maskFbo.end();
        
        // copy result back
        tempFbo.begin();
        maskFbo.draw(0, 0);
        tempFbo.end();
    }
    
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
}

void projectionFading::draw() {
    ofSetColor(255);
    maskFbo.draw(position[0], position[1]);
}
