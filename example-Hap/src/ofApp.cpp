/*
 ofApp.cpp
 ofxHapPlayerExample
 
 Copyright (c) 2013, Tom Butterworth. All rights reserved.
 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:
 
 * Redistributions of source code must retain the above copyright
 notice, this list of conditions and the following disclaimer.
 
 * Redistributions in binary form must reproduce the above copyright
 notice, this list of conditions and the following disclaimer in the
 documentation and/or other materials provided with the distribution.
 
 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS BE LIABLE FOR ANY
 DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    // Limit drawing to a sane rate
    ofSetVerticalSync(true);
    
    ofBackground(220, 220, 220);
    
    // Load a movie file
    //   player.load("movies/ptitbonhomme.mov");
    player.load("movies/SampleHap.mov");
    
    // Start playback
    player.play();
    
    vid.load("movies/IMG_4224.MOV");
    vid.play();
    vid.setLoopState(OF_LOOP_NORMAL);
    
#ifdef TARGET_OPENGLES
    shaderBlurX.load("shadersES2/shaderBlurX");
    shaderBlurY.load("shadersES2/shaderBlurY");
#else
    if(ofIsGLProgrammableRenderer()){
        shaderBlurX.load("shadersGL3/shaderBlurX");
        shaderBlurY.load("shadersGL3/shaderBlurY");
    }else{
        shaderBlurX.load("shadersGL2/shaderBlurX");
        shaderBlurY.load("shadersGL2/shaderBlurY");
    }
#endif
    
    cout << player.getWidth() << endl;
    fboBlurOnePass.allocate(player.getWidth(), player.getHeight());
    fboBlurTwoPass.allocate(player.getWidth(), player.getHeight());
    fboVideoPass.allocate(player.getWidth(), player.getHeight());
}

//--------------------------------------------------------------
void ofApp::update(){
    // Signal the player to update
    player.update();
    vid.update();
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    ofEnableAlphaBlending();
    
    float blur = ofMap(mouseX, 0, ofGetWidth(), 0, 10, true);
    
    
    
    //----------------------------------------------------------
    fboVideoPass.begin();
    
    ofSetColor(255, 255, 255);
    if (player.isLoaded())
    {
        // Draw the frame
        player.draw(0, 0);
    }
    
    // Draw the FPS display
    ofSetColor(20,20,20);
    ofDrawBitmapString(ofToString(ofGetFrameRate(), 0) + " FPS", 20, 20);
    
    fboVideoPass.end();

    
    //----------------------------------------------------------
    fboBlurOnePass.begin();
    
    shaderBlurX.begin();
    shaderBlurX.setUniform1f("blurAmnt", blur);
    
    fboVideoPass.draw(0,0);
    
    shaderBlurX.end();
    
    fboBlurOnePass.end();
    
    //----------------------------------------------------------
    fboBlurTwoPass.begin();
    
    shaderBlurY.begin();
    shaderBlurY.setUniform1f("blurAmnt", blur);
    
    fboBlurOnePass.draw(0, 0);
    
    shaderBlurY.end();
    
    fboBlurTwoPass.end();
    
    //----------------------------------------------------------
    ofSetColor(ofColor::white);
    //fboBlurTwoPass.draw(20, 30);
    fboBlurOnePass.draw(20, 30);
    

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    float position = (float) x / (float) ofGetWindowWidth();
    cout << "Setting playhead to " << ofToString(position) << endl;
    player.setPosition(position);
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){
	vector< string > fileList = dragInfo.files;
    player.load(fileList[0]);
}
