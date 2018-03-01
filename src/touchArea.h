//
//  touchArea.hpp
//  transparency
//
//  Created by Petr Devaikin on 10.02.18.
//

#ifndef touchArea_hpp
#define touchArea_hpp

#include "ofMain.h"

class touchArea {
private:
    vector<ofVec2f> borderPoints;
    
    ofFbo depthFbo; // depth points with substracted zero level
    
    ofPixels zeroLevelPixels; // depth points of the canvas in default state
    ofImage brush; // touch image for imitation
    
public:
    const int WIDTH = 600;
    const int HEIGHT = 600;
    const float MAX_DEPTH = .5;
    
    touchArea();
    
    void recognizeBorders();
    
    vector<ofVec2f> getBorderPoints();
    ofFbo & getDepth();
    
    void drawBorder(int x, int y);
    void draw(int x, int y);
    
    void imitateTouch(int x, int y);
    void imitateRelease();
    
    void setBorderPoint(int i, float x, float y);
};

#endif /* touchArea_hpp */
