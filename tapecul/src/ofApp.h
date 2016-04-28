#pragma once

#include "ofMain.h"

#include "ofxParallax.h"
#include "ofxOsc.h"
#include "ofxEasing.h"
#include "ofxHapPlayer.h"

#define PORT 8001


class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

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
		
    bool isTravellingRight;
    
    ofPoint touchPoint;
    ofPoint releasePoint;
    bool isTouched;
    
    float currentImpulse;
    float targetX;
    float receivedX, prevReceivedX;
    std::list<float> prevX;
    float avgX;
    
    float currentImpulseEased;
    float worldSpeed;
    
    ofxParallax parallax;
    
    int blurID;
    
    int minX, maxX;
    
    ofxOscReceiver receiver;
    
    int current_msg_string;
    
    ofxHapPlayer player;

};
