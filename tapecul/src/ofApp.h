#pragma once

#include "ofMain.h"

#include "ofxParallax.h"
#include "ofxOsc.h"
#include "ofxEasing.h"
#include "ofEvents.h"

#if defined(TARGET_WIN32)
#include "ofxDSHapVideoPlayer.h"
#define ofxHapPlayer ofxDSHapVideoPlayer
#else
#include "ofxHapPlayer.h"
#endif


#if defined(TARGET_WIN32)
#else
#endif

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
		
    void setupArduino(const int & version);
//    void digitalPinChanged(const int & pinNum);
    void analogPinChanged(const int & pinNum);
    void updateArduino();

    void setupSecondWindow(ofEventArgs & args);
    void drawSecondWindow(ofEventArgs & args);
    
    ofArduino	ard;
    bool		bSetupArduino;			// flag variable for setting up arduino once
    
    float accelX, forcePressureLeft;
    
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
    

    ofxHapPlayer lune, lion, nuage3, nuage4, nuage5;


};
