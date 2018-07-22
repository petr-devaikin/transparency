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

bool projectionSimpleShader::setSize(int width, int height) {
    if (projectionFading::setSize(width, height)) {
        
        // reallocate buffer
        resultFbo.allocate(width, height, GL_RGBA);
        resultFbo.begin();
        {
            ofClear(100, 0, 0, 255);
        }
        resultFbo.end();
        return true;
    }
    else
        return false;
}


void projectionSimpleShader::prepareShader() {
    // shader. ... set some params if needed
    shader.setUniform1f("scaleCoef", 5 / 6); // to start from red and to end with blue (?)
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
