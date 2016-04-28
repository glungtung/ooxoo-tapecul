#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    // Load a movie file
    player.load("movies/ptitbonhomme.mov");
    // Start playback
    player.setLoopState(OF_LOOP_NORMAL);
    player.play();
    
    
    blurID = 0;
    
    worldSpeed = 60;
    
    currentImpulse = 0;
    currentImpulseEased = 0;
    
    isTravellingRight = true;
    
    parallax.setup();
    //    if(i==0) {
    //        theLayer->speed = speed; //5 * 360;
    //        //theLayer->isBlurred = true;
    //    } else if(i==1){
    //        theLayer->speed = 5 * 130;
    //        theLayer->isBlurred = true;
    //    } else if(i==2) {
    //        theLayer->speed = 5 * 80;
    //    }
    //    parallax.setupLayers(3);
    
 //   parallax.addNewLayer(0, ofVec2f(-00, 0), ofVec2f(1566, 768), 5 * 360, ofVec2f(-1566, +500));
 //   parallax.addNewLayer(1, ofVec2f(0, 0), ofVec2f(1236, 768), 5 * 130, ofVec2f(-(1236/2), +(1236/2)));
 //   parallax.addNewLayer(2, ofVec2f(0, 0), ofVec2f(1236, 768), 5 * 80, ofVec2f(-(1236/2), +(1236/2)));
    //parallax.addNewLayer(0, ofVec2f(0, 0), ofVec2f(1536, 768), 5 * 360);
    //parallax.addNewLayer(1, ofVec2f(0, 0), ofVec2f(1236, 768), 5 * 130);
    //parallax.addNewLayer(2, ofVec2f(0, 0), ofVec2f(1236, 768), 5 * 80);
    parallax.addNewLayer(0, ofVec2f(0, 0), ofVec2f(2536, 768), 0.2, ofVec2f(-9000, 9000));
    parallax.addNewLayer(1, ofVec2f(0, 0), ofVec2f(1236, 768), 1, ofVec2f(-9000, 9000));
    parallax.addNewLayer(2, ofVec2f(0, 0), ofVec2f(800, 800), -2, ofVec2f(-9000, 9000));
    parallax.unblurAll();
    
    
    parallax.addImageToLayer(0, "images/3-background.jpg", ofPoint(0, 0));
    parallax.addImageToLayer(1, "images/2-middle.png", ofPoint(0, 0));
//    parallax.addImageToLayer(2, "images/1-foreground.png", ofPoint(1000, 0));
    parallax.addVideoToLayer(2, &player, ofPoint(0, 0));
    
    isTouched = true;

    //OSC
    receiver.setup(PORT);
    receivedX = 0.;
    prevReceivedX = 0.;
    


}

//--------------------------------------------------------------
void ofApp::update(){

    ofSetWindowTitle(ofToString(ofGetFrameRate()));
    
    player.update();

    
    // check for waiting messages
    while(receiver.hasWaitingMessages()){
        // get the next message
        ofxOscMessage m;
        receiver.getNextMessage(m);

        // check for mouse moved message
        if(m.getAddress() == "/wii/5/accel/pry/0"){
            // both the arguments are int32's
            receivedX = m.getArgAsFloat(0);
            prevX.push_front(receivedX);
            if (prevX.size() > 2)
                prevX.pop_back();
        }
    }
    
    avgX = 0;
    for (auto x : prevX)
        avgX += x;
    if (prevX.size() > 0)
        avgX = avgX / prevX.size();
    
    
   // if (abs(receivedX - prevReceivedX) > 0.01) {
    receivedX = prevReceivedX + (avgX - prevReceivedX) / 10;
    cout << receivedX << endl;

    prevReceivedX = receivedX;

    targetX = ofxeasing::map(receivedX, 0.3, 0.7, 0., 1., ofxeasing::linear::easeIn);
 //   }
    
    float diff = 0;
    if(isTouched == true) {
        diff = (currentImpulse - currentImpulseEased) * 0.4;
    } else {
        diff = (0.0 - currentImpulseEased) * 0.1;
    }
    currentImpulseEased += diff;
    //cout << currentImpulseEased << endl;
    
    parallax.updateTarget(targetX);
    
    if(currentImpulseEased != 0.0f) {
        //parallax.update(currentImpulseEased);
    }

}

//--------------------------------------------------------------
void ofApp::draw(){
    parallax.draw();

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
 }

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

    touchPoint = releasePoint;
    releasePoint.set(x, y);
    ofVec2f vel = releasePoint - touchPoint;
    
    //cout << vel.x << endl;
    
  //  currentImpulse = vel.x * 5;
    
 //   targetX = ofClamp(x, 0, ofGetWidth()) / ofGetWidth();

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
