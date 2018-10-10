//
//  projectionInvertedBrush.hpp
//  transparency
//
//  Created by Petr Devaikin on 23.07.18.
//

#ifndef projectionInvertedBrush_hpp
#define projectionInvertedBrush_hpp

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "baseProjection.hpp"
#include "layerWithMask.hpp"
#include "layerTouch.hpp"

class projectionInvertedBrush : public baseProjection {
private:
    float timer;
    
    vector<ofImage> originalImages;
    vector<ofImage> scaledImages;
    
    vector<layerWithMask> layers;
    void resetLayers(); // clean layers array and add the first image
    void addImageToLayers(int i); // add new layer based on the i-th image
    
    vector<layerTouch> currentTouches;
    
    int detectLayerIndex(ofPoint point); // finds the next level which was touched
    
    float thresholdSensitive; // to detect touch
    float expansionSpeed; // pixels per second
    float bluredRadius; // blur size
    
    ofxCvContourFinder contourFinder; // to find blobs
    ofxCvGrayscaleImage touchAreaImage; //
    void calculateTouchBlobs(); // calculate blobs from touch area
    
    ofShader shaderExpansion;
    ofShader shaderExpansionAdder;
    ofShader shaderTransparency;
    
    bool onesAndZerosAllocated;
    unsigned char * onesBlock; // to check if all pixels are 1 inside layerWithMask
    
    ofImage touchBrush; // to initiate trunsition
    ofFbo touchBrushResized; // resized and preprocessed
    
public:
    projectionInvertedBrush(touchArea * t, float t1 = 0.5, float expSpeed = 50, float bluredR = 50);
    ~projectionInvertedBrush();
    
    void addImage(const string& imgPath);
    
    bool setSize(int width, int height);
    void update();
    
    void draw();
};

#endif /* projectionInvertedBrush_hpp */
