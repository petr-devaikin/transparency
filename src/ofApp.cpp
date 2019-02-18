#include "ofApp.h"


//--------------------------------------------------------------
void ofApp::setup(){
    // init camera
    camera = new cameraManager();
    camera->findCamera();
    
    // init calibrator
    calib = new calibrator(camera);
    
    // init touch
    touch = new touchArea(camera, .3, ofVec2f(1024, 1024)); // 30 cm max dept. Result image 1024x1024
    
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
    
    // load projection polyline
    ofPolyline projectionPolyline;
    projectionPolyline.addVertex(projectionSettings.getValue("projection_x1", 100), projectionSettings.getValue("projection_y1", 100));
    projectionPolyline.addVertex(projectionSettings.getValue("projection_x2", 400), projectionSettings.getValue("projection_y2", 100));
    projectionPolyline.addVertex(projectionSettings.getValue("projection_x3", 400), projectionSettings.getValue("projection_y3", 400));
    projectionPolyline.addVertex(projectionSettings.getValue("projection_x4", 100), projectionSettings.getValue("projection_y4", 400));
    
    // load camera polyline
    ofPolyline cameraPolyline;
    cameraPolyline.addVertex(projectionSettings.getValue("camera_x1", 100), projectionSettings.getValue("camera_y1", 100));
    cameraPolyline.addVertex(projectionSettings.getValue("camera_x2", 400), projectionSettings.getValue("camera_y2", 100));
    cameraPolyline.addVertex(projectionSettings.getValue("camera_x3", 400), projectionSettings.getValue("camera_y3", 400));
    cameraPolyline.addVertex(projectionSettings.getValue("camera_x4", 100), projectionSettings.getValue("camera_y4", 400));
    
    projectionPolyline.close();
    cameraPolyline.close();
    
    calib->setProjectionArea(projectionPolyline);
    calib->setCameraArea(cameraPolyline);
    
    // gui
    // gui.loadFromFile("settings.xml");
}

void ofApp::saveSettings() {
    // save projection settings
    ofxXmlSettings projectionSettings;
    
    // save projection polyline
    ofPolyline projectionPolyline = calib->getProjectionPolyline();
    projectionSettings.setValue("projection_x1", projectionPolyline.getVertices()[0][0]);
    projectionSettings.setValue("projection_y1", projectionPolyline.getVertices()[0][1]);
    projectionSettings.setValue("projection_x2", projectionPolyline.getVertices()[1][0]);
    projectionSettings.setValue("projection_y2", projectionPolyline.getVertices()[1][1]);
    projectionSettings.setValue("projection_x3", projectionPolyline.getVertices()[2][0]);
    projectionSettings.setValue("projection_y3", projectionPolyline.getVertices()[2][1]);
    projectionSettings.setValue("projection_x4", projectionPolyline.getVertices()[3][0]);
    projectionSettings.setValue("projection_y4", projectionPolyline.getVertices()[3][1]);
    
    // save camera polyline
    ofPolyline cameraPolyline = calib->getCameraPolyline();
    projectionSettings.setValue("camera_x1", cameraPolyline.getVertices()[0][0]);
    projectionSettings.setValue("camera_y1", cameraPolyline.getVertices()[0][1]);
    projectionSettings.setValue("camera_x2", cameraPolyline.getVertices()[1][0]);
    projectionSettings.setValue("camera_y2", cameraPolyline.getVertices()[1][1]);
    projectionSettings.setValue("camera_x3", cameraPolyline.getVertices()[2][0]);
    projectionSettings.setValue("camera_y3", cameraPolyline.getVertices()[2][1]);
    projectionSettings.setValue("camera_x4", cameraPolyline.getVertices()[3][0]);
    projectionSettings.setValue("camera_y4", cameraPolyline.getVertices()[3][1]);
    
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
        //(touch->getTransformedTouch()).draw(0, 0);
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
        // if projection polygon is set, go to recognition state
        if (calib->getState() == projectionSetup)
            calib->recognize();
        
        // if camera polygon is set ore recognized correctly, confirm it
        else if (calib->getState() == showingRecognizedArea) {
            calib->confirmRecognizedArea();
            touch->setSensitiveArea(calib->getCameraPolyline());
            proj->setOutputPolyline(calib->getProjectionPolyline());
            touch->start();
            proj->start();
        }
    }
    else if (key == 'r') {
        touch->stop();
        proj->stop();
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
        if (dragPolygonCorner) {
            ofPolyline projectionPolyline = calib->getProjectionPolyline();
            projectionPolyline.getVertices()[dragCornerNumber].set(x, y);
            calib->setProjectionArea(projectionPolyline);
        }
    }
    else if (calib->getState() == showingRecognizedArea) {
        // manually setup camera area
        if (dragPolygonCorner) {
            ofPolyline cameraPolyline = calib->getCameraPolyline();
            cameraPolyline.getVertices()[dragCornerNumber].set(x, y);
            calib->setCameraArea(cameraPolyline);
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
                dragPolygonCorner = true;
                dragCornerNumber = i;
                break;
            }
        }
    }
    else if (calib->getState() == showingRecognizedArea) {
        // manually setup camera sensitive area
        // check if area around on of the corners of camera polygon was touched
        ofPolyline cameraPolygon = calib->getCameraPolyline();
        for (int i = 0; i < cameraPolygon.size(); i++) {
            if (cameraPolygon.getVertices()[i].distance(ofVec2f(x, y)) < SENS_RANGE) {
                dragPolygonCorner = true;
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
    if (calib->getState() == projectionSetup || calib->getState() == showingRecognizedArea) {
        if (dragPolygonCorner) dragPolygonCorner = false;
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
