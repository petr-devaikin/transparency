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
    ofPolyline sensitiveArea;
    float maxDepth; // max depth in mm (or meters?) to scale the depth image
    
    ofImage substractedDepthImage;
    ofFbo resultFbo; // depth points with substracted zero level, transformed to the result screen projection
    
    ofImage brush; // touch image for imitation
    
    cameraManager * camera;
    
    void updateFromCamera(); // ger rgb and depth data from camera
    
    // running indicator
    bool started;
public:
    touchArea(cameraManager * _camera, float _maxDepth, ofPolyline _sensitiveArea); // maxDepth in meters
    
    void start();
    void stop();
    bool isRunning();
    
    int getResultWidth();
    int getResultHeight();
    
    ofFbo & getTouch();
    
    //void setMaxDepth(float maxDepth); // in mm
    
    void update();
    
    // immitation
    void imitateTouch(int x, int y);
    void imitateRelease();
};

#endif /* touchArea_hpp */
