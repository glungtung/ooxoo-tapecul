#include "ofMain.h"
#include "ofApp.h"

//========================================================================
int main( ){
    ofGLFWWindowSettings settings;
    //settings.setGLVersion(3,2);

    settings.width = 1680;
    settings.height = 1050;
    settings.monitor = 0;
    settings.setPosition(ofVec2f(0,0));
    //settings.multiMonitorFullScreen = true;
    settings.windowMode = OF_FULLSCREEN;
    settings.resizable = false;
    shared_ptr<ofAppBaseWindow> mainWindow = ofCreateWindow(settings);
    
    settings.width = 1600;
    settings.height = 1200;
    settings.setPosition(ofVec2f(1680,0));
    settings.monitor = 1;
    settings.resizable = false;
    settings.windowMode = OF_FULLSCREEN;
    // uncomment next line to share main's OpenGL resources with gui
    settings.shareContextWith = mainWindow;
    shared_ptr<ofAppBaseWindow> secondWindow = ofCreateWindow(settings);
    secondWindow->setVerticalSync(true);
    
    shared_ptr<ofApp> mainApp(new ofApp);
    ofAddListener(secondWindow->events().draw,mainApp.get(),&ofApp::drawSecondWindow);
    
   /* secondWindow->setOrientation(OF_ORIENTATION_90_LEFT);
    // TODO: every window should set orientation on it's renderer
    if(secondWindow->renderer()){
        secondWindow->renderer()->setOrientation(OF_ORIENTATION_90_LEFT,true);
    }
    */
    ofRunApp(mainWindow, mainApp);
    ofRunMainLoop();

   // mainWindow->setOrientation(OF_ORIENTATION_90_LEFT);
   // mainWindow->setFullscreen(true);


   // secondWindow->setFullscreen(true);
    
}
