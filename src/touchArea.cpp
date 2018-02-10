//
//  touchArea.cpp
//  transparency
//
//  Created by Petr Devaikin on 10.02.18.
//

#include "touchArea.h"

touchArea::touchArea() {
    borderPoints.push_back(ofVec2f(100, 300));
    borderPoints.push_back(ofVec2f(200, 290));
    borderPoints.push_back(ofVec2f(220, 500));
    borderPoints.push_back(ofVec2f(80, 540));
    
    depthImage.allocate(300, 600, OF_IMAGE_COLOR);
}

void touchArea::drawBorder() {
    ofSetColor(ofColor::blue);
    ofNoFill();
    ofPolyline border;
    for (int i = 0; i < borderPoints.size(); i++) {
        border.addVertex(borderPoints[i]);
    }
    border.close();
    border.draw();
}

void touchArea::draw() {
    depthImage.bind();
    depthImage.draw(0, 0);
    depthImage.unbind();
    //drawBorder();
}

ofImage touchArea::getDepthImage() {
    return depthImage;
}

vector<ofVec2f> touchArea::getBorderPoints() {
    return borderPoints;
}

void touchArea::imitateTouch(int x, int y) {
    for (int i = x - 10; i < x + 10; i++)
        for (int j = y - 10; j < y + 10; j++)
            depthImage.setColor(i, j, ofColor::white);
    
    depthImage.update();
}

void touchArea::imitateRelease() {
}
