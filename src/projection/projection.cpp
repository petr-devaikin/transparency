//
//  projection.cpp
//  transparency
//
//  Created by Petr Devaikin on 23.07.18.
//

#include "projection.hpp"
#include "ofxCv.h"

// Initialization

projection::projection() {
    size.set(0, 0);
    position.set(0, 0);
}

void projection::setTouchArea(touchArea * t) {
    touch = t;
}

// Size and position setters and getters

bool projection::setPosition(float x, float y) {
    if (x != position[0] || y != position[1]) {
        position.set(x, y);
        return true;
    }
    else
        return false;
}

bool projection::setX(float x) {
    return setPosition(x, position[1]);
}

bool projection::setY(float y) {
    return setPosition(position[0], y);
}

bool projection::setSize(int width, int height) {
    if (width != size[0] || height != size[1]) {
        size.set(width, height);
        return true;
    }
    else
        return false;
}

bool projection::setWidth(int width) {
    return setSize(width, size[1]);
}

bool projection::setHeight(int height) {
    return setSize(size[0], height);
}

ofVec2f projection::getPosition() {
    return position;
}

ofVec2i projection::getSize() {
    return size;
}

float projection::getX() {
    return position[0];
}

float projection::getY() {
    return position[1];
}

int projection::getWidth() {
    return size[0];
}

int projection::getHeight() {
    return size[1];
}

// Drawing

void projection::drawBorder() {
    ofSetColor(255, 0, 0);
    ofNoFill();
    ofDrawRectangle(position[0], position[1], size[0], size[1]);
}
