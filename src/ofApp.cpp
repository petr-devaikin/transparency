#include "ofApp.h"


//--------------------------------------------------------------
void ofApp::setup(){
    // init camera
    camera = new cameraManager();
    camera->findCamera();
    
    // init calibrator
    calib = new calibrator(camera);
    
    // init touch
    touch = new touchArea(camera, .3, ofVec2f(1014, 1024)); // 30 cm max dept. Result image 1024x1024
    
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
    /*
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
    
     */
    
    
    // load settings from files
    loadSettings();
    
    // start immediately for now
    //touch->start();
    //proj->start();
}

//--------------------------------------------------------------
void ofApp::loadSettings() {
    // depth sensor settings
    //ofxXmlSettings depthSettings;
    //depthSettings.loadFile(DEPTH_SETTINGS_FILE);
    
    
    
    //touch->setMaxDepth(100); // set 10cm max depth
    
    // projection settings
    ofxXmlSettings projectionSettings;
    projectionSettings.loadFile(PROJ_SETTINGS_FILE);
    
    ofPolyline projectionPolyline;
    projectionPolyline.addVertex(projectionSettings.getValue("x1", 100), projectionSettings.getValue("y1", 100));
    projectionPolyline.addVertex(projectionSettings.getValue("x2", 400), projectionSettings.getValue("y2", 100));
    projectionPolyline.addVertex(projectionSettings.getValue("x3", 400), projectionSettings.getValue("y3", 400));
    projectionPolyline.addVertex(projectionSettings.getValue("x4", 100), projectionSettings.getValue("y4", 400));
    
    calib->presetProjectionArea(projectionPolyline);
    
    // gui
    // gui.loadFromFile("settings.xml");
}

void ofApp::saveSettings() {
    // save projection settings
    ofxXmlSettings projectionSettings;
    
    ofPolyline projectionPolyline = calib->getProjectionPolyline();
    projectionSettings.setValue("x1", projectionPolyline.getVertices()[0][0]);
    projectionSettings.setValue("y1", projectionPolyline.getVertices()[0][1]);
    projectionSettings.setValue("x2", projectionPolyline.getVertices()[1][0]);
    projectionSettings.setValue("y2", projectionPolyline.getVertices()[1][1]);
    projectionSettings.setValue("x3", projectionPolyline.getVertices()[2][0]);
    projectionSettings.setValue("y3", projectionPolyline.getVertices()[2][1]);
    projectionSettings.setValue("x4", projectionPolyline.getVertices()[3][0]);
    projectionSettings.setValue("y4", projectionPolyline.getVertices()[3][1]);
    
    projectionSettings.saveFile(PROJ_SETTINGS_FILE);
    
    // save depth settings
    
    //ofxXmlSettings depthSettings;
    //depthSettings.saveFile(DEPTH_SETTINGS_FILE);
    
    // GUI
    // gui.saveToFile("settings.xml");
}

//--------------------------------------------------------------
void ofApp::update(){
    camera->update();
    
    calib->update();
    
    if (calib->getState() == done) {
        touch->update();
        proj->update();
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofBackground(ofColor::black);
    
    ofSetColor(255);
    ofFill();
    
    calib->draw(); // draw calibration process. If done, it draws nothing
    
    if (calib->getState() == done) {
        proj->draw();
    }
    
    //if (showGui)
    //    gui.draw();
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
    cout << "Remove calibrator\n";
    delete calib;
    cout << "Stop camera\n";
    camera->disconnectCamera();
    delete camera;
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if (key == ' ') {
        // TBD go to next calibration step
    }
    else if (key == 'q') {
        calib->startAgain();
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
    if (calib->getState() == projectionSetup) {
        // manually setup projection area
        if (dragProjectionCorner) {
            ofPolyline projectionPolyline = calib->getProjectionPolyline();
            projectionPolyline.getVertices()[dragCornerNumber].set(x, y);
            
            calib->presetProjectionArea(projectionPolyline);
        }
    }
    else if (calib->getState() == done) {
        // imitate touch
        if (!camera->isCameraFound())
            touch->imitateTouch(x, y); // starting from top left corner
    }
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    if (calib->getState() == projectionSetup) {
        // manually setup projection area
        // check if area around on of the corners of projection polygon was touched
        ofPolyline projectionPolygon = calib->getProjectionPolyline();
        for (int i = 0; i < projectionPolygon.size(); i++) {
            if (projectionPolygon.getVertices()[i].distance(ofVec2f(x, y)) < SENS_RANGE) {
                dragProjectionCorner = true;
                dragCornerNumber = i;
                break;
            }
        }
    }
    else if (calib->getState() == done) {
        // imitate touch
        if (!camera->isCameraFound())
            touch->imitateTouch(x, y);
    }
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    if (calib->getState() == projectionSetup) {
        if (dragProjectionCorner) dragProjectionCorner = false;
    }
    else if (calib->getState() == done)
        touch->imitateRelease();
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
