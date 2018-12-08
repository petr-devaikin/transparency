//
//  calibrator.cpp
//  transparency
//
//  Created by Petr Devaikin on 08.12.18.
//

#include "calibrator.hpp"
#include "ofxOpenCv.h"

calibrator::calibrator(cameraManager * _camera, float qrTimer) {
    currentState = projectionSetup;
    couldNotRecognize = false;
    
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
}

void calibrator::presetProjectionArea(ofPolyline _preset) {
    if (_preset.size() == 4)
        projectionPolyline = _preset;
}

void calibrator::update() {
    float newTime = ofGetElapsedTimef();
    
    if (currentState == recognizingQr) {
        timer -= newTime - currentTime;
        
        if (timer < 0) {
            bool recognized = false;
            // TBD start recognition
            
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

void calibrator::confirmedRecognizedArea() {
    cout << "Confirm recognition\n";
    currentState = done;
}

void calibrator::startAgain() {
    cout << "Start calibration again\n";
    currentState = projectionSetup;
    couldNotRecognize = false;
}

// ----

CalibratorState calibrator::getState() {
    return currentState;
}


void calibrator::draw() {
    if (currentState == projectionSetup) {
        ofSetColor(255, 0, 0);
        projectionPolyline.draw();
        
        ofSetColor(255);
        ofDrawBitmapString("Move corners to cover all projected area and press Space", 10, 50);
        if (couldNotRecognize)
            ofDrawBitmapString("Couldn't recognize marker", 10, 70);
    }
    else if (currentState == recognizingQr) {
        // TBD drawing qr
    }
    else if (currentState == showingRecognizedArea) {
        // draw camera image
        ofSetColor(255);
        ofImage rgb = camera->getRGBImage();
        rgb.draw(0, 0);
        
        // draw recognized polyline on top
        ofNoFill();
        ofSetColor(255, 0, 0);
        cameraPolyline.draw();
        ofFill();
        
        ofDrawBitmapString("Press Space to confirm recognition. Press Q to start again.", 10, 50);
    }
    // do nothing if done
}

ofPolyline calibrator::getProjectionPolyline() {
    return projectionPolyline;
}

ofPolyline calibrator::getCameraPolyline() {
    return cameraPolyline;
}
