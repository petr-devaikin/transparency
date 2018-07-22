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

class projectionSimpleShader : public projectionFading {
protected:
    ofFbo resultFbo;
    
    ofShader shader; // to combine two images
    virtual void prepareShader(); // to pass some parameters, for example
public:
    const float FADE_PERIOD = 2; // how long white color disappear
    
    projectionSimpleShader(string filename = "shadersGL3/rainbow_shader");
    
    bool setSize(int width, int height);
    
    void update();
    
    void draw();
};

#endif /* projection_transparency_hpp */
