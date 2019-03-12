//
//  touchArea.cpp
//  transparency
//
//  Created by Petr Devaikin on 10.02.18.
//

#include "touchArea.h"

using namespace cv;

touchArea::touchArea(cameraManager * _camera, float threshold) {
    camera = _camera;
    this->threshold = threshold;
    
    // looking for depth camera
    if (!camera->isCameraFound()) {
        // camera not found. prepare fake brush
        //brush.load("brush.png");
        //brush.setImageType(OF_IMAGE_GRAYSCALE);
    }
}

vector<ofPoint> touchArea::detectTouch() {
    vector<ofPoint> result;
    
    if (!camera->isCameraFound()) return result;
    
    thresholdImage = *(camera->getSubstractedImage());
    thresholdImage.threshold(255 * threshold);
    
    contourFinder.findContours(thresholdImage, thresholdImage.getWidth() * thresholdImage.getHeight() / 36, thresholdImage.getWidth() * thresholdImage.getHeight() / 4, 4, false);
    
    for (int i = 0; i < contourFinder.nBlobs; i++) {
        ofxCvBlob blob = contourFinder.blobs[i];
        result.push_back(blob.centroid);
    }
    
    return result;
}

void touchArea::setThreshold(float threshold) {
    this->threshold = threshold;
}

float touchArea::getThreshold() {
    return threshold;
}

// immitation

void touchArea::imitateTouch(int x, int y) {
    /*
    resultFbo.begin();
    {
        ofClear(0, 0, 0, 255);
        ofSetColor(255);
        //ofSetColor(100);
        brush.draw(x - brush.getWidth(), y - brush.getHeight(), brush.getWidth() * 2, brush.getHeight() * 2);
        
        //brush.draw(x - brush.getWidth(), 600 - y + brush.getHeight(), brush.getWidth() * 2, brush.getHeight() * 2);
    }
    resultFbo.end();
     */
}

void touchArea::imitateRelease() {
    /*
    resultFbo.begin();
    {
        ofClear(0, 0, 0, 255);
    }
    resultFbo.end();
     */
}
