//
//  projectionInvertedBrush.hpp
//  transparency
//
//  Created by Petr Devaikin on 23.07.18.
//

#ifndef projectionInvertedBrush_hpp
#define projectionInvertedBrush_hpp

#include "ofMain.h"
#include "projection.hpp"

class projectionInvertedBrush : public projection {
private:
    bool touchStarted;
    bool resultColor; // 0 or 1
    
    bool detectColor(ofFbo * fbo);
    
    float thresholdSensetive; // to detect touch
    float thresholdBrush; // for real touch area (should be lower)
    
    ofFbo resultFbo;
    ofFbo workFbo;
    void applyThresholdToWorkFbo(float threshold);
    
    ofShader shaderThreshold;
    
    // check if burrer empty
    bool checkIfEmpty(ofFbo * fbo); // check if all pixels are 0;
    bool zeroAllocated;
    unsigned char * zeroBlock; // to check if zero
    
    // gesture stuff
    ofFbo gestureFbo; // to store current gesture
    void cleanGestureBuffer();
    void addTouchToGesture(ofFbo * touch);
    void applyGesture();
public:
    projectionInvertedBrush(float t1 = 0.5, float t2 = 0.4);
    ~projectionInvertedBrush();
    
    bool setSize(int width, int height);
    void update();
    
    void draw();
};

#endif /* projectionInvertedBrush_hpp */
