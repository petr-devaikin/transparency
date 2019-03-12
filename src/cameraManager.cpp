//
//  cameraManager.cpp
//  transparency
//
//  Created by Petr Devaikin on 01.12.18.
//

#include "cameraManager.hpp"

cameraManager::cameraManager(int width, int height, float maxDepth, float exposure, int downsampling) {
    this->maxDepth = maxDepth;
    this->width = width;
    this->height = height;
    this->exposure = exposure;
    this->downsampling = downsampling;
    
    cameraFound = false;
    depthScale = 1;
    rangeK = 65535;
    
    // init filters
    //depth_to_disparity = rs2::disparity_transform(true);
    //disparity_to_depth = rs2::disparity_transform(false);
    
    // allocate rgb image
    rgbCameraImage.allocate(width, height, OF_IMAGE_COLOR);
    
    // allocate depth images
    zeroImage.setUseTexture(false);
    zeroImage.allocate(width, height);
    lastImage.setUseTexture(false);
    lastImage.allocate(width, height);
};

cameraManager::~cameraManager() {
}


bool cameraManager::findCamera(float laserPower) {
    cout << "Looking for RealSense\n";
    rs2::config cfg;
    cfg.enable_stream(RS2_STREAM_DEPTH, width, height);
    cfg.enable_stream(RS2_STREAM_COLOR, width, height);
    rs2::context ctx;
    auto device_list = ctx.query_devices();
    
    if (device_list.size() > 0) {
        profile = pipe.start(cfg);
        auto depth_sensor = profile.get_device().first<rs2::depth_sensor>();
        
        cameraName = depth_sensor.get_info(RS2_CAMERA_INFO_NAME);
        
        // depth sensor settings
        depth_sensor.set_option(RS2_OPTION_VISUAL_PRESET, RS2_RS400_VISUAL_PRESET_DEFAULT);
        depth_sensor.set_option(RS2_OPTION_ENABLE_AUTO_EXPOSURE, 0.f);
        depth_sensor.set_option(RS2_OPTION_EXPOSURE, exposure);
        
        // get scale from the sensor
        depthScale = depth_sensor.get_depth_scale();
        
        // calculate scale coefficient
        rangeK = 65535.0 * 65535.0 * depthScale / maxDepth;
        
        frames = pipe.wait_for_frames();
        cout << "RealSense found!\n";
        
        cameraFound = true;
        return true;
    }
    else {
        cout << "Camera not found!\n";
        
        cameraFound = false;
        return false;
    }
}

void cameraManager::setExposure(float e) {
    if (!cameraFound) return;
    
    exposure = e;
    auto depth_sensor = profile.get_device().first<rs2::depth_sensor>();
    depth_sensor.set_option(RS2_OPTION_EXPOSURE, exposure);
}

float cameraManager::getExposure() {
    return exposure;
}

string cameraManager::getCameraName() {
    return cameraName;
}

float cameraManager::getMaxDepth() {
    return maxDepth;
}

void cameraManager::disconnectCamera() {
    if (cameraFound) {
        cameraFound = false;
        cout << "Disconnect Camera";
        pipe.stop();
    }
}

bool cameraManager::isCameraFound() {
    return cameraFound;
}

void cameraManager::update() {
    if (!cameraFound) return;
        
    rs2::frameset newFrames;
    if (pipe.poll_for_frames(&newFrames))
        frames = newFrames;
    
    // update rgb picture
    rs2::frame rgbFrame = frames.get_color_frame();
    memcpy(rgbCameraImage.getPixels().getData(), rgbFrame.get_data(), width * height * 3);
    rgbCameraImage.update();
    
    // update depth buffer
    rs2::depth_frame depthFrame = frames.get_depth_frame();
    
    // apply filters
    //depthFrame = depth_to_disparity.process(depthFrame);
    //depthFrame = spat_filter.process(depthFrame);
    depthFrame = temp_filter.process(depthFrame);
    //depthFrame = disparity_to_depth.process(depthFrame);
    
    depthFrame = hole_filter.process(depthFrame);
    
    // copy depth data
    memcpy((lastImage.getShortPixelsRef()).getData(), depthFrame.get_data(), width * height * 2);
    lastImage.flagImageChanged();
    
    // if zero level is set, substract it
    if (zeroLevelSet) {
        tmpImage = zeroImage;
        tmpImage -= lastImage;
        tmpImage.convertToRange(0, rangeK);
        resultImage = tmpImage;
    }
}

void cameraManager::setZeroLevel() {
    zeroLevelSet = true;
    zeroImage = lastImage;
}

ofImage cameraManager::getRGBImage() {
    return rgbCameraImage;
}

void cameraManager::setRoi(ofRectangle roi) {
    lastImage.setROI(roi);
    zeroImage.setROI(roi);
    resultImage.allocate(roi.width, roi.height);
    tmpImage.setUseTexture(false);
    tmpImage.allocate(roi.width, roi.height);
}

ofxCvGrayscaleImage * cameraManager::getSubstractedImage() {
    return &resultImage;
}

int cameraManager::getWidth() {
    return width;
}

int cameraManager::getHeight() {
    return height;
}
