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
    
public:
    touchArea();
    
    void drawBorder();
};

#endif /* touchArea_hpp */
