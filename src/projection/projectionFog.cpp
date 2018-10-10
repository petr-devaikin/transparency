//
//  projectionFog.cpp
//  transparency
//
//  Created by Petr Devaikin on 10.10.18.
//

#include "projectionFog.hpp"

projectionFog::projectionFog(touchArea * t, float _maxFogDepth, float _depthBlur, float _fadePeriod) : baseProjection(t) {
    maxFogDepth = _maxFogDepth;
    fadePeriod = _fadePeriod;
    depthBlur = _depthBlur;
    
    // shaders
    maxAndFadeShader.load("shadersGL3/maxAndFade");
    depthCutShader.load("shadersGL3/depthCut");
    
    timer = ofGetElapsedTimef();
}

bool projectionFog::setSize(int width, int height) {
    if (baseProjection::setSize(width, height)) {
        return true;
        
        // prepare fbo's
        work.allocate(width, height, GL_RGBA);
        work.begin();
        ofClear(0, 0, 0, 255);
        work.end();
        
        mask.allocate(width, height, GL_RGBA);
        mask.begin();
        ofClear(0, 0, 0, 255);
        mask.end();
        
        result.allocate(width, height, GL_RGBA);
        result.begin();
        ofClear(0, 0, 0, 255);
        result.end();
    }
    else
        return false;
}

void projectionFog::update(ofImage rgbFrame, ofImage depthFrame, ofImage depthZeroLevel) {
    // update current mask
    updateMask();
    
    // update result
    result.begin();
    ofClear(0, 0, 0, 255);
    ofSetColor(255);
    depthCutShader.begin();
    
    depthCutShader.setUniform1f("depthBlur", depthBlur);
    depthCutShader.setUniformTexture("cutMask", mask.getTexture(), 1);
    depthCutShader.setUniformTexture("depth", depthFrame.getTexture(), 2);
    depthCutShader.setUniformTexture("depthZero", depthZeroLevel.getTexture(), 3);
    
    rgbFrame.draw();
    
    depthCutShader.end();
    result.end();
}

void projectionFog::updateMask() {
    // calculate fading step
    float currentTime = ofGetElapsedTimef();
    float step = 255 * (currentTime - timer) / fadePeriod;
    float delta = floor(step) - step;
    step = floor(step);
    timer = currentTime - delta / 255 * fadePeriod; // go back for a bit
    
    // update mask
    work.begin();
    ofClear(0, 0, 0, 255);
    ofSetColor(255);
    maxAndFadeShader.begin();
    maxAndFadeShader.setUniformTexture("mask", mask.getTexture(), 1);
    maxAndFadeShader.setUniform1i("fadeStep", step);
    (touch->getDepth()).draw(0, 0);
    maxAndFadeShader.end();
    work.end();
    
    mask.begin();
    ofClear(0, 0, 0, 255);
    ofSetColor(255);
    work.draw(0, 0);
    mask.end();
}
