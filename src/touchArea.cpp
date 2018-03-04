//
//  touchArea.cpp
//  transparency
//
//  Created by Petr Devaikin on 10.02.18.
//

#include "touchArea.h"

touchArea::touchArea(bool testMode) {
    _testMode = testMode;
    
    zeroLevelPixels.allocate(WIDTH, HEIGHT, OF_IMAGE_GRAYSCALE);
    //zeroLevelPixels.clear();
    
    brush.load("brush.png");
    brush.setImageType(OF_IMAGE_GRAYSCALE);
    
    depthFbo.allocate(WIDTH, HEIGHT, GL_RGBA);
    depthFbo.begin();
    {
        ofClear(0, 0, 0, 255);
    }
    depthFbo.end();
    
    depthFboRaw.allocate(WIDTH, HEIGHT, GL_RGBA);
    depthFboRaw.begin();
    {
        ofClear(0, 0, 0, 255);
    }
    depthFboRaw.end();
    
    // init border points with 0
    
    borderPoints.push_back(ofVec2f(0, 0));
    borderPoints.push_back(ofVec2f(0, 0));
    borderPoints.push_back(ofVec2f(0, 0));
    borderPoints.push_back(ofVec2f(0, 0));
    
    // init marker recognizer
    
    //aruco.setup("board/intrinsics.yml", WIDTH, HEIGHT);
    //aruco.setMinMaxMarkerDetectionSize(0.001, 0.25);
    ofVec2f camSize(WIDTH, HEIGHT);
    art.setup(camSize, camSize);
    boardImage.load("board/board.png"); // load board image to dispolay
    
    // only for testing. image to recognize markers
    if (_testMode) {
        fakeImage.load("board/test_image.jpg");
        fakeImage.setImageType(OF_IMAGE_COLOR);
        
        // crop it!
        if (fakeImage.getWidth() / fakeImage.getHeight() > WIDTH / (float) HEIGHT) {
            float diff = fakeImage.getWidth() - fakeImage.getHeight() * WIDTH / (float) HEIGHT;
            fakeImage.crop(diff / 2, 0, fakeImage.getWidth() - diff, fakeImage.getHeight());
        }
        else {
            float diff = fakeImage.getHeight() - fakeImage.getWidth() * HEIGHT / (float) WIDTH;
            fakeImage.crop(0, diff / 2, fakeImage.getWidth(), fakeImage.getHeight() - diff);
        }
        fakeImage.resize(WIDTH, HEIGHT);
    }
}

ofImage & touchArea::getSensorImage() {
    return fakeImage;
}

void touchArea::updateZeroPixels() {
    // update zero level from depth camera
    
    if (_testMode) {
        // immitate some depth values. test
        for (int i = 0; i < WIDTH; i++)
            for (int j = 0; j < HEIGHT; j++)
                zeroLevelPixels.setColor(i, j, ofColor(ofVec2f(i, j).distance(ofVec2f(0, 0)) * 100.0 / WIDTH));
        zeroLevelPixels.update();
        
        // this is only for tests
        depthFboRaw.begin();
        {
            ofDisableBlendMode();
            zeroLevelPixels.draw(0, 0);
        }
        depthFboRaw.end();
    }
}

void touchArea::recognizeBorders() {
    // recognize markers
    /*
    art.ofxArtool5::GenericTracker::update(getSensorImage().getPixels());
    if (art.isFound()) {
        int n = art.getNumMarkers();
        ARMarkerInfo marker = art.getMarker(0);
        for (int i = 0; i < 4; i++) {
            borderPoints[i] = ofVec2f(marker.vertex[i][0], marker.vertex[i][1]);
        }
        for (int i = 0; i < art.getNumMarkers(); i++)
            cout << art.getMarker(i).idPatt << " " << art.getMarker(i).id << "\n";
        /*
        borderPoints[0] = ofVec2f(p1[0], p1[1]);
        borderPoints[1] = ofVec2f(p2[0], p2[1]);
        borderPoints[2] = ofVec2f(p3[0], p3[1]);
        borderPoints[3] = ofVec2f(p4[0], p4[1]);
     
    }
    */
    
    // update pixels
    updateZeroPixels();
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
    getSensorImage().draw(x, y);
}

void touchArea::drawDepth(int x, int y) {
    ofSetColor(255, 255, 255, 200);
    depthFboRaw.draw(x, y);
}

void touchArea::updateDepth() {
    // get depthFboRaw from camera
    
    // prepare depthRbo
    depthFbo.begin();
    {
        ofDisableBlendMode();
        depthFboRaw.draw(0, 0);
        ofEnableBlendMode(OF_BLENDMODE_SUBTRACT);
        zeroLevelPixels.draw(0, 0);
        ofDisableBlendMode();
    }
    depthFbo.end();
}

ofFbo & touchArea::getDepth() {
    return depthFbo;
}

ofImage & touchArea::getBoard() {
    return boardImage;
}

vector<ofVec2f> touchArea::getBorderPoints() {
    return borderPoints;
}

void touchArea::setBorderPoint(int i, float x, float y) {
    borderPoints[i].set(x, y);
}

void touchArea::imitateTouch(int x, int y) {
    depthFboRaw.begin();
    {
        ofClear(0, 0, 0, 255);
        ofSetColor(255);
        zeroLevelPixels.draw(0, 0);
        ofEnableBlendMode(OF_BLENDMODE_ADD);
        brush.draw(x - brush.getWidth(), y - brush.getHeight(), brush.getWidth() * 2, brush.getHeight() * 2);
        ofDisableBlendMode();
    }
    depthFboRaw.end();
}

void touchArea::imitateRelease() {
    depthFboRaw.begin();
    {
        ofDisableBlendMode();
        zeroLevelPixels.draw(0, 0);
    }
    depthFboRaw.end();
}

void touchArea::update() {
    updateDepth();
}
