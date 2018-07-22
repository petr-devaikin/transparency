//
//  projectionFluctuation.cpp
//  transparency
//
//  Created by Petr Devaikin on 20.07.18.
//

#include "projectionFluctuation.hpp"
#include <math.h>

projectionFluctuation::projectionFluctuation() : projectionSimpleShader("shadersGL3/fluctuation_shader") {
    startTimestamp = ofGetElapsedTimef();
}

void projectionFluctuation::prepareShader() {
    float angle = (ofGetElapsedTimef() - startTimestamp) / FLUCTUATION_PERIOD * 2 * M_PI;
    shader.setUniform1f("angle", angle);
    // send current wave value calculated from current time and of get elapsed timef to shader
}
