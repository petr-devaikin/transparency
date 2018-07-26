//
//  projection.hpp
//  transparency
//
//  Created by Petr Devaikin on 23.07.18.
//

#ifndef projection_hpp
#define projection_hpp

#include "ofMain.h"
#include "touchArea.h"

class baseProjection {
private:
protected:
    ofVec2f position;
    ofVec2f size;
    touchArea * touch;
public:
    baseProjection();
    
    bool setPosition(float x, float y);
    bool setSize(int width, int height);
    
    bool setX(float x);
    bool setY(float y);
    bool setWidth(int width);
    bool setHeight(int height);
    
    ofVec2f getSize();
    ofVec2f getPosition();
    
    float getX();
    float getY();
    float getWidth();
    float getHeight();
    
    void setTouchArea(touchArea * t);
    
    virtual void update() = 0;
    
    void drawBorder();
    virtual void draw() = 0;
};

#endif /* projection_hpp */
