//
//  touchArea.cpp
//  transparency
//
//  Created by Petr Devaikin on 10.02.18.
//

#include "touchArea.h"

touchArea::touchArea() {
    borderPoints.push_back(ofVec2f(100, 300));
    borderPoints.push_back(ofVec2f(200, 290));
    borderPoints.push_back(ofVec2f(220, 500));
    borderPoints.push_back(ofVec2f(80, 540));
}

void touchArea::drawBorder() {
    ofSetColor(ofColor::blue);
    ofNoFill();
    ofPolyline border;
    for (int i = 0; i < borderPoints.size(); i++) {
        border.addVertex(borderPoints[i]);
    }
    border.close();
    border.draw();
}
