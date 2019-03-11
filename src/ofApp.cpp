#include "ofApp.h"


//--------------------------------------------------------------
void ofApp::setup() {
    // init camera
    camera = new cameraManager(CAMERA_MAX_DEPTH, CAMERA_WIDTH, CAMERA_HEIGHT);
    camera->findCamera();
    
    // init calibrator
    calib = new calibrator(camera, IMAGE_WIDTH, IMAGE_HEIGHT);
    
    // load settings from files
    loadSettings();
}

//--------------------------------------------------------------
void ofApp::loadSettings() {
    // depth sensor settings
    //ofxXmlSettings depthSettings;
    //depthSettings.loadFile(DEPTH_SETTINGS_FILE);
    
    //touch->setMaxDepth(100); // set 10cm max depth
    
    // projection settings
    ofxXmlSettings projectionSettings;
    projectionSettings.loadFile(ofFilePath::join(BASE_PATH, PROJ_SETTINGS_FILE));
    
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
    
    projectionSettings.saveFile(ofFilePath::join(BASE_PATH, PROJ_SETTINGS_FILE));
    
    // save depth settings
    
    //ofxXmlSettings depthSettings;
    //depthSettings.saveFile(DEPTH_SETTINGS_FILE);
    
}

void ofApp::initProjection() {
    // init proj
    proj = new projectionInvertedBrush(BASE_PATH, touch, calib);
    proj->addImage(ofFilePath::join(BASE_PATH, "surrender_jei_2018/surrender_001.png"));
    proj->addImage(ofFilePath::join(BASE_PATH, "surrender_jei_2018/surrender_002.png"));
    proj->addImage(ofFilePath::join(BASE_PATH, "surrender_jei_2018/surrender_003.png"));
    proj->addImage(ofFilePath::join(BASE_PATH, "surrender_jei_2018/surrender_004.png"));
    proj->addImage(ofFilePath::join(BASE_PATH, "surrender_jei_2018/surrender_005.png"));
    proj->addImage(ofFilePath::join(BASE_PATH, "surrender_jei_2018/surrender_006.png"));
    proj->addImage(ofFilePath::join(BASE_PATH, "surrender_jei_2018/surrender_007.png"));
    proj->addImage(ofFilePath::join(BASE_PATH, "surrender_jei_2018/surrender_008.png"));
    proj->addImage(ofFilePath::join(BASE_PATH, "surrender_jei_2018/surrender_009.png"));
    proj->addImage(ofFilePath::join(BASE_PATH, "surrender_jei_2018/surrender_010.png"));
    proj->addImage(ofFilePath::join(BASE_PATH, "surrender_jei_2018/surrender_011.png"));
    proj->addImage(ofFilePath::join(BASE_PATH, "surrender_jei_2018/surrender_012.png"));
    proj->addImage(ofFilePath::join(BASE_PATH, "surrender_jei_2018/surrender_013.png"));
    proj->addImage(ofFilePath::join(BASE_PATH, "surrender_jei_2018/surrender_014.png"));
    proj->addImage(ofFilePath::join(BASE_PATH, "surrender_jei_2018/surrender_015.png"));
    proj->addImage(ofFilePath::join(BASE_PATH, "surrender_jei_2018/surrender_016.png"));
    proj->addImage(ofFilePath::join(BASE_PATH, "surrender_jei_2018/surrender_017.png"));
    proj->addImage(ofFilePath::join(BASE_PATH, "surrender_jei_2018/surrender_018.png"));
    proj->addImage(ofFilePath::join(BASE_PATH, "surrender_jei_2018/surrender_019.png"));
}

void ofApp::initTouchArea() {
    // init touch
    touch = new touchArea(camera, TOUCH_THRESHOLD);
}

//--------------------------------------------------------------
void ofApp::update(){
    camera->update();
    calib->update();
    
    if (calib->getState() == done) {
        proj->update();
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofBackground(ofColor::black);
    
    ofSetColor(255);
    ofFill();
    
    
    if (calib->getState() == done) {
        proj->draw();
    }
    else {
        calib->draw();
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
            
            initTouchArea();
            initProjection();
            
            proj->start();
        }
    }
    else if (key == 'r') {
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
