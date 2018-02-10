//
//  projection.hpp
//  transparency
//
//  Created by Petr Devaikin on 10.02.18.
//

#ifndef projection_hpp
#define projection_hpp

#include "ofMain.h"

class projection {
private:
    ofVec2f position;
    ofVec2f size;
    
public:
    
    projection();
    
    void setPosition(int x, int y);
    void setSize(int width, int height);
    void setX(int x);
    void setY(int y);
    void setWidth(int width);
    void setHeight(int height);
    
    ofVec2f getPosition();
    ofVec2f getSize();
    
    void drawBorder();
};

#endif /* projection_hpp */
