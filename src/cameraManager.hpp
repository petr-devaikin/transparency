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
    ofxCvGrayscaleImage depthImage;
    
    bool zeroLevelSet = false;
    ofxCvShortImage tmpImage;
    ofxCvShortImage zeroImage;
    ofxCvShortImage lastImage;
    ofxCvShortImage lastImageScaled; // from 0 to 4 meters
    ofxCvGrayscaleImage resultImage;
    
    rs2::pipeline pipe;
    rs2::pipeline_profile profile;
    rs2::frameset frames;
    
    float exposure;
    int downsampling; // to reduce resolution
    
    float depthScale; // depth bite to meters
    float maxDepth; // max depth
    float rangeK; // to "scale" depth pixels
    
    // Filters
    //rs2::decimation_filter dec_filter;
    //rs2::spatial_filter spat_filter;
    rs2::temporal_filter temp_filter;
    rs2::hole_filling_filter hole_filter;
    
    rs2::disparity_transform depth_to_disparity;
    rs2::disparity_transform disparity_to_depth;
    
    string cameraName = "";
    bool cameraFound;
    
    bool depthScaled; // if already transformed after fetching
public:
    cameraManager(int width = 1280, int height = 720, float maxDepth = .5, float exposure = 33000, int downsampling = 2);
    ~cameraManager();
    
    bool findCamera(float laserPower = 150);
    void disconnectCamera();
    bool isCameraFound();
    
    void setExposure(float e);
    float getExposure();
    
    float getMaxDepth();
    
    string getCameraName();
    
    void setZeroLevel(); // update zero level from current depth buffer
    
    void update();
    
    int getWidth();
    int getHeight();
    ofImage getRGBImage();
    ofxCvGrayscaleImage * getDepthImage();
    ofxCvGrayscaleImage * getSubstractedImage(); // return last - zero image in ROI; maxDepth in mm
    
    void setRoi(ofRectangle roi);
};

#endif /* cameraManager_hpp */
