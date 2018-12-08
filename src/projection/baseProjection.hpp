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
    
    bool started;
public:
    baseProjection(touchArea * t);
    bool start();
    bool stop();
    
    bool setPosition(float x, float y);
    ofVec2f getPosition();
    bool setX(float x);
    float getX();
    bool setY(float y);
    float getY();
    
    bool setSize(int width, int height);
    bool setWidth(int width);
    float getWidth();
    bool setHeight(int height);
    ofVec2f getSize();
    float getHeight();
    
    virtual void update() = 0;
    
    void drawBorder();
    virtual void draw() = 0;
};

#endif /* projection_hpp */
