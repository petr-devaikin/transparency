#include "ofApp.h"


//--------------------------------------------------------------
void ofApp::setup(){
    // init projection
    proj.init("img0.jpg", "img1.jpg");
    proj.setTouchArea(&touch);
    
    // gui
    gui.setup();
    
    gui.add(showGui.setup("GUI (g)", true));
    gui.add(showGrid.setup("Grid", false)); // show grid to make lines parallel
    gui.add(calibraionMode.setup("Calibration", false)); // change the borders
    gui.add(showDepthView.setup("Depth View", false)); // show depth camera view
    //set this param in gui!!!
    //gui.add(maxDepth.set)
    
    // load settings from files
    loadSettings();
    
    touch.setResultScreenSize(proj.getSize()[0], proj.getSize()[1]);
}

//--------------------------------------------------------------
void ofApp::loadSettings() {
    // depth sensor settings
    ofxXmlSettings depthSettings;
    depthSettings.loadFile(DEPTH_SETTINGS_FILE);
    touch.setBorderPoint(0, depthSettings.getValue("x1", 100), depthSettings.getValue("y1", 100));
    touch.setBorderPoint(1, depthSettings.getValue("x2", 200), depthSettings.getValue("y2", 100));
    touch.setBorderPoint(2, depthSettings.getValue("x3", 200), depthSettings.getValue("y3", 500));
    touch.setBorderPoint(3, depthSettings.getValue("x4", 100), depthSettings.getValue("y4", 500));
    
    touch.setMaxDepth(100); // set 5cm max depth
    
    // projection settings
    ofxXmlSettings projectionSettings;
    projectionSettings.loadFile(PROJ_SETTINGS_FILE);
    proj.setPosition(projectionSettings.getValue("x", 100), projectionSettings.getValue("y", 50));
    proj.setSize(projectionSettings.getValue("width", 200), projectionSettings.getValue("height", 500));
    proj.setDepthLevel(projectionSettings.getValue("maxDepth", 255));
    
    // gui
    gui.loadFromFile("settings.xml");
}

void ofApp::saveSettings() {
    // save projection settings
    ofxXmlSettings projectionSettings;
    
    projectionSettings.setValue("x", proj.getPosition()[0]);
    projectionSettings.setValue("y", proj.getPosition()[1]);
    projectionSettings.setValue("width", proj.getSize()[0]);
    projectionSettings.setValue("height", proj.getSize()[1]);
    projectionSettings.setValue("maxDepth", proj.getDepthLevel());
    
    projectionSettings.saveFile(PROJ_SETTINGS_FILE);
    
    // save depth settings
    
    ofxXmlSettings depthSettings;
    
    depthSettings.setValue("x1", touch.getBorderPoints()[0][0]);
    depthSettings.setValue("y1", touch.getBorderPoints()[0][1]);
    depthSettings.setValue("x2", touch.getBorderPoints()[1][0]);
    depthSettings.setValue("y2", touch.getBorderPoints()[1][1]);
    depthSettings.setValue("x3", touch.getBorderPoints()[2][0]);
    depthSettings.setValue("y3", touch.getBorderPoints()[2][1]);
    depthSettings.setValue("x4", touch.getBorderPoints()[3][0]);
    depthSettings.setValue("y4", touch.getBorderPoints()[3][1]);
    
    depthSettings.saveFile(DEPTH_SETTINGS_FILE);
    
    // GUI
    gui.saveToFile("settings.xml");
}

//--------------------------------------------------------------
void ofApp::update(){
    touch.update();
    proj.update();
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofBackground(ofColor::black);
    
    if (showGrid)
    {
        ofSetColor(255);
        int x = 20;
        int y = 20;
        while (x < ofGetWidth())
        {
            ofDrawLine(x, 0, x, ofGetHeight());
            x += 20;
        }
        while (y < ofGetHeight())
        {
            ofDrawLine(0, y, ofGetWidth(), y);
            y += 20;
        }
    }
    else
    {
        if (!calibraionMode) {
            proj.draw();
            proj.drawBorder();
            
            if (showDepthView) {
                touch.drawDepth(10, proj.getPosition()[1]);
            }
        }
        else {
            if (showDepthView) {
                touch.drawImage(10,  (ofGetHeight() - touch.getHeight()) / 2);
                touch.drawBorder(10, (ofGetHeight() - touch.getHeight()) / 2);
            }
            else {
                proj.draw();
                proj.drawBorder();
            }
        }
    }
    
    if (showGui)
        gui.draw();
}

