//
//  projectionInvertedBrush.hpp
//  transparency
//
//  Created by Petr Devaikin on 23.07.18.
//

#ifndef projectionInvertedBrush_hpp
#define projectionInvertedBrush_hpp

#include "ofMain.h"
#include "baseProjection.hpp"

class projectionInvertedBrush : public baseProjection {
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
    int zeroSize = 0;
    unsigned char * zeroBlock; // to check if zero
    
    // gesture stuff
    void applyTouch(ofFbo * touch);
    
    // images
    ofImage image1_original, image2_original;
    ofImage image1, image2;
    
    ofShader transpShader; // to combine two images
public:
    projectionInvertedBrush(float t1 = 0.5, float t2 = 0.3);
    ~projectionInvertedBrush();
    
    void init(const string& image_1_path, const string& image_2_path);
    
    bool setSize(int width, int height);
    void update();
    
    void draw();
};

#endif /* projectionInvertedBrush_hpp */
