//
//  touchArea.hpp
//  transparency
//
//  Created by Petr Devaikin on 10.02.18.
//

#ifndef touchArea_hpp
#define touchArea_hpp

#include "ofMain.h"
#include "ofxCv.h"
#include "cameraManager.hpp"

class touchArea {
private:
    int width;
    int height;
    int x;
    int y;
    
    ofPolyline sensitiveArea;
    float threshold;
    
    ofFbo resultFbo; // depth points with substracted zero level, transformed to the result screen projection
    
    ofImage brush; // touch image for imitation
    
    cameraManager * camera;
    
    ofxCvContourFinder contourFinder;
    
    void updateFromCamera(); // ger rgb and depth data from camera
    
    // running indicator
    bool started;
public:
    touchArea(cameraManager * _camera, ofPolyline _sensitiveArea, float threshold); // threshold to detect touch from 0 to 1
    
    void start();
    void stop();
    bool isRunning();
    
    int getResultWidth();
    int getResultHeight();
    
    ofFbo & getTouchImage();
    vector<ofPoint> detectTouch();
    
    //void setMaxDepth(float maxDepth); // in mm
    
    void update();
    
    // immitation
    void imitateTouch(int x, int y);
    void imitateRelease();
};

#endif /* touchArea_hpp */
