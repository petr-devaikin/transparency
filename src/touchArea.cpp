//
//  touchArea.cpp
//  transparency
//
//  Created by Petr Devaikin on 10.02.18.
//

#include "touchArea.h"

using namespace cv;

touchArea::touchArea(bool testMode) {
    _testMode = testMode;
    
    //zeroLevelPixels.allocate(WIDTH, HEIGHT, OF_IMAGE_GRAYSCALE);
    //zeroLevelPixels.clear();
    
    brush.load("brush.png");
    brush.setImageType(OF_IMAGE_GRAYSCALE);
    
    // zeroLevels
    rgbCameraImage.allocate(WIDTH, HEIGHT, OF_IMAGE_COLOR);
    depthCameraData = new unsigned short [WIDTH * HEIGHT](); // allocate with zeros
    zeroDepthCameraData = new unsigned short [WIDTH * HEIGHT](); // allocate with zeros
    
    // init border points with 0
    touchBorderPoints.push_back(ofVec2f(0, 0));
    touchBorderPoints.push_back(ofVec2f(0, 0));
    touchBorderPoints.push_back(ofVec2f(0, 0));
    touchBorderPoints.push_back(ofVec2f(0, 0));
    
    
    // looking depth camera
    if (findCamera()) {}
    else {
        _testMode = true;
    }
    
    // init transform matrix
    transform = ofMatrix4x4(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);
    depthFbo.allocate(10, 10, GL_RGBA);
    // [? ? 0 ?]
    // [? ? 0 ?]
    // [0 0 1 0]
    // [? ? 0 1]
    
    // init filters
    depth_to_disparity = rs2::disparity_transform(true);
    disparity_to_depth = rs2::disparity_transform(false);
    
    // load shader
    rainbowShader.load("shadersGL3/rainbow_shader");
}

bool touchArea::findCamera() {
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
        depth_scale = depth_sensor.get_depth_scale();
        
        frames = pipe.wait_for_frames();
        cout << "RealSense found!\n";
        return true;
    }
    else {
        cout << "Camera not found!\n";
        return false;
    }
}

touchArea::~touchArea() {
    pipe.stop();
    
    delete[] depthCameraData;
    delete[] zeroDepthCameraData;
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
    for (int i = 0; i < touchBorderPoints.size(); i++) {
        border.addVertex(touchBorderPoints[i] + ofVec2f(x, y));
    }
    border.close();
    border.draw();
    
    // label the corners
    ofDrawBitmapString("Top Left", touchBorderPoints[0][0], touchBorderPoints[0][1]);
    ofDrawBitmapString("Top Right", touchBorderPoints[1][0], touchBorderPoints[1][1]);
    ofDrawBitmapString("Bottom Right", touchBorderPoints[2][0], touchBorderPoints[2][1]);
    ofDrawBitmapString("Bottom Left", touchBorderPoints[3][0], touchBorderPoints[3][1]);
    
    ofNoFill();
    ofSetColor(100, 100, 100);
    ofDrawRectangle(x, y, WIDTH, HEIGHT);
}

void touchArea::drawImage(int x, int y) {
    ofSetColor(255, 255, 255, 200);
    rgbCameraImage.draw(x, y);
}

void touchArea::drawDepth(int x, int y) {
    ofSetColor(255);
    rainbowShader.begin();
    {
        depthFbo.draw(x, y);
    }
    rainbowShader.end();
    
    ofSetColor(0, 0, 255);
    ofNoFill();
    ofDrawRectangle(x, y, depthFbo.getWidth(), depthFbo.getHeight());
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
    
    // apply filters
    //depthFrame = dec_filter.process(depthFrame);
    /*
    depthFrame = depth_to_disparity.process(depthFrame);
    depthFrame = spat_filter.process(depthFrame);
    depthFrame = temp_filter.process(depthFrame);
    depthFrame = disparity_to_depth.process(depthFrame);
     */
    
    // copy depth data
    memcpy(depthCameraData, depthFrame.get_data(), WIDTH * HEIGHT * 2);
    
    // update zero pixels in the very beginning
    if (!isZeroInitialized) {
        isZeroInitialized = true;
        updateZeroPixels();
    }
    
    // calculate substracted depth result
    ofImage substractedImage;
    substractedImage.allocate(WIDTH, HEIGHT, OF_IMAGE_GRAYSCALE);
    
    // iterate only withing the bounding area
    for (int i = floor(boundingArea.x); i <= ceil(boundingArea.x + boundingArea.width); i++)
        for (int j = floor(boundingArea.y); j <= ceil(boundingArea.y + boundingArea.height); j++) {
            float res = 0;
            // exclude error pixels
            if (depthCameraData[j * WIDTH + i] > 0) {
                res = zeroDepthCameraData[j * WIDTH + i] - depthCameraData[j * WIDTH + i];
                res = res * depth_scale / maxDepth;
                res = pow(res, 3); // power of 3 to be sensitive more to deep touch
                res *= 255;
                if (res < 0) res = 0;
                if (res > 255) res = 255;
            }
            substractedImage.getPixels().getData()[(j * WIDTH + i)] = res;
        }
    
    depthFbo.begin();
    {
        ofClear(0, 0, 0, 255);
        
        ofPushMatrix();
        
        ofMultMatrix(transform);
        
        ofSetColor(255);
        substractedImage.draw(0, 0);
        
        ofPopMatrix();
    }
    depthFbo.end();
}

ofFbo & touchArea::getDepth() {
    return depthFbo;
}

vector<ofVec2f> touchArea::getBorderPoints() {
    return touchBorderPoints;
}

void touchArea::setBorderPoint(int i, float x, float y) {
    touchBorderPoints[i].set(x, y);
    
    // recalculate bounding rectangle
    float minX = WIDTH, minY = HEIGHT, maxX = 0, maxY = 0;
    for (int j = 0; j < 4; j++) {
        if (touchBorderPoints[j][0] < minX) minX = touchBorderPoints[j][0];
        if (touchBorderPoints[j][1] < minY) minY = touchBorderPoints[j][1];
        if (touchBorderPoints[j][0] > maxX) maxX = touchBorderPoints[j][0];
        if (touchBorderPoints[j][1] > maxY) maxY = touchBorderPoints[j][1];
    }
    boundingArea = ofRectangle(minX, minY, maxX - minX, maxY - minY);
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

void touchArea::setResultScreenSize(int width, int height) {
    // change the result buffer
    depthFbo.allocate(width, height, GL_RGBA);
    depthFbo.begin();
    {
        ofClear(0);
    }
    depthFbo.end();
    
    // update transformation

    vector<Point2f> srcPoints, dstPoints;
    for (int i = 0; i < 4; i++) {
        srcPoints.push_back(Point2f(touchBorderPoints[i][0], touchBorderPoints[i][1]));
    }
    
    dstPoints.push_back(Point2f(0, 0));
    dstPoints.push_back(Point2f(width, 0));
    dstPoints.push_back(Point2f(width, height));
    dstPoints.push_back(Point2f(0, height));
    
    
    Mat m = findHomography(srcPoints, dstPoints);
    transform.set(m.at<double>(0, 0), m.at<double>(1, 0), 0, m.at<double>(2, 0),
                  m.at<double>(0, 1), m.at<double>(1, 1), 0, m.at<double>(2, 1),
                  0, 0, 1, 0,
                  m.at<double>(0, 2), m.at<double>(1, 2), 0, 1);
}

void touchArea::setMaxDepth(float m) {
    maxDepth = m;
}
