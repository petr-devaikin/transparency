#include "ofApp.h"


//--------------------------------------------------------------
void ofApp::setup(){
    // init touch
    touch = new touchArea(.3); // 30 cm
    proj = new projectionInvertedBrush(touch);
    proj->addImage("surrender_jei_2018/surrender_001.png");
    proj->addImage("surrender_jei_2018/surrender_002.png");
    proj->addImage("surrender_jei_2018/surrender_003.png");
    proj->addImage("surrender_jei_2018/surrender_004.png");
    proj->addImage("surrender_jei_2018/surrender_005.png");
    proj->addImage("surrender_jei_2018/surrender_006.png");
    proj->addImage("surrender_jei_2018/surrender_007.png");
    proj->addImage("surrender_jei_2018/surrender_008.png");
    proj->addImage("surrender_jei_2018/surrender_009.png");
    proj->addImage("surrender_jei_2018/surrender_010.png");
    proj->addImage("surrender_jei_2018/surrender_011.png");
    proj->addImage("surrender_jei_2018/surrender_012.png");
    proj->addImage("surrender_jei_2018/surrender_013.png");
    proj->addImage("surrender_jei_2018/surrender_014.png");
    proj->addImage("surrender_jei_2018/surrender_015.png");
    proj->addImage("surrender_jei_2018/surrender_016.png");
    proj->addImage("surrender_jei_2018/surrender_017.png");
    proj->addImage("surrender_jei_2018/surrender_018.png");
    proj->addImage("surrender_jei_2018/surrender_019.png");
    
    // gui
    gui.setup();
    
    gui.add(showGui.setup("GUI (g)", true));
    
    modeNormal.addListener(this, &ofApp::setNormalMode);
    modeDepth.addListener(this, &ofApp::setDepthMode);
    modeOutputCalibration.addListener(this, &ofApp::setOutputCalibrationMode);
    modeInputCalibration.addListener(this, &ofApp::setInputCalibrationMode);
    
    gui.add(modeNormal.setup("Normal mode", true));
    gui.add(modeDepth.setup("Depth mode", false));
    gui.add(modeOutputCalibration.setup("Output calibration", false));
    gui.add(modeInputCalibration.setup("Input calibration", false));
    
    //set this param in gui!!!
    //gui.add(maxDepth.set)
    
    // load settings from files
    loadSettings();
}

void ofApp::setNormalMode(bool& toggled) {
    if (toggled) {
        modeDepth = false;
        modeOutputCalibration = false;
        modeInputCalibration = false;
    }
}

void ofApp::setDepthMode(bool& toggled) {
    if (toggled) {
        modeNormal = false;
        modeOutputCalibration = false;
        modeInputCalibration = false;
    }
}

void ofApp::setOutputCalibrationMode(bool& toggled) {
    if (toggled) {
        modeNormal = false;
        modeDepth = false;
        modeInputCalibration = false;
    }
}

void ofApp::setInputCalibrationMode(bool& toggled) {
    if (toggled) {
        modeNormal = false;
        modeDepth = false;
        modeOutputCalibration = false;
    }
}

//--------------------------------------------------------------
void ofApp::loadSettings() {
    // depth sensor settings
    ofxXmlSettings depthSettings;
    depthSettings.loadFile(DEPTH_SETTINGS_FILE);
    touch->setBorderPoint(0, depthSettings.getValue("x1", 100), depthSettings.getValue("y1", 100));
    touch->setBorderPoint(1, depthSettings.getValue("x2", 200), depthSettings.getValue("y2", 100));
    touch->setBorderPoint(2, depthSettings.getValue("x3", 200), depthSettings.getValue("y3", 500));
    touch->setBorderPoint(3, depthSettings.getValue("x4", 100), depthSettings.getValue("y4", 500));
    
    //touch->setMaxDepth(100); // set 10cm max depth
    
    // projection settings
    ofxXmlSettings projectionSettings;
    projectionSettings.loadFile(PROJ_SETTINGS_FILE);
    proj->setPosition(projectionSettings.getValue("x", 100), projectionSettings.getValue("y", 50));
    proj->setSize(projectionSettings.getValue("width", 200), projectionSettings.getValue("height", 500));
    
    // gui
    // gui.loadFromFile("settings.xml");
}

void ofApp::saveSettings() {
    // save projection settings
    ofxXmlSettings projectionSettings;
    
    projectionSettings.setValue("x", proj->getPosition()[0]);
    projectionSettings.setValue("y", proj->getPosition()[1]);
    projectionSettings.setValue("width", proj->getSize()[0]);
    projectionSettings.setValue("height", proj->getSize()[1]);
    
    projectionSettings.saveFile(PROJ_SETTINGS_FILE);
    
    // save depth settings
    
    ofxXmlSettings depthSettings;
    
    depthSettings.setValue("x1", touch->getBorderPoints()[0][0]);
    depthSettings.setValue("y1", touch->getBorderPoints()[0][1]);
    depthSettings.setValue("x2", touch->getBorderPoints()[1][0]);
    depthSettings.setValue("y2", touch->getBorderPoints()[1][1]);
    depthSettings.setValue("x3", touch->getBorderPoints()[2][0]);
    depthSettings.setValue("y3", touch->getBorderPoints()[2][1]);
    depthSettings.setValue("x4", touch->getBorderPoints()[3][0]);
    depthSettings.setValue("y4", touch->getBorderPoints()[3][1]);
    
    depthSettings.saveFile(DEPTH_SETTINGS_FILE);
    
    // GUI
    // gui.saveToFile("settings.xml");
}

