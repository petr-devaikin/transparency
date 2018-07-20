//
//  projection.cpp
//  transparency
//
//  Created by Petr Devaikin on 10.02.18.
//

#include "projectionSimpleShader.h"

projectionSimpleShader::projectionSimpleShader(string filename) {
    shader.load(filename);
}

void projectionSimpleShader::setSize(int width, int height) {
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


void projectionSimpleShader::prepareShader() {
    // shader. ... set some params if needed
}


void projectionSimpleShader::update() {
    projectionFading::update();
    
    // final drawing
    resultFbo.begin();
    {
        ofClear(0, 0, 0, 0);
        ofSetColor(255);
        
        shader.begin();
        {
            prepareShader();
            maskFbo.draw(0, 0);
        }
        shader.end();
    }
    resultFbo.end();
}

void projectionSimpleShader::draw() {
    ofSetColor(255);
    resultFbo.draw(position[0], position[1]);
}
