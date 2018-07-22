//
//  projectionFluctuation.hpp
//  transparency
//
//  Created by Petr Devaikin on 20.07.18.
//

#ifndef projectionFluctuation_hpp
#define projectionFluctuation_hpp

#include "ofMain.h"
#include "projectionSimpleShader.h"

class projectionFluctuation : public projectionSimpleShader {
private:
    float startTimestamp;
protected:
    void prepareShader();
public:
    const float FLUCTUATION_PERIOD = 1; // in seconds
    
    projectionFluctuation();
};

#endif /* projectionFluctuation_hpp */
