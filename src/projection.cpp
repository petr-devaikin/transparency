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
    // load shader
    shader.load("shadersGL3/shader");
    
    //alpha.allocate(1000, 1000, OF_IMAGE_COLOR);
    homography = Mat(4, 4, CV_32F);
    homography.at<float>(2, 0) = homography.at<float>(2, 1) = homography.at<float>(2, 3) = 0;
    homography.at<float>(0, 2) = homography.at<float>(1, 2) = homography.at<float>(3, 2) = 0;
    homography.at<float>(2, 2) = homography.at<float>(3, 3) = 1;
    
    // load images
    image1.load("img0.jpg");
    image2.load("img1.jpg");
}

void projection::setPosition(int x, int y) {
    position.set(x, y);
}

void projection::setSize(int width, int height) {
    size.set(width, height);
    
    // upload image again!
    image1.resize(width, height);
    image2.resize(width, height);
    
    // reallocate buffer
    resultFbo.allocate(width, height, GL_RGBA);
    maskFbo.allocate(width, height, GL_RGBA);
}

void projection::setX(int x) {
    position[0] = x;
}

void projection::setY(int y) {
    position[1] = y;
}

void projection::setWidth(int width) {
    setSize(width, size[1]);
}

void projection::setHeight(int height) {
    setSize(size[0], height);
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
    
    dstPoints.push_back(Point2f(0, 0));
    dstPoints.push_back(Point2f(size[0], 0));
    dstPoints.push_back(Point2f(size[0], size[1]));
    dstPoints.push_back(Point2f(0, size[1]));

    Mat m3x3 = findHomography(srcPoints, dstPoints);
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++) {
            int ii = i < 2 ? i : i + 1;
            int jj = j < 2 ? j : j + 1;
            homography.at<float>(ii, jj) = m3x3.at<double>(j, i);
        }
}

void projection::drawBorder() {
    ofSetColor(100, 100, 100);
    ofNoFill();
    ofDrawRectangle(position[0], position[1], size[0], size[1]);
}

void projection::update() {
    // modify mask
    maskFbo.begin();
    {
        ofClear(0, 0, 0, 0);
        
        ofPushMatrix();
        
        ofMatrix4x4 m4x4;
        m4x4 = ofMatrix4x4::ofMatrix4x4(homography.ptr<float>(0));
        ofMultMatrix(m4x4);
        
        ofSetColor(255);
        (touch->getDepth()).draw(0, 0);
        
        ofPopMatrix();
    }
    maskFbo.end();
    
    
    // final drawing
    resultFbo.begin();
    {
        ofSetColor(255);
        image1.draw(0, 0);
        
        shader.setUniformTexture("maskTex", maskFbo.getTexture(), 1);
        shader.begin();
        {
            image2.draw(0, 0);
        }
        shader.end();
    }
    resultFbo.end();
}

void projection::draw() {
    ofSetColor(255);
    resultFbo.draw(position[0], position[1]);
    
    drawBorder();
}
