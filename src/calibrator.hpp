//
//  calibrator.hpp
//  transparency
//
//  Created by Petr Devaikin on 08.12.18.
//

#ifndef calibrator_hpp
#define calibrator_hpp

#include "cameraManager.hpp"
#include "touchArea.h"
#include "ofMain.h"
#include "ofxGui.h"

enum CalibratorState {
    exposureSetup,
    projectionSetup,
    recognizingQr,
    showingRecognizedArea,
    thresholdSetup,
    done
};

class calibrator {
private:
    CalibratorState currentState;
    bool couldNotRecognize;
    
    cameraManager * camera;
    touchArea * touch;
    
    int imageWidth;
    int imageHeight;
    
    ofPolyline projectionPolyline;
    ofPolyline cameraPolyline;
    
    float maxDepth = .5;
    
    void calculateTransformation();
    
    float currentTime;
    float timer;
    float startTimerValue;
    
    ofxPanel guiExposure;
    ofxFloatSlider exposureSlider;
    void exposureChanged(float &newExposure);
    
    ofxPanel guiThreshold;
    ofxFloatSlider thresholdSlider;
    void thresholdChanged(float &newThreshold);
public:
    calibrator(cameraManager * camera, touchArea * touch, int _imageWidth, int _imageHeight, float qrTimer = 0.1); // qr timer - for how long to show qr before recognizing
    
    void setProjectionArea(ofPolyline _preset);
    void setCameraArea(ofPolyline _preset);
    
    void setMaxDepth(float maxDepth);
    void setThreshold(float threshold);
    void setExposure(float e);
    
    void update();
    
    void recognize();
    
    void confirmExposure();
    void confirmRecognizedArea();
    void confirmThreshold();
    void startAgain();
    
    CalibratorState getState();
    
    void draw();
    
    ofPolyline getProjectionPolyline();
    ofPolyline getCameraPolyline();
    ofRectangle getProjectionBox();
    ofRectangle getCameraBox();
    
    ofMatrix4x4 getCamera2ProjectionTransform();
    ofMatrix4x4 getImage2ProjectionTransform();
};

#endif /* calibrator_hpp */
