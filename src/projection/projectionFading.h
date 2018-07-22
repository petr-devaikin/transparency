//
//  projection.hpp
//  transparency
//
//  Created by Petr Devaikin on 10.02.18.
//

#ifndef projection_hpp
#define projection_hpp

#include "ofMain.h"
#include "touchArea.h"
#include "ofxCv.h"

class projectionFading {
private:
    touchArea * touch;
    
    ofFbo tempFbo; // fbo for mixin
    
    ofShader maxShader; // to leave touch trace
    ofShader finalShader; // to combine two images
    
    float lastMaskUpdate;
protected:
    ofVec2f position;
    ofVec2f size;
    ofFbo maskFbo; // transformed mask from touchArea
public:
    const float FADE_PERIOD = 3; // how long white color disappear
    
    projectionFading();
    
    void setPosition(int x, int y);
    bool setSize(int width, int height);
    void setX(int x);
    void setY(int y);
    void setWidth(int width);
    void setHeight(int height);
    
    int getWidth();
    int getHeight();
    
    
    ofVec2f getPosition();
    ofVec2f getSize();
    
    void setTouchArea(touchArea * t);
    
    void update();
    
    void drawBorder();
    void draw();
};

#endif /* projection_hpp */
