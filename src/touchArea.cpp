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
    
    // allocate result buffer
    resultFbo.allocate(sensitiveArea.getBoundingBox().width, sensitiveArea.getBoundingBox().height, GL_RGBA);
    resultFbo.begin();
    ofClear(0);
    resultFbo.end();
    
    // allocate image for substraction
    substractedDepthImage.allocate(sensitiveArea.getBoundingBox().width, sensitiveArea.getBoundingBox().height, OF_IMAGE_COLOR);
    
    // looking for depth camera
    if (!camera->isCameraFound()) {
        // camera not found. prepare fake brush
        brush.load("brush.png");
        brush.setImageType(OF_IMAGE_GRAYSCALE);
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
    for (int i = 0; i < sensitiveArea.getBoundingBox().width; i++)
        for (int j = 0; j < sensitiveArea.getBoundingBox().height; j++) {
            float res = 0;
            
            // check if pixel is inside sensitive area
            // get difference from camera
            if (sensitiveArea.inside(i + sensitiveArea.getBoundingBox().x, j + sensitiveArea.getBoundingBox().y)) {
                float res = camera->getDistanceChange(i + sensitiveArea.getBoundingBox().x, j + sensitiveArea.getBoundingBox().y) / maxDepth;
                res = pow(res, 3); // power of 3 to be sensitive more to deep touch
                res *= 255;
                if (res < 0) res = 0;
                if (res > 255) res = 255;
            }
                
            int localPixelNumber = 3 * (j * sensitiveArea.getBoundingBox().width + i);
            
            substractedDepthImage.getPixels().getData()[localPixelNumber] = res;
            substractedDepthImage.getPixels().getData()[localPixelNumber + 1] = res;
            substractedDepthImage.getPixels().getData()[localPixelNumber + 2] = res;
        }
    
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
