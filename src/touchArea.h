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
    ofPixels depthImage;
    
public:
    const int WIDTH = 300;
    const int HEIGHT = 600;
    
    touchArea();
    
    vector<ofVec2f> getBorderPoints();
    ofPixels & getDepthImage();
    
    void drawBorder();
    void draw();
    
    void imitateTouch(int x, int y);
    void imitateRelease();
};

#endif /* touchArea_hpp */
