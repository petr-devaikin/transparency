//
//  projectionFog.hpp
//  transparency
//
//  Created by Petr Devaikin on 10.10.18.
//

#ifndef projectionFog_hpp
#define projectionFog_hpp

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "baseProjection.hpp"

class projectionFog : public baseProjection {
private:
    float timer;
    
    float maxFogDepth;
    float fadePeriod; // how fast fog goes back in sec
    float depthBlur;
    
    ofFbo work; // work fbo
    ofFbo mask; // current touch mask
    ofFbo result; // current resulting image
    
    void updateMask(); // add current touch to mask
    
    ofShader maxAndFadeShader;
    ofShader depthCutShader;
public:
    projectionFog(touchArea * t, float _maxFogDepth = 255, float _depthBlur = 10, float _fadePeriod = 5);
    //~projectionInvertedBrush();
    
    bool setSize(int width, int height);
    void update(ofImage rgbFrame, ofImage depthFrame, ofImage depthZeroLevel);
    
    void draw();
};

#endif /* projectionFog_hpp */
