//
//  touchArea.cpp
//  transparency
//
//  Created by Petr Devaikin on 10.02.18.
//

#include "touchArea.h"

using namespace cv;

touchArea::touchArea(cameraManager * _camera, ofPolyline _sensitiveArea, float threshold) {
    camera = _camera;
    sensitiveArea = _sensitiveArea;
    this->threshold = threshold;
    
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
    return width;
}

int touchArea::getResultHeight() {
    return height;
}

void touchArea::updateFromCamera() {
    // drawing substracted pixels on resultFbo
    
    resultFbo.begin();
    {
        ofSetColor(255);
        (camera->getSubstractedImage()).draw(0, 0);
    }
    resultFbo.end();
}

vector<ofPoint> touchArea::detectTouch() {
    vector<ofPoint> result;
    
    ofxCvGrayscaleImage img = camera->getSubstractedImage();
    img.threshold(255 * threshold);
    
    contourFinder.findContours(img, 4, img.getWidth() * img.getHeight(), 4, false);
    
    for (int i = 0; i < contourFinder.nBlobs; i++) {
        ofxCvBlob blob = contourFinder.blobs[i];
        result.push_back(blob.centroid);
    }
    
    return result;
}

ofFbo & touchArea::getTouchImage() {
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
