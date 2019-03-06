//
//  calibrator.hpp
//  transparency
//
//  Created by Petr Devaikin on 08.12.18.
//

#ifndef calibrator_hpp
#define calibrator_hpp

#include "cameraManager.hpp"
#include "ofMain.h"

enum CalibratorState {
    projectionSetup,
    recognizingQr,
    showingRecognizedArea,
    done
};

class calibrator {
private:
    CalibratorState currentState;
    bool couldNotRecognize;
    
    cameraManager * camera;
    
    int imageWidth;
    int imageHeight;
    
    ofPolyline projectionPolyline;
    ofPolyline cameraPolyline;
    
    void calculateTransformation();
    
    float currentTime;
    float timer;
    float startTimerValue;
public:
    calibrator(cameraManager * _camera, int _imageWidth, int _imageHeight, float qrTimer = 0.1); // qr timer - for how long to show qr before recognizing
    void setProjectionArea(ofPolyline _preset);
    void setCameraArea(ofPolyline _preset);
    
    void update();
    
    void recognize();
    
    void confirmRecognizedArea();
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
