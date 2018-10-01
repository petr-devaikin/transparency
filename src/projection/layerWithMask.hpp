//
//  layerWithMask.hpp
//  transparency
//
//  Created by Petr Devaikin on 30.09.18.
//

#ifndef layerWithMask_hpp
#define layerWithMask_hpp

#include "ofMain.h"

class layerWithMask {
private:
    unsigned char * ones;
    int _imageIndex;
    ofImage image;
    ofFbo mask;
    ofFbo tempFbo;
    bool _isFull;
    ofShader shaderExp;
    
    ofFbo touchBrush;
public:
    layerWithMask(ofImage img, int imageIndex, unsigned char * oneBlock, ofShader shdr, ofFbo touchBr);
    bool isFull(); // check if the mask covers everything
    void setFull(); // set all mask to 1
    
    bool checkIfTouched(ofFbo * checkArea); // check if intersection of checkArea and mask is more than 50% of checkArea
    void addTouch(ofFbo * touch);
    int getImageIndex(); // index of the image from sourceImage array
    
    void expand(float radius);
    void draw(float x, float y);
};

#endif /* layerWithMask_hpp */
