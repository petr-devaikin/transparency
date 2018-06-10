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
}

void projection::init(const string& img_1_path, const string& img_2_path) {
    lastMaskUpdate = ofGetElapsedTimef();
    // load shader
    shader.load("shadersGL3/shader");
    
    //alpha.allocate(1000, 1000, OF_IMAGE_COLOR);
    homography = Mat(4, 4, CV_32F);
    homography.at<float>(2, 0) = homography.at<float>(2, 1) = homography.at<float>(2, 3) = 0;
    homography.at<float>(0, 2) = homography.at<float>(1, 2) = homography.at<float>(3, 2) = 0;
    homography.at<float>(2, 2) = homography.at<float>(3, 3) = 1;
    
    // load images
    image1_original.load(img_1_path);
    image2_original.load(img_2_path);
    
    image1.clone(image1_original);
    image2.clone(image2_original);
}

void projection::setPosition(int x, int y) {
    position.set(x, y);
}

void projection::setSize(int width, int height) {
    if (width != size[0] || height != size[1])
    {
        size.set(width, height);
        
        // upload image again!
        image1.clone(image1_original);
        image2.clone(image2_original);
        image1.resize(width, height);
        image2.resize(width, height);
        
        // reallocate buffer
        resultFbo.allocate(width, height, GL_RGBA);
        maskFbo.allocate(width, height, GL_RGBA);
        maskFbo.begin();
        ofClear(0, 0, 0, 255);
        maskFbo.end();
    }
}

void projection::setX(int x) {
    setPosition(x, position[1]);
}

void projection::setY(int y) {
    setPosition(position[0], y);
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
    ofSetColor(255, 255, 255);
    ofNoFill();
    ofDrawRectangle(position[0] - 5, position[1] - 5, size[0] + 10, size[1] + 10);
}

void projection::update() {
    float currentTime = ofGetElapsedTimef();
    float timeDiff = currentTime - lastMaskUpdate;
    int fadeLevel = 255 * timeDiff / FADE_PERIOD;
    if (fadeLevel >= 1)
        lastMaskUpdate = currentTime;
    
    // modify mask
    maskFbo.begin();
    {
        if (fadeLevel >= 1)
        {
            ofSetColor(fadeLevel, fadeLevel, fadeLevel, 255);
            ofFill();
            ofEnableBlendMode(OF_BLENDMODE_SUBTRACT);
            ofDrawRectangle(0, 0, size[0], size[1]);
        }
        
        ofPushMatrix();
        
        ofMatrix4x4 m4x4;
        m4x4 = ofMatrix4x4(homography.ptr<float>(0));
        ofMultMatrix(m4x4);
        
        ofEnableBlendMode(OF_BLENDMODE_SCREEN); // need MAX! not add
        ofSetColor(255);
        (touch->getDepth()).draw(0, 0);
        ofEnableBlendMode(OF_BLENDMODE_ALPHA);
        
        ofPopMatrix();
    }
    maskFbo.end();

    // final drawing
    resultFbo.begin();
    {
        ofClear(0, 0, 0, 0);
        ofSetColor(255);
        
        //maskFbo.draw(0, 0); // to test how mask is transformed
        
        shader.begin();
        {
            shader.setUniform1f("multiplier", 255.0 / maxDepthLevel);
            shader.setUniformTexture("bgTex", image1.getTexture(), 1);
            shader.setUniformTexture("maskTex", maskFbo.getTexture(), 2);
            image2.draw(0, 0);
        }
        shader.end();
         
    }
    resultFbo.end();
}

void projection::draw() {
    ofSetColor(255);
    resultFbo.draw(position[0], position[1]);
}

void projection::setDepthLevel(int d) {
    maxDepthLevel = d;
}

int projection::getDepthLevel() {
    return maxDepthLevel;
}
