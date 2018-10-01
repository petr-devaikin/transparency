//
//  layerTouch.hpp
//  transparency
//
//  Created by Petr Devaikin on 02.10.18.
//

#ifndef layerTouch_hpp
#define layerTouch_hpp

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "layerWithMask.hpp"

class layerTouch {
public:
    layerTouch(layerWithMask * l, ofRectangle a);
    
    layerWithMask * layer;
    ofRectangle area;
};

#endif /* layerTouch_hpp */
