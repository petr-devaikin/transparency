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
    ofShader shaderExpAdder;
    
    ofFbo touchBrush;
public:
    layerWithMask(ofImage img, int imageIndex, unsigned char * oneBlock, ofShader shdrE, ofShader shdrA, ofFbo touchBr);
    bool isFull(); // check if the mask covers everything
    void setFull(); // set all mask to 1
    
    bool checkIfTouched(ofPoint point); // check if intersection of checkArea and mask is more than 50% of checkArea
    void addTouch(ofPoint point, float blurRadius);
    int getImageIndex(); // index of the image from sourceImage array
    
    void expand(float radius);
    
    ofImage getImage();
    ofFbo getMask();
};

#endif /* layerWithMask_hpp */
