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
#include "ofxOpenCv.h"

class cameraManager {
private:
    int width;
    int height;
    
    ofImage rgbCameraImage; // last data from sensor
    
    bool zeroLevelSet = false;
    ofxCvShortImage tmpImage;
    ofxCvShortImage zeroImage;
    ofxCvShortImage lastImage;
    ofxCvGrayscaleImage resultImage;
    
    rs2::pipeline pipe;
    rs2::frameset frames;
    
    float depthScale; // depth bite to meters
    float maxDepth; // max depth
    float rangeK; // to "scale" depth pixels
    
    // Filters
    rs2::decimation_filter dec_filter;
    rs2::spatial_filter spat_filter;
    rs2::temporal_filter temp_filter;
    rs2::hole_filling_filter hole_filter;
    
    rs2::disparity_transform depth_to_disparity;
    rs2::disparity_transform disparity_to_depth;
    
    bool cameraFound;
public:
    cameraManager(float maxDepth, int width = 640, int height = 360);
    ~cameraManager();
    
    bool findCamera();
    void disconnectCamera();
    bool isCameraFound();
    
    void setZeroLevel(); // update zero level from current depth buffer
    
    void update();
    
    int getWidth();
    int getHeight();
    ofImage getRGBImage();
    ofxCvGrayscaleImage & getSubstractedImage(); // return last - zero image in ROI; maxDepth in mm
    
    void setRoi(ofRectangle roi);
};

#endif /* cameraManager_hpp */
