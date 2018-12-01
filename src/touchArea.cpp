//
//  touchArea.cpp
//  transparency
//
//  Created by Petr Devaikin on 10.02.18.
//

#include "touchArea.h"

using namespace cv;

touchArea::touchArea(cameraManager * _camera, float _maxDepth, ofVec2f resultCanvasSize) {
    camera = _camera;
    maxDepth = _maxDepth;
    
    started = false;
    
    // allocate result buffer
    resultFbo.allocate(resultCanvasSize[0], resultCanvasSize[1], GL_RGBA);
    resultFbo.begin();
    {
        ofClear(0);
    }
    resultFbo.end();
    
    // init border points with 0
    touchBorderPoints.push_back(ofVec2f(0, 0));
    touchBorderPoints.push_back(ofVec2f(0, 0));
    touchBorderPoints.push_back(ofVec2f(0, 0));
    touchBorderPoints.push_back(ofVec2f(0, 0));
    
    // looking for depth camera
    if (!camera.findCamera()) {
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

touchArea::~touchArea() {
}

void touchArea::start() {
    camera.setZeroLevel();
    started = true;
}

void touchArea::stop() {
    started = false;
}

void touchArea::updateFromCamera() {
    // iterate only withing the bounding area
    for (int i = floor(boundingArea.x); i <= ceil(boundingArea.x + boundingArea.width); i++)
        for (int j = floor(boundingArea.y); j <= ceil(boundingArea.y + boundingArea.height); j++) {
            float res = camera.getDistanceChange(i, j) / maxDepth;
            res = pow(res, 3); // power of 3 to be sensitive more to deep touch
            res *= 255;
            if (res < 0) res = 0;
            if (res > 255) res = 255;
            
            int localI = i - floor(boundingArea.x);
            int localJ = j - floor(boundingArea.y);
            
            int localPixelNumber = 3 * (localJ * int(substractedDepthImage.getWidth()) + localI);
            
            substractedDepthImage.getPixels().getData()[localPixelNumber] = res;
            substractedDepthImage.getPixels().getData()[localPixelNumber + 1] = res;
            substractedDepthImage.getPixels().getData()[localPixelNumber + 2] = res;
        }
    
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

ofFbo & touchArea::getDepth() {
    return resultFbo;
}

vector<ofVec2f> touchArea::getBorderPoints() {
    return touchBorderPoints;
}

void touchArea::setBorderPoints(vector<ofVec2f> points) {
    if (points.size() != 4) {
        cout << "Cannot set depth area. Need 4 points\n";
        return;
    }
    
    for (int i = 0; i < 4; i++)
        touchBorderPoints[i].set(points[i]);
    
    // recalculate bounding rectangle
    float minX = INFINITY, minY = INFINITY, maxX = 0, maxY = 0;
    for (int j = 0; j < 4; j++) {
        if (touchBorderPoints[j][0] < minX) minX = touchBorderPoints[j][0];
        if (touchBorderPoints[j][1] < minY) minY = touchBorderPoints[j][1];
        if (touchBorderPoints[j][0] > maxX) maxX = touchBorderPoints[j][0];
        if (touchBorderPoints[j][1] > maxY) maxY = touchBorderPoints[j][1];
    }
    boundingArea = ofRectangle(minX, minY, maxX - minX, maxY - minY);
    
    // allocate substracted depth image (not transformed)
    substractedDepthImage.allocate(int(boundingArea.width) + 1, int(boundingArea.height) + 1, OF_IMAGE_COLOR);
    calculateTransformation();
}

void touchArea::calculateTransformation() {
    int width = substractedDepthImage.getWidth() - 1; // 1 was added during allocation
    int height = substractedDepthImage.getHeight() - 1;
    
    // update transformation
    vector<Point2f> srcPoints, dstPoints;
    for (int i = 0; i < 4; i++) {
        srcPoints.push_back(Point2f(touchBorderPoints[i][0], touchBorderPoints[i][1]));
    }
    
    dstPoints.push_back(Point2f(0, 0));
    dstPoints.push_back(Point2f(width, 0));
    dstPoints.push_back(Point2f(width, height));
    dstPoints.push_back(Point2f(0, height));
    
    
    Mat m = findHomography(srcPoints, dstPoints);
    transform.set(m.at<double>(0, 0), m.at<double>(1, 0), 0, m.at<double>(2, 0),
                  m.at<double>(0, 1), m.at<double>(1, 1), 0, m.at<double>(2, 1),
                  0, 0, 1, 0,
                  m.at<double>(0, 2), m.at<double>(1, 2), 0, 1);
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
