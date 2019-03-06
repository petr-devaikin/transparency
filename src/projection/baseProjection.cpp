//
//  projection.cpp
//  transparency
//
//  Created by Petr Devaikin on 23.07.18.
//

#include "baseProjection.hpp"

using namespace cv;

// Initialization

baseProjection::baseProjection(touchArea * t) {
    started = false;
    
    touch = t;
}


bool baseProjection::start() {
    if (!started) {
        started = true;
        return true;
    }
    else
        return false;
}

bool baseProjection::stop() {
    if (started) {
        started = false;
        return true;
    }
    else
        return false;
}
