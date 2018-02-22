//
//  projection.cpp
//  transparency
//
//  Created by Petr Devaikin on 10.02.18.
//

#include "projection.h"

using namespace cv;
using namespace ofxCv;

projection::projection() {
    //alpha.allocate(1000, 1000, OF_IMAGE_COLOR);
    homography = Mat(4, 4, CV_32F);
    homography.at<float>(2, 0) = homography.at<float>(2, 1) = homography.at<float>(2, 3) = 0;
    homography.at<float>(0, 2) = homography.at<float>(1, 2) = homography.at<float>(3, 2) = 0;
    homography.at<float>(2, 2) = homography.at<float>(3, 3) = 1;
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

void projection::setTouchArea(touchArea * t) {
    touch = t;
}

void projection::updateMapping() {
    vector<ofVec2f> touchPoiunts = touch->getBorderPoints();
    
    vector<Point2f> srcPoints, dstPoints;
    for (int i = 0; i < 4; i++) {
        srcPoints.push_back(Point2f(touchPoiunts[i][0], touchPoiunts[i][1]));
    }
    
    dstPoints.push_back(Point2f(position[0], position[1]));
    dstPoints.push_back(Point2f(position[0] + size[0], position[1]));
    dstPoints.push_back(Point2f(position[0] + size[0], position[1] + size[1]));
    dstPoints.push_back(Point2f(position[0], position[1] + size[1]));

    Mat m3x3 = findHomography(srcPoints, dstPoints);
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++) {
            int ii = i < 2 ? i : i + 1;
            int jj = j < 2 ? j : j + 1;
            homography.at<float>(ii, jj) = m3x3.at<double>(j, i);
        }
}

void projection::drawBorder() {
    ofSetColor(ofColor::grey);
    ofNoFill();
    ofDrawRectangle(position[0], position[1], size[0], size[1]);
}

void projection::draw() {
    //warpPerspective(touch.getDepthImage(), alpha.getPixels(), homography, CV_INTER_LINEAR);
    ofPushMatrix();
    
    ofMatrix4x4 m4x4;
    m4x4 = ofMatrix4x4::ofMatrix4x4(homography.ptr<float>(0));
    ofMultMatrix(m4x4);
    
    ofSetColor(255);
    ofImage img;
    alpha = touch->getDepthImage();
    img.setFromPixels(alpha);
    img.draw(0, 0);
    ofSetColor(0, 200, 0);
    /*
    ofDrawRectangle(0, 0, 350, 600);
    ofDrawRectangle(50, 50, 200, 500);
    ofDrawRectangle(100, 100, 100, 400);
     */
    
    ofPopMatrix();
    //alpha.update();
    
    
    
    drawBorder();
    
    //touch.draw();
}
