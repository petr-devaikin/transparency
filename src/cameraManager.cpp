//
//  cameraManager.cpp
//  transparency
//
//  Created by Petr Devaikin on 01.12.18.
//

#include "cameraManager.hpp"

cameraManager::cameraManager(int cameraWidth, int cameraHeight, float maxDepth, float exposure, int downsampling) {
    this->maxDepth = maxDepth;
    this->cameraWidth = cameraWidth;
    this->cameraHeight = cameraHeight;
    width = cameraWidth / downsampling;
    height = cameraHeight / downsampling;
    this->exposure = exposure;
    this->downsampling = downsampling;
    
    roi.set(0, 0, width, height);
    
    cameraFound = false;
    depthScale = 1;
    rangeK = 65535;
    
    // init filters
    //depth_to_disparity = rs2::disparity_transform(true);
    //disparity_to_depth = rs2::disparity_transform(false);
    
    // allocate rgb image
    rgbCameraImage.allocate(cameraWidth, cameraHeight);
    rgbCameraImageResized.allocate(width, height);
    
    // allocate depth images
    depthImage.setUseTexture(false);
    depthImage.allocate(cameraWidth, cameraHeight);
    
    tmpImage.setUseTexture(false);
    tmpImage.allocate(width, height);
    
    substractedImage.allocate(width, height);
    
    zeroImage.setUseTexture(false);
    zeroImage.allocate(width, height);
    
    lastImageReranged.setUseTexture(false);
    lastImageReranged.allocate(width, height);
    depthVisualizationImage.allocate(width, height);
};

cameraManager::~cameraManager() {
}


bool cameraManager::findCamera(float laserPower) {
    cout << "Looking for RealSense\n";
    rs2::config cfg;
    cfg.enable_stream(RS2_STREAM_DEPTH, cameraWidth, cameraHeight);
    cfg.enable_stream(RS2_STREAM_COLOR, cameraWidth, cameraHeight);
    rs2::context ctx;
    auto device_list = ctx.query_devices();
    
    if (device_list.size() > 0) {
        profile = pipe.start(cfg);
        auto depth_sensor = profile.get_device().first<rs2::depth_sensor>();
        
        cameraName = depth_sensor.get_info(RS2_CAMERA_INFO_NAME);
        cameraName += " USB";
        cameraName += depth_sensor.get_info(RS2_CAMERA_INFO_USB_TYPE_DESCRIPTOR);
        
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
    /*
    rs2::frame rgbFrame = frames.get_color_frame();
    memcpy(rgbCameraImage.getPixels().getData(), rgbFrame.get_data(), cameraWidth * cameraHeight * 3);
    rgbCameraImage.flagImageChanged();
    rgbCameraImageResized.scaleIntoMe(rgbCameraImage);
     */
    
    // update depth buffer
    rs2::depth_frame depthFrame = frames.get_depth_frame();
    
    if (applyFilters) {
        // apply filters
        //depthFrame = depth_to_disparity.process(depthFrame);
        //depthFrame = spat_filter.process(depthFrame);
        depthFrame = temp_filter.process(depthFrame);
        //depthFrame = disparity_to_depth.process(depthFrame);
        depthFrame = hole_filter.process(depthFrame);
    }
    
    // copy depth data
    memcpy((depthImage.getShortPixelsRef()).getData(), depthFrame.get_data(), cameraWidth * cameraHeight * 2);
    depthImage.flagImageChanged();
    
    lastImageReranged.scaleIntoMe(depthImage); // copy depth values
    depthVisualizationCalculated = false;
    
    // if zero level is set, substract it
    if (zeroLevelSet) {
        tmpImage = zeroImage;
        
        lastImageReranged.setROI(roi); // use that picture to calculate substracted image
        tmpImage -= lastImageReranged;
        tmpImage.convertToRange(0, rangeK);
        substractedImage = tmpImage; // convert to grayscale
        //substractedImage *= maskImage;
        
        lastImageReranged.resetROI(); // substracted image calculated, reset roi (to calibrate exposure only)
    }
}

void cameraManager::setZeroLevel() {
    zeroLevelSet = true;
    zeroImage.resetROI();
    zeroImage.scaleIntoMe(depthImage);
    zeroImage.setROI(roi);
}

void cameraManager::setMask(ofPolyline mask) {
    roi = mask.getBoundingBox();
    zeroImage.setROI(roi);
    substractedImage.allocate(roi.width, roi.height);
    tmpImage.allocate(roi.width, roi.height);
    
    /*
    // preparing mask
    ofFbo tempFbo;
    tempFbo.allocate(roi.width, roi.height, GL_UNSIGNED_BYTE);
    tempFbo.begin();
    ofSetColor(255);
    ofBeginShape();
    for(int i = 0; i < mask.getVertices().size(); i++) {
        ofVertex(mask.getVertices().at(i).x - roi.getX(), mask.getVertices().at(i).y - roi.getY());
    }
    ofEndShape();
    tempFbo.end();
    
    ofPixels pixels;
    tempFbo.readToPixels(pixels);
    
    maskImage.allocate(roi.width, roi.height);
    maskImage.setFromPixels(pixels);
     */
}

ofxCvColorImage & cameraManager::getRGBImage() {
    return rgbCameraImageResized;
}

ofxCvGrayscaleImage & cameraManager::getSubstractedImage() {
    return substractedImage;
}

ofxCvGrayscaleImage & cameraManager::getDepthVisualizationImage() {
    if (!depthVisualizationCalculated) {
        depthVisualizationCalculated = true;
        lastImageReranged.convertToRange(0, 65535.0 * 65535.0 * depthScale / 4); // from 0 to 4 meters
    }
    depthVisualizationImage = lastImageReranged;
    return depthVisualizationImage;
}

int cameraManager::getWidth() {
    return width;
}

int cameraManager::getHeight() {
    return height;
}

void cameraManager::startApplyingFilters() {
    applyFilters = true;
}
