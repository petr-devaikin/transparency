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
    int cameraWidth;
    int cameraHeight;
    int width;
    int height;
    
    ofRectangle roi;
    
    ofxCvColorImage rgbCameraImage; // last data from sensor
    ofxCvColorImage rgbCameraImageResized; // last data from sensor
    
    bool zeroLevelSet = false;
    ofxCvShortImage depthImage; // latest depth pixels
    
    ofxCvShortImage tmpImage; // to calculate difference
    ofxCvShortImage zeroImage;
    ofxCvGrayscaleImage substractedImage;
    
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
    
    ofxCvShortImage lastImageReranged; // from 0 to 4 meters
    ofxCvGrayscaleImage depthVisualizationImage; // converted to grayscale
    bool depthVisualizationCalculated; // if already transformed after fetching
    
    //ofxCvGrayscaleImage maskImage; // to leave pixels only inside the polyline
    
    bool applyFilters = false;
public:
    cameraManager(int cameraWidth = 1280, int cameraHeight = 720, float maxDepth = .5, float exposure = 33000, int downsampling = 2);
    ~cameraManager();
    
    bool findCamera(float laserPower = 150);
    void disconnectCamera();
    bool isCameraFound();
    
    void startApplyingFilters();
    
    void setExposure(float e);
    float getExposure();
    
    float getMaxDepth();
    
    string getCameraName();
    
    void setZeroLevel(); // update zero level from current depth buffer
    
    void update();
    
    int getWidth();
    int getHeight();
    ofxCvColorImage & getRGBImage();
    ofxCvGrayscaleImage & getDepthVisualizationImage();
    ofxCvGrayscaleImage & getSubstractedImage(); // return last - zero image in ROI; maxDepth in mm
    
    void setMask(ofPolyline mask);
};

#endif /* cameraManager_hpp */