//--------------------------------------------------------------
void ofApp::exit(){
    // save settings
    saveSettings();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if (key == 'g')
        showGui = !showGui;
    
    if (key == 't')
        showDepthView = !showDepthView;
    
    if (calibraionMode && key == 'z') {
        touch.updateZeroPixels();
        touch.setResultScreenSize(proj.getSize()[0], proj.getSize()[1]);
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
    if (!calibraionMode)
        touch.imitateTouch(x - 10, y - (ofGetHeight() - touch.getHeight()) / 2);
    else {
        if (dragTouchArea) {
            ofVec2f currentPosition = touch.getBorderPoints()[dragTouchAreaCornerNumber];
            touch.setBorderPoint(dragTouchAreaCornerNumber, currentPosition[0] + x - dragStart[0],
                             currentPosition[1] + y - dragStart[1]);
            dragStart.set(x, y);
        }
        
        if (dragProjection) {
            ofVec2f currentPosition = proj.getPosition();
            proj.setPosition(currentPosition[0] + x - dragStart[0],
                             currentPosition[1] + y - dragStart[1]);
            dragStart.set(x, y);
        }
        
        if (resizeProjectionX) {
            ofVec2f currentSize = proj.getSize();
            proj.setWidth(currentSize[0] + x - dragStart[0]);
            dragStart.set(x, y);
        }
        
        if (resizeProjectionY) {
            ofVec2f currentSize = proj.getSize();
            proj.setHeight(currentSize[1] + y - dragStart[1]);
            dragStart.set(x, y);
        }
    }
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    if (!calibraionMode)
        touch.imitateTouch(x - 10, y - (ofGetHeight() - touch.getHeight()) / 2);
    else {
        if (showDepthView) {
            int xOffset = 10;
            int yOffset = (ofGetHeight() - touch.getHeight()) / 2;
            
            vector<ofVec2f> points = touch.getBorderPoints();
            for (int i = 0; i < points.size(); i++) {
                if (points[i].distance(ofVec2f(x - xOffset, y - yOffset)) < SENS_RANGE) {
                    dragTouchArea = true;
                    dragTouchAreaCornerNumber = i;
                    dragStart.set(x, y);
                    break;
                }
            }
        }
        
        // drag projection area only if the touch area is not dragged
        if (!dragTouchArea) {
            if (x >= proj.getPosition()[0] && x <= proj.getPosition()[0] + proj.getSize()[0] &&
                abs(y - proj.getPosition()[1] - proj.getSize()[1] - BORDER_MARGIN) < SENS_RANGE) {
                resizeProjectionY = true;
                dragStart.set(x, y);
            }
            else if (y >= proj.getPosition()[1] && y <= proj.getPosition()[1] + proj.getSize()[1] + BORDER_MARGIN &&
                    abs(x - proj.getPosition()[0] - proj.getSize()[0]) < SENS_RANGE) {
                resizeProjectionX = true;
                dragStart.set(x, y);
            }
            else if (x >= proj.getPosition()[0] && x <= proj.getPosition()[0] + proj.getSize()[0] &&
                y >= proj.getPosition()[1] && y <= proj.getPosition()[1] + proj.getSize()[1]) {
                dragProjection = true;
                dragStart.set(x, y);
            }
        }
    }
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    if (!calibraionMode)
        touch.imitateRelease();
    else {
        if (dragTouchArea)
            dragTouchArea = false;
        if (dragProjection)
            dragProjection = false;
        if (resizeProjectionY)
            resizeProjectionY = false;
        if (resizeProjectionX)
            resizeProjectionX = false;
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
