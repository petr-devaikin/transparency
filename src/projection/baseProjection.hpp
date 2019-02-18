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
    int width;
    int height;
    touchArea * touch;
    
    ofMatrix4x4 transform;
    void calculateTransformation(ofPolyline outputArea);
    
    bool started;
public:
    baseProjection(touchArea * t);
    
    bool start();
    bool stop();
    
    void setOutputPolyline(ofPolyline points);
    
    virtual void update() = 0;
    
    virtual void draw() = 0;
};

#endif /* projection_hpp */
