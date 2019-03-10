//
//  touchDetector.hpp
//  transparency
//
//  Find blobs on fbo and returns centroids
//
//  Created by Petr Devaikin on 10.03.19.
//

#ifndef touchDetector_hpp
#define touchDetector_hpp

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "touchArea.h"

class touchDetector {
private:
    float threshold;
    
    ofxCvContourFinder contourFinder; // to find blobs
    ofxCvGrayscaleImage touchAreaImage;
public:
    touchDetector(float threshold);
    
    vector<ofPoint> detect(touchArea * touch);
};

#endif /* touchDetector_hpp */
