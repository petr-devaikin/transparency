//
//  projection.cpp
//  transparency
//
//  Created by Petr Devaikin on 10.02.18.
//

#include "projection.h"

projection::projection() {
}

void projection::setPosition(int x, int y) {
    position.set(x, y);
}

void projection::setSize(int width, int height) {
    size.set(width, height);
}

void projection::setX(int x) {
    position[0] = x;
}

void projection::setY(int y) {
    position[1] = y;
}

void projection::setWidth(int width) {
    size[0] = width;
}

void projection::setHeight(int height) {
    size[1] = height;
}

ofVec2f projection::getPosition() {
    return position;
}

ofVec2f projection::getSize() {
    return size;
}

void projection::drawBorder() {
    ofSetColor(ofColor::red);
    ofNoFill();
    ofDrawRectangle(position[0], position[1], size[0], size[1]);
}
