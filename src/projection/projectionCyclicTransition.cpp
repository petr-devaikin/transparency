//
//  projectionCyclicTransition.cpp
//  transparency
//
//  Created by Petr Devaikin on 22.07.18.
//

#include "projectionCyclicTransition.hpp"

projectionCyclicTransition::projectionCyclicTransition() {
    lastUpdate = 0;
    cyclicShader.load("shadersGL3/adding_cyclic_shader");
    peakTransitionShader.load("shadersGL3/peak_transition_shader");
    blurXShader.load("shadersGL3/shaderBlurX");
    blurYShader.load("shadersGL3/shaderBlurY");
}

bool projectionCyclicTransition::setSize(int width, int height) {
    if (projectionFading::setSize(width, height)) {
        
        // reallocate buffers
        currentStateFbo.allocate(width, height, GL_RGBA);
        currentStateFbo.begin();
        {
            ofClear(0, 0, 0, 255);
        }
        currentStateFbo.end();
        
        resultFbo.allocate(width, height, GL_RGBA);
        resultFbo.begin();
        {
            ofClear(0, 0, 0, 255);
        }
        resultFbo.end();
        
        return true;
    }
    else
        return false;
}

void projectionCyclicTransition::update() {
    projectionFading::update();
    
    // preparing coef for shader
    float currentTime = ofGetElapsedTimef();
    float timeCoef = 0;
    if (lastUpdate > 0) timeCoef = (currentTime - lastUpdate) / CYCLE_PERIOD;
    lastUpdate = currentTime;
    
    // update current state. use resultFbo as a temp buffer
    resultFbo.begin();
    {
        // ofClear(0, 0, 0);
        ofSetColor(255);
        
        cyclicShader.begin();
        {
            cyclicShader.setUniform1f("timeCoef", timeCoef);
            cyclicShader.setUniformTexture("currentState", currentStateFbo.getTexture(), 1);
            
            maskFbo.draw(0, 0);
        }
        cyclicShader.end();
    }
    resultFbo.end();
    
    // save current state
    currentStateFbo.begin();
    {
        ofSetColor(255);
        resultFbo.draw(0, 0);
    }
    currentStateFbo.end();
    
    // final drawing
    resultFbo.begin();
    {
        ofClear(0, 0, 0, 0);
        ofSetColor(255);
        
        rainbowShader.begin();
        {
            //rainbowShader.setUniform1f("scaleCoef", 1);
            currentStateFbo.draw(0, 0);
        }
        rainbowShader.end();
    }
    resultFbo.end();
}

void projectionCyclicTransition::draw() {
    ofSetColor(255);
    resultFbo.draw(position[0], position[1]);
}
