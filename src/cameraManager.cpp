//
//  cameraManager.cpp
//  transparency
//
//  Created by Petr Devaikin on 01.12.18.
//

#include "cameraManager.hpp"

cameraManager::cameraManager() {
    cameraFound = false;
    depthScale = 1;
    
    // init filters
    depth_to_disparity = rs2::disparity_transform(true);
    disparity_to_depth = rs2::disparity_transform(false);
};


bool cameraManager::findCamera() {
    cout << "Looking for RealSense\n";
    rs2::config cfg;
    cfg.enable_stream(RS2_STREAM_DEPTH, WIDTH, HEIGHT);
    cfg.enable_stream(RS2_STREAM_COLOR, WIDTH, HEIGHT);
    rs2::context ctx;
    auto device_list = ctx.query_devices();
    
    if (device_list.size() > 0) {
        rs2::pipeline_profile profile = pipe.start(cfg);
        auto depth_sensor = profile.get_device().first<rs2::depth_sensor>();
        
        // get scale from the sensor
        depthScale = depth_sensor.get_depth_scale();
        
        // depth buffers
        depthCameraData = new unsigned short [WIDTH * HEIGHT](); // allocate with zeros
        zeroDepthCameraData = new unsigned short [WIDTH * HEIGHT](); // allocate with zeros
        
        // rgb image
        rgbCameraImage.allocate(WIDTH, HEIGHT, OF_IMAGE_COLOR);
        
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
        
        // remove depth buffer
        delete[] depthCameraData;
        delete[] zeroDepthCameraData;
    }
}

bool cameraManager::isCameraFound() {
    return cameraFound;
}

void cameraManager::update() {
    if (cameraFound) {
        rs2::frameset newFrames;
        if (pipe.poll_for_frames(&newFrames))
            frames = newFrames;
    }
    
    // update rgb picture
    rs2::frame rgbFrame = frames.get_color_frame();
    memcpy(rgbCameraImage.getPixels().getData(), rgbFrame.get_data(), WIDTH * HEIGHT * 3);
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
    memcpy(depthCameraData, depthFrame.get_data(), WIDTH * HEIGHT * 2);
}

void cameraManager::setZeroLevel() {
    memcpy(zeroDepthCameraData, depthCameraData, WIDTH * HEIGHT * 2);
}

ofImage cameraManager::getRGBImage() {
    return rgbCameraImage;
}

float cameraManager::getDistanceChange(int i, int j) {
    if (depthCameraData[j * WIDTH + i] == 0 || zeroDepthCameraData[j * WIDTH + i] == 0)
        return 0;
    else {
        return (zeroDepthCameraData[j * WIDTH + i] - depthCameraData[j * WIDTH + i]) * depthScale;
    }
}

int cameraManager::getWidth() {
    return WIDTH;
}

int cameraManager::getHeight() {
    return HEIGHT;
}
