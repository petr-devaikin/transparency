//
//  touchArea.hpp
//  transparency
//
//  Created by Petr Devaikin on 10.02.18.
//

#ifndef touchArea_hpp
#define touchArea_hpp

#include "ofMain.h"
#include "ofxARTPattern.h"

class touchArea {
private:
    vector<ofVec2f> borderPoints;
    
    ofFbo depthFbo; // depth points with substracted zero level
    
    ofPixels zeroLevelPixels; // depth points of the canvas in default state
    ofImage brush; // touch image for imitation
    
    //ofxAruco aruco;
    ofxArtool5::PatternTracker art;
    ofImage boardImage;
    
    ofImage fakeImage; // only for testing
    
    ofImage & getSensorImage();
    
public:
    const int WIDTH = 640;
    const int HEIGHT = 480;
    const float MAX_DEPTH = .5;
    
    touchArea();
    
    void recognizeBorders();
    
    vector<ofVec2f> getBorderPoints();
    ofFbo & getDepth();
    ofImage & getBoard();
    
    void drawBorder(int x, int y);
    void drawImage(int x, int y);
    void drawDepth(int x, int y);
    
    void imitateTouch(int x, int y);
    void imitateRelease();
    
    void setBorderPoint(int i, float x, float y);
};

#endif /* touchArea_hpp */
