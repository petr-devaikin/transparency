//
//  cameraManager.cpp
//  transparency
//
//  Created by Petr Devaikin on 01.12.18.
//

#include "cameraManager.hpp"

cameraManager::cameraManager(float maxDepth, int width, int height) {
    this->maxDepth = maxDepth;
    this->width = width;
    this->height = height;
    
    cameraFound = false;
    depthScale = 1;
    
    // init filters
    depth_to_disparity = rs2::disparity_transform(true);
    disparity_to_depth = rs2::disparity_transform(false);
    
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


bool cameraManager::findCamera() {
    cout << "Looking for RealSense\n";
    rs2::config cfg;
    cfg.enable_stream(RS2_STREAM_DEPTH, width, height);
    cfg.enable_stream(RS2_STREAM_COLOR, width, height);
    rs2::context ctx;
    auto device_list = ctx.query_devices();
    
    if (device_list.size() > 0) {
        rs2::pipeline_profile profile = pipe.start(cfg);
        auto depth_sensor = profile.get_device().first<rs2::depth_sensor>();
        
        // get scale from the sensor
        depthScale = depth_sensor.get_depth_scale();
        
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
    //depthFrame = dec_filter.process(depthFrame);
    
    depthFrame = depth_to_disparity.process(depthFrame);
    //depthFrame = spat_filter.process(depthFrame);
    depthFrame = temp_filter.process(depthFrame);
    depthFrame = disparity_to_depth.process(depthFrame);
    
    depthFrame = hole_filter.process(depthFrame);
    
    // copy depth data
    memcpy((lastImage.getShortPixelsRef()).getData(), depthFrame.get_data(), width * height * 2);
    lastImage.flagImageChanged();
    
    // if zero level is set, substract it
    if (zeroLevelSet) {
        tmpImage = zeroImage;
        tmpImage -= lastImage;
        resultImage = tmpImage;
        //resultImage.convertToRange(0, 255 * 65535 / maxDepth * depthScale);
        //tmpImage.convertToRange(0, 255 * 10);
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

ofxCvGrayscaleImage & cameraManager::getSubstractedImage() {
    return resultImage;
}

int cameraManager::getWidth() {
    return width;
}

int cameraManager::getHeight() {
    return height;
}
