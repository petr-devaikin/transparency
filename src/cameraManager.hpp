//
//  cameraManager.hpp
//  transparency
//
//  Created by Petr Devaikin on 01.12.18.
//

#ifndef cameraManager_hpp
#define cameraManager_hpp

#include "ofMain.h"
#include <librealsense2/rs.hpp>

class cameraManager {
private:
    const int WIDTH = 1280;
    const int HEIGHT = 720;
    
    ofImage rgbCameraImage; // last data from sensor
    unsigned short * depthCameraData; // last data from sensor
    unsigned short * zeroDepthCameraData; // zero level pioxels
    
    rs2::pipeline pipe;
    rs2::frameset frames;
    
    float depthScale; // depth bite to meters
    
    // Filters
    rs2::decimation_filter dec_filter;
    rs2::spatial_filter spat_filter;
    rs2::temporal_filter temp_filter;
    rs2::hole_filling_filter hole_filter;
    
    rs2::disparity_transform depth_to_disparity;
    rs2::disparity_transform disparity_to_depth;
    
    bool cameraFound;
public:
    cameraManager();
    ~cameraManager();
    
    bool findCamera();
    void disconnectCamera();
    bool isCameraFound();
    
    void setZeroLevel(); // update zero level from current depth buffer
    
    void update();
    
    int getWidth();
    int getHeight();
    ofImage getRGBImage();
    
    float getDistanceChange(int i, int j); // return distance from the zero level in meters
};

#endif /* cameraManager_hpp */
