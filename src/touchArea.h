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
#include "ofxCv.h"

class touchArea {
private:
    bool testMode;
    
    const int WIDTH = 1280;
    const int HEIGHT = 720;
    
    float depth_scale = 1; // depth bite to meters
    
    vector<ofVec2f> touchBorderPoints;
    ofRectangle boundingArea;
    float maxDepth; // max depth in mm to scale the depth image
    
    ofFbo depthFbo; // depth points with substracted zero level, transformed to the result screen projection
    
    ofImage brush; // touch image for imitation
    
    ofImage rgbCameraImage; // last data from sensor
    unsigned short * depthCameraData; // last data from sensor
    unsigned short * zeroDepthCameraData; // zero level pioxels
    
    void updateFromCamera(); // ger rgb and depth data from camera
    
    // RealSense
    bool findCamera();
    rs2::pipeline pipe;
    rs2::frameset frames;
    
    // transformation
    ofMatrix4x4 transform;
    
    // to get zero pixels in the very begining
    bool isZeroInitialized = false;
    
    // filters
    rs2::decimation_filter dec_filter;
    rs2::spatial_filter spat_filter;
    rs2::temporal_filter temp_filter;
    rs2::hole_filling_filter hole_filter;
    
    rs2::disparity_transform depth_to_disparity;
    rs2::disparity_transform disparity_to_depth;
public:
    touchArea(float _maxDepth, bool _testMode = false); // maxDepth in mm
    ~touchArea();
    
    int getWidth();
    int getHeight();
    
    vector<ofVec2f> getBorderPoints();
    ofFbo & getDepth();
    ofImage & getCameraImage();
    
    //void drawBorder(int x, int y);
    //void drawImage(int x, int y);
    //void drawDepth(int x, int y);
    
    void setBorderPoint(int i, float x, float y);
    
    void resetZeroPixels();
    void setResultScreenSize(ofVec2f screenSize);
    
    //void setMaxDepth(float maxDepth); // in mm
    
    void update();
    
    // immitation
    void imitateTouch(int x, int y);
    void imitateRelease();
};

#endif /* touchArea_hpp */
