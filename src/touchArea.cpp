//
//  touchArea.cpp
//  transparency
//
//  Created by Petr Devaikin on 10.02.18.
//

#include "touchArea.h"

using namespace cv;

touchArea::touchArea(cameraManager * _camera, float _maxDepth, ofPolyline _sensitiveArea) {
    camera = _camera;
    maxDepth = _maxDepth;
    sensitiveArea = _sensitiveArea;
    
    started = false;
    
    width = sensitiveArea.getBoundingBox().width;
    height = sensitiveArea.getBoundingBox().height;
    x = sensitiveArea.getBoundingBox().x;
    y = sensitiveArea.getBoundingBox().y;
    
    // allocate result buffer
    resultFbo.allocate(sensitiveArea.getBoundingBox().width, sensitiveArea.getBoundingBox().height, GL_RGBA);
    resultFbo.begin();
    ofClear(0);
    resultFbo.end();
    
    // allocate buffer for substraction
    substractedPixels = new unsigned char[width * height];
    substractedDepthImage.allocate(width, height, OF_IMAGE_GRAYSCALE);
    
    // allocate image for substraction
    substractedDepthImage.allocate(width, height, OF_IMAGE_COLOR);
    
    // looking for depth camera
    if (!camera->isCameraFound()) {
        // camera not found. prepare fake brush
        brush.load("brush.png");
        brush.setImageType(OF_IMAGE_GRAYSCALE);
    }
}

touchArea::~touchArea() {
    delete [] substractedPixels;
}

void touchArea::start() {
    //camera->setZeroLevel();
    started = true;
}

void touchArea::stop() {
    started = false;
}

int touchArea::getResultWidth() {
    return width;
}

int touchArea::getResultHeight() {
    return height;
}

void touchArea::updateFromCamera() {
    // calculate distance between current position and zero level.
    // transform this value to decrease sensitivity at the beginning of touch
    // iterate only withing the bounding area
    for (int i = 0; i < width; i++)
        for (int j = 0; j < height; j++) {
            float res = 0;
            
            // check if pixel is inside sensitive area
            // get difference from camera
            if (sensitiveArea.inside(i + x, j + y)) {
                float res = camera->getDistanceChange(i + x, j + y) / maxDepth;
                res = pow(res, 3); // power of 3 to be sensitive more to deep touch
                res *= 255;
                if (res < 0) res = 0;
                if (res > 255) res = 255;
            }
            
            substractedPixels[j * width + i] = res;
        }
    
    // create image based on substractedPixels
    memcpy(substractedPixels, substractedDepthImage.getPixels().getData(), width * height);
    substractedDepthImage.update();
    
    // transform resulting picture from camera space to image space
    resultFbo.begin();
    {
        ofSetColor(255);
        substractedDepthImage.draw(0, 0);
    }
    resultFbo.end();
}

ofFbo & touchArea::getTouch() {
    return resultFbo;
}

unsigned char * touchArea::getTouchPixels() {
    return substractedPixels;
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
