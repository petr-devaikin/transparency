//
//  touchArea.cpp
//  transparency
//
//  Created by Petr Devaikin on 10.02.18.
//

#include "touchArea.h"

touchArea::touchArea() {
    zeroLevelPixels.allocate(WIDTH, HEIGHT, OF_IMAGE_GRAYSCALE);
    zeroLevelPixels.clear();
    
    brush.load("brush.png");
    brush.setImageType(OF_IMAGE_GRAYSCALE);
    
    depthFbo.allocate(WIDTH, HEIGHT, GL_RGBA);
    depthFbo.begin();
    {
        ofClear(0, 0, 0, 255);
    }
    depthFbo.end();
}

void touchArea::recognizeBorders() {
    borderPoints.push_back(ofVec2f(100, 300));
    borderPoints.push_back(ofVec2f(200, 290));
    borderPoints.push_back(ofVec2f(220, 500));
    borderPoints.push_back(ofVec2f(80, 540));
    
    // also setup zero level
    zeroLevelPixels.clear();
    
    // setup min and max depth for kinect!
}

void touchArea::drawBorder() {
    ofSetColor(0, 255, 0);
    ofNoFill();
    ofPolyline border;
    for (int i = 0; i < borderPoints.size(); i++) {
        border.addVertex(borderPoints[i]);
    }
    border.close();
    border.draw();
}

void touchArea::draw() {
    ofSetColor(255);
    depthFbo.draw(0, 0);
    drawBorder();
}

ofFbo & touchArea::getDepth() {
    return depthFbo;
}

vector<ofVec2f> touchArea::getBorderPoints() {
    return borderPoints;
}

void touchArea::imitateTouch(int x, int y) {
    depthFbo.begin();
    {
        ofClear(0, 0, 0, 255);
        ofSetColor(255);
        brush.draw(x - brush.getWidth(), y - brush.getHeight(), brush.getWidth() * 2, brush.getHeight() * 2);
    }
    depthFbo.end();
}

void touchArea::imitateRelease() {
    depthFbo.begin();
    {
        ofClear(0, 0, 0, 255);
    }
    depthFbo.end();
}
