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
    ofImage image1, image2;
    
    ofFbo maskFbo; // transformed mask from touchArea
    ofFbo resultFbo;
    
    cv::Mat homography;
    
    ofShader shader;
    
    float lastMaskUpdate;
    
public:
    const float FADE_PERIOD = 3;
    
    projection();
    
    void setPosition(int x, int y);
    void setSize(int width, int height);
    void setX(int x);
    void setY(int y);
    void setWidth(int width);
    void setHeight(int height);
    
    ofVec2f getPosition();
    ofVec2f getSize();
    
    void setTouchArea(touchArea * t);
    void updateMapping();
    
    void update();
    
    void drawBorder();
    void drawBoard(); // for calibration
    void draw();
};

#endif /* projection_hpp */
