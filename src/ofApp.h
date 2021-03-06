#pragma once

#include "ofMain.h"

#include "projectionFading.h"
#include "projectionTransparency.h"
#include "projectionSimpleShader.h"
#include "projectionFluctuation.hpp"
#include "projectionCyclicTransition.hpp"
#include "projectionInvertedBrush.hpp"
#include "projectionFlowers.h"

#include "touchArea.h"
#include "ofxGui.h"
#include "ofxXmlSettings.h"

const int SENS_RANGE = 5;

const string PROJ_SETTINGS_FILE = "projection_settings.xml";
const string DEPTH_SETTINGS_FILE = "depth_settings.xml";


class ofApp : public ofBaseApp{
private:
    //projectionTransparency proj;
    //projectionSimpleShader proj;
    //projectionFluctuation proj;
    //projectionFlowers proj = projectionFlowers(100);
    //projectionCyclicTransition proj;
    projectionInvertedBrush * proj;
    touchArea * touch; // 10 cm
    
    ofxPanel gui;
    
    ofxToggle showGui;
    
    ofxToggle modeNormal;
    ofxToggle modeDepth;
    ofxToggle modeOutputCalibration;
    ofxToggle modeInputCalibration;
    
    void setNormalMode(bool& toggled);
    void setDepthMode(bool& toggled);
    void setOutputCalibrationMode(bool& toggled);
    void setInputCalibrationMode(bool& toggled);
    
    //ofParameter<int> maxDepth;
    
    bool dragProjection = false;
    bool resizeProjectionX = false;
    bool resizeProjectionY = false;
    bool dragTouchArea = false;
    int dragTouchAreaCornerNumber = 0;
    
    ofVec2f dragStart;
    
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
