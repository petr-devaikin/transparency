//
//  touchArea.cpp
//  transparency
//
//  Created by Petr Devaikin on 10.02.18.
//

#include "touchArea.h"
#include "ofxCv.h"

touchArea::touchArea(bool testMode) {
    _testMode = testMode;
    
    //zeroLevelPixels.allocate(WIDTH, HEIGHT, OF_IMAGE_GRAYSCALE);
    //zeroLevelPixels.clear();
    
    brush.load("brush.png");
    brush.setImageType(OF_IMAGE_GRAYSCALE);
    
    depthFbo.allocate(WIDTH, HEIGHT, GL_RGBA);
    depthFbo.begin();
    {
        ofClear(0, 0, 0, 255);
    }
    depthFbo.end();
    
    // zeroLevels
    rgbCameraImage.allocate(WIDTH, HEIGHT, OF_IMAGE_COLOR);
    depthCameraData = new unsigned short [WIDTH * HEIGHT](); // allocate with zeros
    zeroDepthCameraData = new unsigned short [WIDTH * HEIGHT](); // allocate with zeros
    
    // init border points with 0
    borderPoints.push_back(ofVec2f(0, 0));
    borderPoints.push_back(ofVec2f(0, 0));
    borderPoints.push_back(ofVec2f(0, 0));
    borderPoints.push_back(ofVec2f(0, 0));
    
    
    // depth camera
    rs2::config cfg;
    cfg.enable_stream(RS2_STREAM_DEPTH, WIDTH, HEIGHT);
    cfg.enable_stream(RS2_STREAM_COLOR, WIDTH, HEIGHT);
    rs2::context ctx;
    auto device_list = ctx.query_devices();
    
    if (device_list.size() > 0) {
        pipe.start(cfg);
        frames = pipe.wait_for_frames();
        cout << "Camera found!\n";
    }
    else {
        cout << "Camera not found!\n";
        _testMode = true;
    }
}

void touchArea::updateZeroPixels() {
    if (!_testMode) {
        // copy zero level from current frame
        memcpy(zeroDepthCameraData, depthCameraData, WIDTH * HEIGHT * 2);
    }
}

void touchArea::drawBorder(int x, int y) {
    ofSetColor(0, 255, 0);
    ofNoFill();
    ofPolyline border;
    for (int i = 0; i < borderPoints.size(); i++) {
        border.addVertex(borderPoints[i] + ofVec2f(x, y));
    }
    border.close();
    border.draw();
    
    ofNoFill();
    ofSetColor(100, 100, 100);
    ofDrawRectangle(x, y, WIDTH, HEIGHT);
}

void touchArea::drawImage(int x, int y) {
    ofSetColor(255, 255, 255, 200);
    rgbCameraImage.draw(x, y);
}

void touchArea::drawDepth(int x, int y) {
    ofSetColor(255, 255, 255, 200);
    depthFbo.draw(x, y);
}

void touchArea::updateFromCamera() {
    rs2::frameset newFrames;
    if (pipe.poll_for_frames(&newFrames))
        frames = newFrames;
    
    // update rgb picture
    rs2::frame rgbFrame = frames.get_color_frame();
    memcpy(rgbCameraImage.getPixels().getData(), rgbFrame.get_data(), WIDTH * HEIGHT * 3);
    rgbCameraImage.update();
    
    // update depth buffer
    rs2::depth_frame depthFrame = frames.get_depth_frame();
    memcpy(depthCameraData, depthFrame.get_data(), WIDTH * HEIGHT * 2);
    
    // calculate substracted depth result
    ofImage substractedImage;
    substractedImage.allocate(WIDTH, HEIGHT, OF_IMAGE_GRAYSCALE);
    
    // iterate only withing borders !!!
    for (int i = 0; i < WIDTH; i++)
        for (int j = 0; j < HEIGHT; j++) {
            substractedImage.getPixels().getData()[j * WIDTH + i] = depthCameraData[j * WIDTH + i] - zeroDepthCameraData[j * WIDTH + i];
        }
    
    depthFbo.begin();
    {
        ofClear(0, 0, 0, 255);
        substractedImage.draw(0, 0);
    }
    depthFbo.end();
}

ofFbo & touchArea::getDepth() {
    return depthFbo;
}

vector<ofVec2f> touchArea::getBorderPoints() {
    return borderPoints;
}

void touchArea::setBorderPoint(int i, float x, float y) {
    borderPoints[i].set(x, y);
}

void touchArea::imitateTouch(int x, int y) {
    depthFbo.begin();
    {
        ofClear(0, 0, 0, 255);
        ofSetColor(255);
        ofSetColor(100);
        brush.draw(x - brush.getWidth(), y - brush.getHeight(), brush.getWidth() * 2, brush.getHeight() * 2);
    }
    depthFbo.end();
}

void touchArea::imitateRelease() {
    depthFbo.begin();
    {
        ofClear(0, 0, 0, 255);
    }
    depthFbo.end();
}

void touchArea::update() {
    if (!_testMode)
        updateFromCamera();
}

int touchArea::getWidth() {
    return WIDTH;
}

int touchArea::getHeight() {
    return HEIGHT;
}
