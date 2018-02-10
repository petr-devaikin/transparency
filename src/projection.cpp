//
//  projection.cpp
//  transparency
//
//  Created by Petr Devaikin on 10.02.18.
//

#include "projection.h"

projection::projection() {
    alpha.allocate(1000, 1000, OF_IMAGE_COLOR);
}

void projection::setPosition(int x, int y) {
    position.set(x, y);
}

void projection::setSize(int width, int height) {
    size.set(width, height);
}

void projection::setX(int x) {
    position[0] = x;
}

void projection::setY(int y) {
    position[1] = y;
}

void projection::setWidth(int width) {
    size[0] = width;
}

void projection::setHeight(int height) {
    size[1] = height;
}

ofVec2f projection::getPosition() {
    return position;
}

ofVec2f projection::getSize() {
    return size;
}

void projection::setTouchArea(touchArea t) {
    touch = t;
}

void projection::updateMapping() {
    vector<ofVec2f> touchPoiunts = touch.getBorderPoints();
    
    vector<cv::Point2f> srcPoints, dstPoints;
    for (int i = 0; i < 4; i++) {
        srcPoints.push_back(cv::Point2f(touchPoiunts[i][0], touchPoiunts[i][1]));
    }
    
    dstPoints.push_back(cv::Point2f(position[0], position[1]));
    dstPoints.push_back(cv::Point2f(position[0] + size[0], position[1]));
    dstPoints.push_back(cv::Point2f(position[0] + size[0], position[1] + size[1]));
    dstPoints.push_back(cv::Point2f(position[0], position[1] + size[1]));
    
    //homography = cv::findHomography(srcPoints, dstPoints);
}

void projection::drawBorder() {
    //ofxCv::warpPerspective(touch.getDepthImage(), alpha, homography, CV_INTER_LINEAR);
    
    ofSetColor(ofColor::red);
    ofNoFill();
    ofDrawRectangle(position[0], position[1], size[0], size[1]);
}
