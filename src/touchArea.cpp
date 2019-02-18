//
//  touchArea.cpp
//  transparency
//
//  Created by Petr Devaikin on 10.02.18.
//

#include "touchArea.h"

using namespace cv;

touchArea::touchArea(cameraManager * _camera, float _maxDepth, ofVec2f resultCanvasSize)
{
    camera = _camera;
    maxDepth = _maxDepth;
    
    started = false;
    
    // allocate image for substraction
    substractedDepthImage.allocate(camera->getWidth(), camera->getHeight(), OF_IMAGE_COLOR);
    
    // allocate result buffer
    resultFbo.allocate(resultCanvasSize[0], resultCanvasSize[1], GL_RGBA);
    resultFbo.begin();
    {
        ofClear(0);
    }
    resultFbo.end();
    
    // looking for depth camera
    if (!camera->isCameraFound()) {
        // camera not found. prepare fake brush
        brush.load("brush.png");
        brush.setImageType(OF_IMAGE_GRAYSCALE);
    }
    else {
        // init transform matrix
        transform = ofMatrix4x4(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);
        // [? ? 0 ?]
        // [? ? 0 ?]
        // [0 0 1 0]
        // [? ? 0 1]
    }
}

void touchArea::start() {
    //camera->setZeroLevel();
    started = true;
}

void touchArea::stop() {
    started = false;
}

int touchArea::getResultWidth() {
    return resultFbo.getWidth();
}

int touchArea::getResultHeight() {
    return resultFbo.getHeight();
}

void touchArea::updateFromCamera() {
    // calculate distance between current position and zero level.
    // transform this value to decrease sensitivity at the beginning of touch
    // iterate only withing the bounding area
    for (int i = floor(boundingArea.x); i <= ceil(boundingArea.x + boundingArea.width); i++)
        for (int j = floor(boundingArea.y); j <= ceil(boundingArea.y + boundingArea.height); j++) {
            float res = camera->getDistanceChange(i, j) / maxDepth;
            res = pow(res, 3); // power of 3 to be sensitive more to deep touch
            res *= 255;
            if (res < 0) res = 0;
            if (res > 255) res = 255;
            
            int localPixelNumber = 3 * (j * int(substractedDepthImage.getWidth()) + i);
            
            substractedDepthImage.getPixels().getData()[localPixelNumber] = res;
            substractedDepthImage.getPixels().getData()[localPixelNumber + 1] = res;
            substractedDepthImage.getPixels().getData()[localPixelNumber + 2] = res;
        }
    
    substractedDepthImage.update();
    
    // transform resulting picture from camera space to image space
    resultFbo.begin();
    {
        ofClear(0, 0, 0, 255);
        
        ofPushMatrix();
        
        ofMultMatrix(transform);
        
        ofSetColor(255);
        substractedDepthImage.draw(0, 0);
        
        ofPopMatrix();
    }
    resultFbo.end();
}

ofFbo & touchArea::getTransformedTouch() {
    return resultFbo;
}

void touchArea::setSensitiveArea(ofPolyline points) {
    if (points.size() != 4) {
        cout << "Cannot set depth area. Need 4 points\n";
        return;
    }
    
    // recalculate bounding rectangle
    boundingArea = points.getBoundingBox();
    
    calculateTransformation(points);
}

void touchArea::calculateTransformation(ofPolyline sensitiveArea) {
    // update transformation
    vector<Point2f> srcPoints, dstPoints;
    for (int i = 0; i < 4; i++) {
        srcPoints.push_back(Point2f(sensitiveArea[i][0], sensitiveArea[i][1]));
    }
    
    dstPoints.push_back(Point2f(0, 0));
    dstPoints.push_back(Point2f(resultFbo.getWidth(), 0));
    dstPoints.push_back(Point2f(resultFbo.getWidth(), resultFbo.getHeight()));
    dstPoints.push_back(Point2f(0, resultFbo.getHeight()));
    
    Mat m = findHomography(srcPoints, dstPoints);
    transform.set(m.at<double>(0, 0), m.at<double>(1, 0), 0, m.at<double>(2, 0),
                  m.at<double>(0, 1), m.at<double>(1, 1), 0, m.at<double>(2, 1),
                  0, 0, 1, 0,
                  m.at<double>(0, 2), m.at<double>(1, 2), 0, 1);
}

void touchArea::update() {
    if (camera->isCameraFound())
        updateFromCamera();
}

// immitation

void touchArea::imitateTouch(int x, int y) {
    resultFbo.begin();
    {
        ofClear(0, 0, 0, 255);
        ofSetColor(255);
        //ofSetColor(100);
        brush.draw(x - brush.getWidth(), y - brush.getHeight(), brush.getWidth() * 2, brush.getHeight() * 2);
        
        //brush.draw(x - brush.getWidth(), 600 - y + brush.getHeight(), brush.getWidth() * 2, brush.getHeight() * 2);
    }
    resultFbo.end();
}

void touchArea::imitateRelease() {
    resultFbo.begin();
    {
        ofClear(0, 0, 0, 255);
    }
    resultFbo.end();
}
