//
//  projection.cpp
//  transparency
//
//  Created by Petr Devaikin on 23.07.18.
//

#include "baseProjection.hpp"
#include "ofxOpenCv.h"

using namespace cv;

// Initialization

baseProjection::baseProjection(touchArea * t) {
    started = false;
    
    touch = t;
    
    //size = _size;
    
    transform = ofMatrix4x4(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);
}

void baseProjection::setOutputPolyline(ofPolyline points) {
    calculateTransformation(points);
}

void baseProjection::calculateTransformation(ofPolyline outputArea) {
    // update transformation
    vector<Point2f> srcPoints, dstPoints;
    
    srcPoints.push_back(Point2f(0, 0));
    srcPoints.push_back(Point2f(width, 0));
    srcPoints.push_back(Point2f(width, height));
    srcPoints.push_back(Point2f(0, height));
    
    for (int i = 0; i < 4; i++) {
        dstPoints.push_back(Point2f(outputArea[i][0], outputArea[i][1]));
    }
    
    Mat m = findHomography(srcPoints, dstPoints);
    transform.set(m.at<double>(0, 0), m.at<double>(1, 0), 0, m.at<double>(2, 0),
                  m.at<double>(0, 1), m.at<double>(1, 1), 0, m.at<double>(2, 1),
                  0, 0, 1, 0,
                  m.at<double>(0, 2), m.at<double>(1, 2), 0, 1);
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
