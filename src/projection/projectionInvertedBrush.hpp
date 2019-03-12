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
#include "calibrator.hpp"
#include "cameraManager.hpp"

class projectionInvertedBrush : public baseProjection {
private:
    float timer;
    
    ofFbo resultFbo;
    void drawLayers(); // draw all the layers on resultFbo
    
    vector<ofImage> originalImages;
    vector<ofFbo> transformedImages;
    
    vector<layerWithMask> layers;
    void resetLayers(); // clean layers array and add the first image
    void addImageToLayers(int i); // add new layer based on the i-th image
    
    float expansionSpeed; // pixels per second
    float bluredRadius; // blur size
    
    ofFbo transformedMask;
    ofFbo tempFbo;
    bool liveAnimation; // change saturation by depth mask
    
    ofShader shaderExpansion;
    ofShader shaderExpansionAdder;
    ofShader shaderSaturationTuner;
    ofShader shaderMix2Images;
    void loadShaders();
    
    unsigned char * onesBlock; // to check if all pixels are 1 inside layerWithMask
    
    ofImage touchBrush; // to initiate trunsition
    ofFbo touchBrushResized; // resized and preprocessed
    void generateBrush();
    void prepareBrush();
    
    calibrator * calib;
    cameraManager * camera;
public:
    projectionInvertedBrush(cameraManager * camera, touchArea * t, calibrator * calib, bool liveAnimation = true, float expSpeed = 50, float bluredR = 50);
    ~projectionInvertedBrush();
    
    bool start();
    
    void addImage(const string& imgPath);
    
    bool setSize(int width, int height);
    
    void updateAllLayers(); // update all layers, increase touch area on each of them
    void update();
    
    void draw();
};

#endif /* projectionInvertedBrush_hpp */
