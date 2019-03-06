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
    ofShader shaderMix2Images;
    
    unsigned char * onesBlock; // to check if all pixels are 1 inside layerWithMask
    
    ofImage touchBrush; // to initiate trunsition
    ofFbo touchBrushResized; // resized and preprocessed
    
    ofMatrix4x4 camera2Projection;
    ofMatrix4x4 image2Projection;
    ofRectangle projectionBox;
public:
    projectionInvertedBrush(const string basePath, touchArea * t, ofMatrix4x4 transformCamera2Projection, ofMatrix4x4 transformImage2Projection, ofRectangle _projectionBox, float _thresholdSensitive = 0.5, float expSpeed = 50, float bluredR = 50);
    ~projectionInvertedBrush();
    
    void setTransform(ofMatrix4x4 transformCamera2Projection, ofMatrix4x4 transformImage2Projection, ofRectangle _projectionBox);
    
    bool start();
    
    void addImage(const string& imgPath);
    
    bool setSize(int width, int height);
    
    void updateAllLayers(); // update all layers, increase touch area on each of them
    void update();
    
    void draw();
};

#endif /* projectionInvertedBrush_hpp */
