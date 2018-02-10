#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    gui.setup();
    
    gui.add(projectionX.set("Projection x", 100, 0, 1000));
    gui.add(projectionY.set("Projection y", 10, 1, 1000));
    gui.add(projectionWidth.set("Projection width", 200, 0, 1000));
    gui.add(projectionHeight.set("Projection height", 300, 0, 1000));
    
    gui.loadFromFile("settings.xml");
}

//--------------------------------------------------------------
void ofApp::update(){
    proj.setX(projectionX);
    proj.setY(projectionY);
    proj.setWidth(projectionWidth);
    proj.setHeight(projectionHeight);
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofBackground(ofColor::black);
    proj.drawBorder();
    touch.drawBorder();
    
    gui.draw();
}

//--------------------------------------------------------------
void ofApp::exit(){
    gui.saveToFile("settings.xml");
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

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
