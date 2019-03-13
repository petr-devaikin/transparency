//
//  calibrator.cpp
//  transparency
//
//  Created by Petr Devaikin on 08.12.18.
//

#include "calibrator.hpp"
#include "ofxOpenCv.h"

using namespace cv;

calibrator::calibrator(cameraManager * _camera, int _imageWidth, int _imageHeight, float qrTimer) {
    currentState = exposureSetup;
    couldNotRecognize = false;
    
    imageWidth = _imageWidth;
    imageHeight = _imageHeight;
    currentTime = 0;
    timer = 0;
    startTimerValue = qrTimer;
    
    camera = _camera;
    
    // init projection polyline before it's preset
    projectionPolyline.addVertex(100, 100);
    projectionPolyline.addVertex(400, 100);
    projectionPolyline.addVertex(400, 400);
    projectionPolyline.addVertex(100, 400);
    
    // generate ArUco markers
    //cv::aruco
    
    // setup GUI
    gui.setup();
    gui.add(exposureSlider.setup("Exposure", camera->getExposure(), 20, 150000));
    exposureSlider.addListener(this, &calibrator::exposureChanged);
}

void calibrator::setProjectionArea(ofPolyline _preset) {
    if (_preset.size() == 4)
        projectionPolyline = _preset;
}

void calibrator::setCameraArea(ofPolyline _preset) {
    if (_preset.size() == 4)
        cameraPolyline = _preset;
}

void calibrator::update() {
    float newTime = ofGetElapsedTimef();
    
    if (currentState == recognizingQr) {
        timer -= newTime - currentTime;
        
        if (timer < 0) {
            bool recognized = false;
            // TBD start recognition
            
            // for now set as recognized
            recognized = true;
            
            if (recognized) {
                // cameraPolyline = ...
                currentState = showingRecognizedArea;
                couldNotRecognize = false;
            }
            else {
                currentState = projectionSetup;
                couldNotRecognize = true;
            }
        }
    }
    
    currentTime = newTime;
}

// user actions

void calibrator::recognize() {
    cout << "Try to recognize\n";
    currentState = recognizingQr;
    // set a timer to show qr for some time
    timer = startTimerValue;
}

void calibrator::confirmExposure() {
    cout << "Confirm exposure\n";
    currentState = projectionSetup;
}

void calibrator::confirmRecognizedArea() {
    cout << "Confirm recognition\n";
    camera->setRoi(cameraPolyline.getBoundingBox());
    camera->setZeroLevel();
    currentState = done;
}

void calibrator::startAgain() {
    cout << "Start calibration again\n";
    currentState = exposureSetup;
    couldNotRecognize = false;
}

// ----

CalibratorState calibrator::getState() {
    return currentState;
}


void calibrator::draw() {
    if (currentState == exposureSetup) {
        ofSetColor(255);
        (camera->getDepthVisualizationImage()).draw(0, 0);
        ofDrawBitmapString("Camera: " + camera->getCameraName(), 10, 80);
        ofDrawBitmapString("Set exposure and press Space", 10, 110);
        gui.draw();
    }
    else if (currentState == projectionSetup) {
        ofSetColor(255, 0, 0);
        
        ofBeginShape();
        for( int i = 0; i < projectionPolyline.getVertices().size(); i++) {
            ofVertex(projectionPolyline.getVertices().at(i).x, projectionPolyline.getVertices().at(i).y);
        }
        ofEndShape();
        
        ofSetColor(255);
        ofDrawBitmapString("Move corners to completely cover the flag with projected polygon and press Space", 10, 50);
        if (couldNotRecognize)
            ofDrawBitmapString("Couldn't recognize marker!", 10, 80);
    }
    else if (currentState == recognizingQr) {
        // TBD drawing qr
    }
    else if (currentState == showingRecognizedArea) {
        // draw camera image
        ofSetColor(255);
        (camera->getRGBImage()).draw(0, 0);
        
        // draw recognized polyline on top
        ofNoFill();
        ofSetColor(0, 255, 0);
        cameraPolyline.draw();
        ofFill();
        
        ofSetColor(255);
        ofDrawBitmapString("Move corners to cover the flag on the camera view and press Space", 10, 50);
        ofDrawBitmapString("Press R to start again.", 10, 70);
    }
    // do nothing if done
}

ofPolyline calibrator::getProjectionPolyline() {
    return projectionPolyline;
}

ofPolyline calibrator::getCameraPolyline() {
    return cameraPolyline;
}

ofRectangle calibrator::getProjectionBox() {
    return projectionPolyline.getBoundingBox();
}

ofRectangle calibrator::getCameraBox() {
    return cameraPolyline.getBoundingBox();
}

ofMatrix4x4 calibrator::getCamera2ProjectionTransform() {
    // update transformation
    vector<Point2f> srcPoints, dstPoints;
    for (int i = 0; i < 4; i++) {
        srcPoints.push_back(Point2f(
                                    cameraPolyline[i][0] - cameraPolyline.getBoundingBox().x,
                                    cameraPolyline[i][1] - cameraPolyline.getBoundingBox().y));
        dstPoints.push_back(Point2f(
                                    projectionPolyline[i][0] - projectionPolyline.getBoundingBox().x,
                                    projectionPolyline[i][1] - projectionPolyline.getBoundingBox().y));
    }
    
    Mat m = findHomography(srcPoints, dstPoints);
    
    ofMatrix4x4 result;
    result.set(m.at<double>(0, 0), m.at<double>(1, 0), 0, m.at<double>(2, 0),
               m.at<double>(0, 1), m.at<double>(1, 1), 0, m.at<double>(2, 1),
               0, 0, 1, 0,
               m.at<double>(0, 2), m.at<double>(1, 2), 0, 1);
    return result;
}

ofMatrix4x4 calibrator::getImage2ProjectionTransform() {
    // update transformation
    vector<Point2f> srcPoints, dstPoints;
    
    srcPoints.push_back(Point2f(0, 0));
    srcPoints.push_back(Point2f(imageWidth, 0));
    srcPoints.push_back(Point2f(imageWidth, imageHeight));
    srcPoints.push_back(Point2f(0, imageHeight));
    
    for (int i = 0; i < 4; i++) {
        dstPoints.push_back(Point2f(
                                    projectionPolyline[i][0] - projectionPolyline.getBoundingBox().x,
                                    projectionPolyline[i][1] - projectionPolyline.getBoundingBox().y));
    }
    
    Mat m = findHomography(srcPoints, dstPoints);
    
    ofMatrix4x4 result;
    result.set(m.at<double>(0, 0), m.at<double>(1, 0), 0, m.at<double>(2, 0),
               m.at<double>(0, 1), m.at<double>(1, 1), 0, m.at<double>(2, 1),
               0, 0, 1, 0,
               m.at<double>(0, 2), m.at<double>(1, 2), 0, 1);
    return result;
}

void calibrator::setMaxDepth(float maxDepth) {
    this->maxDepth = maxDepth;
}

void calibrator::setThreshold(float threshold) {
    this->threshold = threshold;
}

void calibrator::setExposure(float e) {
    exposureSlider = e;
}

float calibrator::getMaxDepth() {
    return maxDepth;
}

float calibrator::getThreshold() {
    return threshold;
}

void calibrator::exposureChanged(float &newExposure){
    camera->setExposure(newExposure);
}
