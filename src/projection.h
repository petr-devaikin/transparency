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

class projection {
private:
    ofVec2f position;
    ofVec2f size;
    
    touchArea * touch;
    ofImage image1_original, image2_original;
    ofImage image1, image2;
    
    ofFbo maskFbo; // transformed mask from touchArea
    ofFbo tempFbo; // fbo for mixin
    ofFbo resultFbo;
    
    ofShader maxShader; // to leave touch trace
    ofShader finalShader; // to combine two images
    
    float lastMaskUpdate;
public:
    const float FADE_PERIOD = 2; // how long white color disappear
    
    projection();
    
    void init(const string& image_1_path, const string& image_2_path);
    
    void setPosition(int x, int y);
    void setSize(int width, int height);
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
