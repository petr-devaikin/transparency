#pragma once

#include "ofMain.h"

#include "projectionInvertedBrush.hpp"

#include "touchArea.h"
#include "cameraManager.hpp"
#include "calibrator.hpp"
#include "ofxGui.h"
#include "ofxXmlSettings.h"

const int SENS_RANGE = 5; // to map areas with mouse

const string PROJ_SETTINGS_FILE = "projection_settings.xml";

const int CAMERA_WIDTH = 1280;
const int CAMERA_HEIGHT = 720;

const int IMAGE_WIDTH = 1024; // projected image original size
const int IMAGE_HEIGHT = 1024;

const int PROJ_SPEED = 50;
const int PROJ_BLUR_RADIUS = 50;
const bool PROJ_ANIMATE_DEPTH = true;

class ofApp : public ofBaseApp{
private:
    cameraManager * camera;
    calibrator * calib;
    projectionInvertedBrush * proj;
    touchArea * touch;
    
    //ofParameter<int> maxDepth;
    
    bool dragPolygonCorner = false;
    int dragCornerNumber = 0;
    
    void calibrationToggleListener(bool &toggleStatus);
    
    // settings
    void loadSettings();
    void saveSettings();

    //
    void initProjection();
public:
    void setup();
    void update();
    void draw();
    void exit();

    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void mouseEntered(int x, int y);
    void mouseExited(int x, int y);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
};
