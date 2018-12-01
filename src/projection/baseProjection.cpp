//
//  projection.cpp
//  transparency
//
//  Created by Petr Devaikin on 23.07.18.
//

#include "baseProjection.hpp"
#include "ofxCv.h"

// Initialization

baseProjection::baseProjection(touchArea * t) {
    started = false;
    
    touch = t;
    
    size.set(0, 0);
    position.set(0, 0);
}

void baseProjection::start() {
    started = true;
}

void baseProjection::stop() {
    started = false;
}

// Size and position setters and getters

bool baseProjection::setPosition(float x, float y) {
    if (x != position[0] || y != position[1]) {
        position.set(x, y);
        return true;
    }
    else
        return false;
}

bool baseProjection::setX(float x) {
    return setPosition(x, position[1]);
}

bool baseProjection::setY(float y) {
    return setPosition(position[0], y);
}

bool baseProjection::setSize(int width, int height) {
    if (width != size[0] || height != size[1]) {
        size.set(width, height);
        
        // update result screen size in touch area
        // touch->setResultScreenSize(size);
        
        return true;
    }
    else
        return false;
}

bool baseProjection::setWidth(int width) {
    return setSize(width, size[1]);
}

bool baseProjection::setHeight(int height) {
    return setSize(size[0], height);
}

ofVec2f baseProjection::getPosition() {
    return position;
}

ofVec2f baseProjection::getSize() {
    return size;
}

float baseProjection::getX() {
    return position[0];
}

float baseProjection::getY() {
    return position[1];
}

float baseProjection::getWidth() {
    return size[0];
}

float baseProjection::getHeight() {
    return size[1];
}

// Drawing

void baseProjection::drawBorder() {
    ofSetColor(255, 0, 0);
    ofNoFill();
    ofDrawRectangle(position[0], position[1], size[0], size[1]);
    ofFill();
    ofSetColor(255);
}
