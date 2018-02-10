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
    ofImage depthImage;
    
public:
    touchArea();
    
    vector<ofVec2f> getBorderPoints();
    ofImage getDepthImage();
    
    void drawBorder();
    void draw();
    
    void imitateTouch(int x, int y);
    void imitateRelease();
};

#endif /* touchArea_hpp */
