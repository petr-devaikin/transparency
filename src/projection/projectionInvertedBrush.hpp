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
#include "layerWithMask.hpp"

class projectionInvertedBrush : public baseProjection {
private:
    float timer;
    
    vector<ofImage> originalImages;
    vector<ofImage> scaledImages;
    
    vector<layerWithMask> layers;
    void resetLayers(); // clean layers array and add the first image
    void addImageToLayers(int i); // add new layer based on the i-th image
    
    bool touchStarted;
    int currentlyTouchedLayerIndex;
    
    int detectLayerIndex(ofFbo * checkArea); // finds the next level which was touched
    
    float thresholdSensetive; // to detect touch
    float thresholdBrush; // for real touch area (should be lower)
    float expansionSpeed; // pixels per second
    
    ofFbo workFbo; // used inside applyThreshold
    ofFbo * applyThresholdToTouchFbo(float threshold); // result workFbo!
    
    ofShader shaderThreshold;
    ofShader shaderExpansion;
    
    bool checkIfEmpty(ofFbo * fbo); // check if all pixels are 0;
    bool onesAndZerosAllocated;
    unsigned char * zerosBlock;
    unsigned char * onesBlock; // to check if all pixels are 1 inside layerWithMask
    
    ofShader transpShader; // to combine two images
public:
    projectionInvertedBrush(touchArea * t, float t1 = 0.5, float t2 = 0.3, float expSpeed = 100);
    ~projectionInvertedBrush();
    
    void addImage(const string& imgPath);
    
    bool setSize(int width, int height);
    void update();
    
    void draw();
};

#endif /* projectionInvertedBrush_hpp */
