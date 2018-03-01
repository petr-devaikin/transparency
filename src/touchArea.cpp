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
    
    borderPoints.push_back(ofVec2f(0, 0));
    borderPoints.push_back(ofVec2f(0, 0));
    borderPoints.push_back(ofVec2f(0, 0));
    borderPoints.push_back(ofVec2f(0, 0));
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

void touchArea::drawBorder(int x, int y) {
    ofSetColor(0, 255, 0);
    ofNoFill();
    ofPolyline border;
    for (int i = 0; i < borderPoints.size(); i++) {
        border.addVertex(borderPoints[i] + ofVec2f(x, y));
    }
    border.close();
    border.draw();
    
    ofNoFill();
    ofSetColor(100, 100, 100);
    ofDrawRectangle(x, y, WIDTH, HEIGHT);
}

void touchArea::draw(int x, int y) {
    float yOffset = (ofGetHeight() - HEIGHT) / 2;
    
    ofSetColor(255, 255, 255, 200);
    depthFbo.draw(x, y, WIDTH, HEIGHT);
}

ofFbo & touchArea::getDepth() {
    return depthFbo;
}

vector<ofVec2f> touchArea::getBorderPoints() {
    return borderPoints;
}

void touchArea::setBorderPoint(int i, float x, float y) {
    borderPoints[i].set(x, y);
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
