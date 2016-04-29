#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
   // ofSetOrientation(OF_ORIENTATION_90_LEFT);
    ofScale(0.6, 0.6);
    
    // Load a movie file
    lune.load("movies/P-lune-1.mov");
    lion.load("movies/P-nuage-lion-2.mov");
    nuage3.load("movies/P-nuage-3.mov");
    nuage4.load("movies/P-nuage-4.mov");
    nuage5.load("movies/P-nuage-5.mov");
    
    // Start playback
    lune.setLoopState(OF_LOOP_NORMAL);
    lion.setLoopState(OF_LOOP_NORMAL);
    nuage3.setLoopState(OF_LOOP_NORMAL);
    nuage4.setLoopState(OF_LOOP_NORMAL);
    nuage5.setLoopState(OF_LOOP_NORMAL);
    
    lune.play();
    lion.play();
    nuage3.play();
    nuage4.play();
    nuage5.play();
    
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
    parallax.addNewLayer(0, ofVec2f(0, 0), ofVec2f(2536, 768), 0.1, ofVec2f(-9000, 9000)); // ciel
    parallax.addNewLayer(1, ofVec2f(0, 0), ofVec2f(1280, 768), 0.3, ofVec2f(-9000, 9000)); // lune
    parallax.addNewLayer(2, ofVec2f(0, 0), ofVec2f(1280, 800), 0.7, ofVec2f(-9000, 9000));   // lion
    parallax.addNewLayer(3, ofVec2f(0, 0), ofVec2f(1280, 800), 0.8, ofVec2f(-9000, 9000));   // nuages
    parallax.addNewLayer(4, ofVec2f(0, 0), ofVec2f(1280, 800), 1, ofVec2f(-9000, 9000));   // nuages
    parallax.addNewLayer(5, ofVec2f(0, 0), ofVec2f(1280, 800), 1.3, ofVec2f(-9000, 9000));   // nuages
    parallax.unblurAll();
    
    
    parallax.addImageToLayer(0, "images/P-ciel.jpg", ofPoint(0, 0));
    parallax.addVideoToLayer(1, &lune, ofPoint(0, 0));
//    parallax.addImageToLayer(2, "images/1-foreground.png", ofPoint(1000, 0));
    parallax.addVideoToLayer(2, &lion, ofPoint(0, 0));
    parallax.addVideoToLayer(3, &nuage3, ofPoint(0, 0));
    parallax.addVideoToLayer(4, &nuage4, ofPoint(0, 0));
    parallax.addVideoToLayer(5, &nuage5, ofPoint(0, 0));
    
    isTouched = true;

    //OSC
    receiver.setup(PORT);
    receivedX = 0.;
    prevReceivedX = 0.;
    

    // replace the string below with the serial port for your Arduino board
    // you can get this from the Arduino application or via command line
    // for OSX, in your terminal type "ls /dev/tty.*" to get a list of serial devices
    ard.connect("/dev/cu.usbmodemfa131", 57600);
    
    // listen for EInitialized notification. this indicates that
    // the arduino is ready to receive commands and it is safe to
    // call setupArduino()
    ofAddListener(ard.EInitialized, this, &ofApp::setupArduino);
    bSetupArduino	= false;	// flag so we setup arduino when its ready, you don't need to touch this :)
}

//--------------------------------------------------------------
void ofApp::update(){
    ofSetWindowTitle(ofToString(ofGetFrameRate()));
    
    updateArduino();
    
    lune.update();
    lion.update();
    nuage3.update();
    nuage4.update();
    nuage5.update();

    
    // check for waiting messages
/*    while(receiver.hasWaitingMessages()){
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
 */
    
    receivedX = accelX;
    prevX.push_front(receivedX);
    if (prevX.size() > 5)
        prevX.pop_back();


    avgX = 0;
    for (auto x : prevX)
        avgX += x;
    if (prevX.size() > 0)
        avgX = avgX / prevX.size();
    
    
   // if (abs(receivedX - prevReceivedX) > 0.01) {
    receivedX = prevReceivedX + (avgX - prevReceivedX) / 10;
    //cout << receivedX << endl;

    prevReceivedX = receivedX;
    targetX = receivedX;
//    targetX = ofxeasing::map(receivedX, 0.3, 0.7, 0., 1., ofxeasing::linear::easeIn);
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



//--------------------------------------------------------------
void ofApp::setupArduino(const int & version) {
    
    // remove listener because we don't need it anymore
    ofRemoveListener(ard.EInitialized, this, &ofApp::setupArduino);
    
    // it is now safe to send commands to the Arduino
    bSetupArduino = true;
    
    // print firmware name and version to the console
    ofLogNotice() << ard.getFirmwareName();
    ofLogNotice() << "firmata v" << ard.getMajorFirmwareVersion() << "." << ard.getMinorFirmwareVersion();
    
    // Note: pins A0 - A5 can be used as digital input and output.
    // Refer to them as pins 14 - 19 if using StandardFirmata from Arduino 1.0.
    // If using Arduino 0022 or older, then use 16 - 21.
    // Firmata pin numbering changed in version 2.3 (which is included in Arduino 1.0)
    
    // set pin A0 to analog input : X angle
    ard.sendAnalogPinReporting(0, ARD_ANALOG);
    // set pin A4 to analog input : force pressure
    ard.sendAnalogPinReporting(4, ARD_ANALOG);
    
    // Listen for changes on the digital and analog pins
//    ofAddListener(ard.EDigitalPinChanged, this, &ofApp::digitalPinChanged);
    ofAddListener(ard.EAnalogPinChanged, this, &ofApp::analogPinChanged);
}

//--------------------------------------------------------------
void ofApp::updateArduino(){
        // update the arduino, get any data or messages.
    // the call to ard.update() is required
    ard.update();
}


// analog pin event handler, called whenever an analog pin value has changed

//--------------------------------------------------------------
void ofApp::analogPinChanged(const int & pinNum) {
    // do something with the analog input. here we're simply going to print the pin number and
    // value to the screen each time it changes
    if (pinNum == 0)
        accelX = ofMap(ard.getAnalog(pinNum), 430,560, 0, 1);
    if (pinNum == 4)
        forcePressureLeft = ard.getAnalog(pinNum);
}

