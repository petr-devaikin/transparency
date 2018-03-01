#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    // gui
    gui.setup();
    
    gui.add(showGui.setup("GUI", true));
    gui.add(showGrid.setup("Grid", false));
    gui.add(showDepthView.setup("Depth View", false));
    gui.add(calibraionMode.setup("Calibration", false));
    
    gui.loadFromFile("settings.xml");
    
    // projection settings
    
    projectionSettings.loadFile("projection_settings.xml");
    
    // depth sensor settings
    
    depthSettings.loadFile("depth_settings.xml");
    
    // move it. it's here only for tests
    touch.recognizeBorders();
    
    proj.setTouchArea(&touch);
    
    proj.setPosition(projectionSettings.getValue("x", 100), projectionSettings.getValue("y", 50));
    proj.setSize(projectionSettings.getValue("width", 200), projectionSettings.getValue("height", 500));
    proj.updateMapping();
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
        touch.draw();
        proj.draw();
    }
    
    if (showGui)
        gui.draw();
}

//--------------------------------------------------------------
void ofApp::exit(){
    gui.saveToFile("settings.xml");
    
    projectionSettings.saveFile("projection_settings.xml");
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
    touch.imitateTouch(x, y);
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    touch.imitateTouch(x, y);
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    touch.imitateRelease();
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