//--------------------------------------------------------------
void ofApp::update(){
    touch->update();
    proj->update();
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofBackground(ofColor::black);
    
    ofSetColor(255);
    ofFill();
    if (modeNormal) {
        // normal mode
        // draw only result projection
        proj->draw();
    }
    else if (modeDepth) {
        // depth mode
        // show current state of touch area
        
        // USE ANOTHER PROJECTION INSTEAD OF THIS!
        ofFbo depth = touch->getDepth();
        depth.draw(proj->getPosition()[0], proj->getPosition()[1]);
        ofSetColor(0, 0, 255);
        // and a border
        ofNoFill();
        ofDrawRectangle(proj->getPosition()[0],
                        proj->getPosition()[1],
                        proj->getSize()[0],
                        proj->getSize()[1]);
    }
    else if (modeOutputCalibration) {
        // output calibration mode
        
        // draw red rectangle at the projection area
        ofSetColor(255, 0, 0);
        ofDrawRectangle(proj->getPosition()[0],
                        proj->getPosition()[1],
                        proj->getSize()[0],
                        proj->getSize()[1]);
        
        // draw grid on top
        ofSetColor(255);
        for (int x = 20; x < ofGetWidth(); x += 20)
            ofDrawLine(x, 0, x, ofGetHeight());
        for (int y = 20; y < ofGetHeight(); y += 20)
            ofDrawLine(0, y, ofGetWidth(), y);
    }
    else if (modeInputCalibration) {
        // input calibration mode
        
        // show rgb camera image
        ofImage cameraImage = touch->getCameraImage();
        cameraImage.draw(0, 0);
        
        // show touch input area border
        vector<ofVec2f> borderPoints = touch->getBorderPoints();
        ofSetColor(0, 255, 0);
        ofNoFill();
        ofPolyline border;
        for (int i = 0; i < 4; i++)
            border.addVertex(borderPoints[i]);
        border.close();
        border.draw();
        
        // label the corners
        ofDrawBitmapString("Top Left", borderPoints[0][0], borderPoints[0][1]);
        ofDrawBitmapString("Top Right", borderPoints[1][0], borderPoints[1][1]);
        ofDrawBitmapString("Bottom Right", borderPoints[2][0], borderPoints[2][1]);
        ofDrawBitmapString("Bottom Left", borderPoints[3][0], borderPoints[3][1]);
    }
    
    if (showGui)
        gui.draw();
}

//--------------------------------------------------------------
void ofApp::exit(){
    // save settings
    cout << "Save settingss\n";
    saveSettings();
    
    cout << "Remove proj class\n";
    delete proj;
    cout << "Remove touch class\n";
    delete touch;
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if (key == 'g')
        showGui = !showGui;
    
    if (modeInputCalibration && key == 'z') {
        touch->resetZeroPixels();
        //touch.setResultScreenSize(proj.getSize()[0], proj.getSize()[1]);
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    if (modeNormal || modeDepth) {
        touch->imitateTouch(x - proj->getPosition()[0],
                            y - proj->getPosition()[1]);
    }
    else if (modeOutputCalibration) {
        if (dragProjection) {
            ofVec2f currentPosition = proj->getPosition();
            proj->setPosition(currentPosition[0] + x - dragStart[0],
                             currentPosition[1] + y - dragStart[1]);
            dragStart.set(x, y);
        }
        
        if (resizeProjectionX) {
            ofVec2f currentSize = proj->getSize();
            proj->setWidth(currentSize[0] + x - dragStart[0]);
            dragStart.set(x, y);
        }
        
        if (resizeProjectionY) {
            ofVec2f currentSize = proj->getSize();
            proj->setHeight(currentSize[1] + y - dragStart[1]);
            dragStart.set(x, y);
        }
    }
    else if (modeInputCalibration) {
        if (dragTouchArea) {
            ofVec2f currentPosition = touch->getBorderPoints()[dragTouchAreaCornerNumber];
            touch->setBorderPoint(dragTouchAreaCornerNumber, currentPosition[0] + x - dragStart[0], currentPosition[1] + y - dragStart[1]);
            dragStart.set(x, y);
        }
    }
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    if (modeNormal || modeDepth) {
        touch->imitateTouch(x - proj->getPosition()[0],
                            y - proj->getPosition()[1]);
    }
    else if (modeOutputCalibration) {
        if (x >= proj->getPosition()[0] &&
            x <= proj->getPosition()[0] + proj->getSize()[0] &&
            abs(y - proj->getPosition()[1] - proj->getSize()[1]) < SENS_RANGE) {
            resizeProjectionY = true;
            dragStart.set(x, y);
        }
        else if (y >= proj->getPosition()[1] &&
                 y <= proj->getPosition()[1] + proj->getSize()[1] &&
                 abs(x - proj->getPosition()[0] - proj->getSize()[0]) < SENS_RANGE) {
            resizeProjectionX = true;
            dragStart.set(x, y);
        }
        else if (x >= proj->getPosition()[0] &&
                 x <= proj->getPosition()[0] + proj->getSize()[0] &&
                 y >= proj->getPosition()[1] &&
                 y <= proj->getPosition()[1] + proj->getSize()[1]) {
            dragProjection = true;
            dragStart.set(x, y);
        }
    }
    else if (modeInputCalibration) {
        vector<ofVec2f> points = touch->getBorderPoints();
        for (int i = 0; i < points.size(); i++) {
            if (points[i].distance(ofVec2f(x, y)) < SENS_RANGE) {
                dragTouchArea = true;
                dragTouchAreaCornerNumber = i;
                dragStart.set(x, y);
                break;
            }
        }
    }
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    if (modeNormal || modeDepth)
        touch->imitateRelease();
    else {
        if (dragTouchArea) dragTouchArea = false;
        if (dragProjection) dragProjection = false;
        if (resizeProjectionY) resizeProjectionY = false;
        if (resizeProjectionX) resizeProjectionX = false;
    }
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
