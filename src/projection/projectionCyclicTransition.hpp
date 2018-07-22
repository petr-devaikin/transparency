//
//  projectionCyclicTransition.hpp
//  transparency
//
//  Created by Petr Devaikin on 22.07.18.
//

#ifndef projectionCyclicTransition_hpp
#define projectionCyclicTransition_hpp

#include "ofMain.h"
#include "projectionSimpleShader.h"

class projectionCyclicTransition : public projectionFading {
private:
    ofFbo currentStateFbo;
    ofFbo resultFbo;
    
    float lastUpdate = 0;
    
    ofShader cyclicShader;
    ofShader rainbowShader;
public:
    const float CYCLE_PERIOD = 1; // period when to go through the whole transition cycle when the canvas is pressed to the maximum
    
    bool setSize(int width, int height);
    void update();
    void draw();
    
    projectionCyclicTransition();
};

#endif /* projectionCyclicTransition_hpp */
