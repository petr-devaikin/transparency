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
    float threshold;
    
    //ofImage brush; // touch image for imitation
    
    cameraManager * camera;
    
    ofxCvGrayscaleImage thresholdImage;
    ofxCvContourFinder contourFinder;
public:
    touchArea(cameraManager * _camera, float threshold = 0.5); // threshold to detect touch from 0 to 1
    
    vector<ofPoint> detectTouch();
    
    void setThreshold(float threshold);
    float getThreshold();
    
    // immitation
    void imitateTouch(int x, int y);
    void imitateRelease();
};

#endif /* touchArea_hpp */
