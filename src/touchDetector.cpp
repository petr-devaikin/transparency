//
//  touchDetector.cpp
//  transparency
//
//  Created by Petr Devaikin on 10.03.19.
//

#include "touchDetector.hpp"

touchDetector::touchDetector(float threshold) {
    this->threshold = threshold;
}

vector<ofPoint> touchDetector::detect(touchArea * touch) {
    vector<ofPoint> result;
    
    ofPixels touchPixels;
    // get current touch canvas
    
    // ! improve here asking for unsigned char array instead of ofImage!
    (touch->getTouch()).readToPixels(touchPixels);
    touchAreaImage.setFromPixels(touchPixels.getChannel(0));
    //touchAreaImage.setFromPixels(touch->getTouchPixels(), touch->getResultWidth(), touch->getResultHeight());
    touchAreaImage.threshold(255 * threshold);
    
    contourFinder.findContours(touchAreaImage, 4, touch->getResultWidth() * touch->getResultHeight(), 4, false);
    
    for (int i = 0; i < contourFinder.nBlobs; i++) {
        ofxCvBlob blob = contourFinder.blobs[i];
        result.push_back(blob.centroid);
    }
    
    return result;
}
