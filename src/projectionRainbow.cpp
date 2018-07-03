//
//  projection.cpp
//  transparency
//
//  Created by Petr Devaikin on 10.02.18.
//

#include "projectionRainbow.h"

projectionRainbow::projectionRainbow() {
    rainbowShader.load("shadersGL3/rainbow_shader");
}

void projectionRainbow::setSize(int width, int height) {
    if ((width != size[0] && width > 0) || (height != size[1] && height > 0)) {
        
        // reallocate buffer
        resultFbo.allocate(width, height, GL_RGBA);
        resultFbo.begin();
        {
            ofClear(100, 0, 0, 255);
        }
        resultFbo.end();
    }
    
    projectionFading::setSize(width, height);
}


void projectionRainbow::update() {
    projectionFading::update();
    
    // final drawing
    resultFbo.begin();
    {
        ofClear(0, 0, 0, 0);
        ofSetColor(255);
        
        rainbowShader.begin();
        {
            maskFbo.draw(0, 0);
        }
        rainbowShader.end();
    }
    resultFbo.end();
}

void projectionRainbow::draw() {
    ofSetColor(255);
    resultFbo.draw(position[0], position[1]);
}
