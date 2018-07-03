//
//  projection.hpp
//  transparency
//
//  Created by Petr Devaikin on 10.02.18.
//

#ifndef projection_rainbow_hpp
#define projection_rainbow_hpp

#include "ofMain.h"
#include "projectionFading.h"

class projectionRainbow : public projectionFading {
private:
    ofFbo resultFbo;
    
    ofShader rainbowShader; // to combine two images
public:
    const float FADE_PERIOD = 2; // how long white color disappear
    
    projectionRainbow();
    
    void setSize(int width, int height);
    
    void update();
    
    void draw();
};

#endif /* projection_transparency_hpp */
