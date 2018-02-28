#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    gui.setup();
    
    gui.add(projectionX.set("Projection x", 100, 0, 1000));
    gui.add(projectionY.set("Projection y", 10, 1, 1000));
    gui.add(projectionWidth.set("Projection width", 200, 0, 1000));
    gui.add(projectionHeight.set("Projection height", 300, 0, 1000));
    
    gui.loadFromFile("settings.xml");
    
    // move it. it's here only for tests
    touch.recognizeBorders();
    
    proj.setTouchArea(&touch);
}

//--------------------------------------------------------------
void ofApp::update(){
    proj.setX(projectionX);
    proj.setY(projectionY);
    proj.setSize(projectionWidth, projectionHeight);
    
    proj.updateMapping(); // do it only of something changed!
    
    proj.update();
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofBackground(ofColor::black);
    
    touch.draw();
    proj.draw();
    
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
