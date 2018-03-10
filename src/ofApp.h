#pragma once

#include "ofMain.h"
#include "projection.h"
#include "touchArea.h"
#include "ofxGui.h"
#include "ofxXmlSettings.h"

class ofApp : public ofBaseApp{
    projection proj;
    touchArea touch;
    
    ofxXmlSettings projectionSettings;
    ofxXmlSettings depthSettings;
    
    ofxPanel gui;
    
    ofxToggle showGui;
    ofxToggle showGrid;
    ofxToggle showDepthView;
    ofxToggle calibraionMode;
    ofParameter<int> maxDepth;
    
    const int SENS_RANGE = 5;
    const int BORDER_MARGIN = 5;
    
    bool dragProjection = false;
    bool resizeProjectionX = false;
    bool resizeProjectionY = false;
    bool dragTouchArea = false;
    int dragTouchAreaCornerNumber = 0;
    
    ofVec2f dragStart;
    
    void calibrationToggleListener(bool &toggleStatus);

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
