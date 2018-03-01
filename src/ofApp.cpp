#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    // projection settings
    
    projectionSettings.loadFile("projection_settings.xml");
    
    // depth sensor settings
    
    depthSettings.loadFile("depth_settings.xml");
    
    // setup from config
    
    proj.setTouchArea(&touch);
    
    proj.setPosition(projectionSettings.getValue("x", 100), projectionSettings.getValue("y", 50));
    proj.setSize(projectionSettings.getValue("width", 200), projectionSettings.getValue("height", 500));
    
    touch.setBorderPoint(0, depthSettings.getValue("x1", 100), depthSettings.getValue("y1", 100));
    touch.setBorderPoint(1, depthSettings.getValue("x2", 200), depthSettings.getValue("y2", 100));
    touch.setBorderPoint(2, depthSettings.getValue("x3", 200), depthSettings.getValue("y3", 500));
    touch.setBorderPoint(3, depthSettings.getValue("x4", 100), depthSettings.getValue("y4", 500));
    
    proj.updateMapping();
    
    // gui
    gui.setup();
    
    gui.add(showGui.setup("GUI (g)", true));
    gui.add(showGrid.setup("Grid", false));
    gui.add(showDepthView.setup("Depth View", false));
    gui.add(calibraionMode.setup("Calibration (r - recognize)", false));
    
    gui.loadFromFile("settings.xml");
    
    calibraionMode.addListener(this, &ofApp::calibrationToggleListener);
}

//--------------------------------------------------------------
void ofApp::update(){
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
                touch.draw(10, (ofGetHeight() - touch.HEIGHT) / 2);
                touch.drawBorder(10, (ofGetHeight() - touch.HEIGHT) / 2);
            }
        }
        else {
            proj.drawBorder();
            touch.drawBorder(10, (ofGetHeight() - touch.HEIGHT) / 2);
        }
    }
    
    if (showGui)
        gui.draw();
}

//--------------------------------------------------------------
void ofApp::exit(){
    calibraionMode.removeListener(this, &ofApp::calibrationToggleListener);
    
    gui.saveToFile("settings.xml");
    
    projectionSettings.setValue("x", proj.getPosition()[0]);
    projectionSettings.setValue("y", proj.getPosition()[1]);
    projectionSettings.setValue("width", proj.getSize()[0]);
    projectionSettings.setValue("height", proj.getSize()[1]);
    projectionSettings.saveFile("projection_settings.xml");
    
    depthSettings.setValue("x1", touch.getBorderPoints()[0][0]);
    depthSettings.setValue("y1", touch.getBorderPoints()[0][1]);
    depthSettings.setValue("x2", touch.getBorderPoints()[1][0]);
    depthSettings.setValue("y2", touch.getBorderPoints()[1][1]);
    depthSettings.setValue("x3", touch.getBorderPoints()[2][0]);
    depthSettings.setValue("y3", touch.getBorderPoints()[2][1]);
    depthSettings.setValue("x4", touch.getBorderPoints()[3][0]);
    depthSettings.setValue("y4", touch.getBorderPoints()[3][1]);
    
    depthSettings.saveFile("depth_settings.xml");
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if (key == 'g')
        showGui = !showGui;
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
        touch.imitateTouch(x - 10, y - (ofGetHeight() - touch.HEIGHT) / 2);
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
    }
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    if (!calibraionMode)
        touch.imitateTouch(x - 10, y - (ofGetHeight() - touch.HEIGHT) / 2));
    else {
        if (showDepthView) {
            int xOffset = 10;
            int yOffset = (ofGetHeight() - touch.HEIGHT) / 2;
            
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
                y >= proj.getPosition()[1] && y <= proj.getPosition()[1] + proj.getSize()[1]) {
                dragProjection = true;
                dragStart.set(x, y);
            }
            
            // resize!
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

void ofApp::calibrationToggleListener(bool & pressed) {
    if (!pressed)
        proj.updateMapping();
}
