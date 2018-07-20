//
//  projectionFluctuation.cpp
//  transparency
//
//  Created by Petr Devaikin on 20.07.18.
//

#include "projectionFluctuation.hpp"

projectionFluctuation::projectionFluctuation() : projectionSimpleShader("shadersGL3/fluctuation") {
    startTimestamp = ofGetElapsedTimef();
}

void projectionFluctuation::prepareShader() {
    // send current wave value calculated from current time and of get elapsed timef to shader
}
