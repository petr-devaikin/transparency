#pragma once

#include "ofMain.h"

#include "projectionInvertedBrush.hpp"

#include "touchArea.h"
#include "cameraManager.hpp"
#include "calibrator.hpp"
#include "ofxGui.h"
#include "ofxXmlSettings.h"

const int SENS_RANGE = 5;

const string BASE_PATH = "";

const string PROJ_SETTINGS_FILE = "projection_settings.xml";


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
