//
//  touchArea.hpp
//  transparency
//
//  Created by Petr Devaikin on 10.02.18.
//

#ifndef touchArea_hpp
#define touchArea_hpp

#include "ofMain.h"
#include <librealsense2/rs.hpp>

class touchArea {
private:
    bool _testMode;
    
    const int WIDTH = 1280;
    const int HEIGHT = 720;
    
    vector<ofVec2f> borderPoints;
    
    ofFbo depthFbo; // depth points with substracted zero level
    
    ofImage brush; // touch image for imitation
    
    ofImage rgbCameraImage; // last data from sensor
    unsigned short * depthCameraData; // last data from sensor
    unsigned short * zeroDepthCameraData; // zero level pioxels
    
    void updateFromCamera(); // ger rgb and depth data from camera
    
    // RealSense
    rs2::pipeline pipe;
    rs2::frameset frames;
public:
    touchArea(bool testMode = false);
    
    int getWidth();
    int getHeight();
    
    vector<ofVec2f> getBorderPoints();
    ofFbo & getDepth();
    
    void drawBorder(int x, int y);
    void drawImage(int x, int y);
    void drawDepth(int x, int y);
    
    void imitateTouch(int x, int y);
    void imitateRelease();
    
    void setBorderPoint(int i, float x, float y);
    void updateZeroPixels();
    
    void update();
};

#endif /* touchArea_hpp */
