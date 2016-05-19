//
//  ofxParallax.h
//
//  Created by Daniel Rosser on 10/04/2014.
//
//

#ifndef __ofxParallax__
#define __ofxParallax__

#include "ofMain.h"
#include "ofxParallaxShader.h"
#if defined(TARGET_WIN32)
#include "ofxDSHapVideoPlayer.h"
#define ofxHapPlayer ofxDSHapVideoPlayer
#else
#include "ofxHapPlayer.h"
#endif


struct ofxParallaxLayer {
    ofxParallaxLayer()
        : texture(NULL), pos(0,0), player(NULL) { }
    ~ofxParallaxLayer() {

        // only for image, managed inside this class
        // videos are managed in main app
        if(texture != NULL && player == NULL) {
            delete texture;
            texture = NULL;
        }
    }
    
    void setup(string path, ofPoint position) {
        if(texture != NULL) {
            delete texture;
            texture = NULL;
        }
        ofImage * imageLoader = new ofImage();
        bool completed = imageLoader->load(path);
        if(!completed) {
            ofLog(OF_LOG_WARNING, "ofxParallaxLayer did not load image: " + path);
        } else {
            texture = new ofTexture();
            texture->allocate(imageLoader->getPixels());
            texture->loadData(imageLoader->getPixels());
        }
        
        pos = position;
        
        initShader();
    }
    
    void setup(ofTexture * tex, ofPoint position) {
        if(texture != NULL) {
            delete texture;
            texture = NULL;
        }
        texture = tex;
        pos = position;
        
        initShader();
    }

    void setup(ofxHapPlayer * play, ofPoint position) {
        if(player != NULL) {
            delete player;
            player = NULL;
        }
        player = play;
        pos = position;
        
        initShader();
    }
    
    void initShader() {
        fboBlurOnePass = new ofFbo();
        fboBlurTwoPass = new ofFbo();
        fboVideoPass = new ofFbo();
        
        int w = (player?player->getWidth():texture->getWidth());
        int h = (player?player->getHeight():texture->getHeight());
        
        fboBlurOnePass->allocate(w, h, GL_RGBA);
        fboBlurTwoPass->allocate(w, h, GL_RGBA);
        fboVideoPass->allocate(w, h, GL_RGBA);
        
        if(ofIsGLProgrammableRenderer()){
            shaderBlurX.load("shadersGL3/shaderBlurX");
            shaderBlurY.load("shadersGL3/shaderBlurY");
        }else{
            shaderBlurX.load("shadersGL2/shaderBlurX");
            shaderBlurY.load("shadersGL2/shaderBlurY");
        }
    }
    
    void draw(ofPoint offset) {
        if (player && player->isInitialized()) {
            texture = player->getTexture();
        }
        
        transformation.makeIdentityMatrix();
        ofPushMatrix();
        transformation.preMultTranslate(pos);
        if (texture != NULL) {
           // float blur = ofMap(ofGetMouseY(), 0, ofGetHeight(), 0, 10, true);
            ofEnableAlphaBlending();
            
            
            //----------------------------------------------------------
            fboVideoPass->begin();
            ofClear(255, 255, 255, 0);
            
            ofSetColor(255, 255, 255);
            texture->draw(0, 0);
            
            fboVideoPass->end();
            

            
            //----------------------------------------------------------
            fboBlurOnePass->begin();
            ofClear(255, 255, 255, 0);

            shaderBlurX.begin();
            shaderBlurX.setUniform1f("blurAmnt", blur);
           
            fboVideoPass->draw(0,0);
            
            shaderBlurX.end();
            
            fboBlurOnePass->end();
            
            //----------------------------------------------------------
            fboBlurTwoPass->begin();
            ofClear(255, 255, 255, 0);

            shaderBlurY.begin();
            shaderBlurY.setUniform1f("blurAmnt", blur);
            
            fboBlurOnePass->draw(0, 0);
            
            shaderBlurY.end();
            
            fboBlurTwoPass->end();
            
            //----------------------------------------------------------
            ofSetColor(ofColor::white);
            fboBlurTwoPass->draw(0, 0);
            
        }
        ofPopMatrix();
    }
    
    void setBlur(float blurAmount) {
        blur = blurAmount;
    }
    
    // Variables
    ofxHapPlayer *player;
    ofTexture * texture;
    ofPoint pos;
    ofMatrix4x4 transformation;
    
    float blur;
    
    ofShader shaderBlurX;
    ofShader shaderBlurY;
    ofFbo *fboBlurOnePass;
    ofFbo *fboBlurTwoPass;
    ofFbo *fboVideoPass;

};

struct ofxParallaxLayers {

    ofxParallaxLayers() :  offset(0,0), isBlurred(false), hasRenderedTexture(false), blurAmount(1), lastFrameCollision(false), collision(0,0), blurStart(0.0), blurSpeed(1.0) {
        
    }
    ~ofxParallaxLayers() {
        if(layer.size() != 0) {
            int size = layer.size()-1;
            for(int i=size; i>=0; i--){
                ofxParallaxLayer* toRemove = layer[i];
                delete toRemove;
                toRemove = NULL;
                layer.pop_back();
            }
        }
        layer.clear();
    }
    
    void exit() {
        
    }
    
    void setupLayersWithTexture(ofTexture * tex, ofPoint pos) {
        ofxParallaxLayer * pnew =  new ofxParallaxLayer();
        pnew->setup(tex, pos);
        layer.push_back(pnew);
    }
    
    void addToLayers(string path, ofPoint pos) {
        ofxParallaxLayer * pnew =  new ofxParallaxLayer();
        pnew->setup(path, pos);
        layer.push_back(pnew);
    }

    void addToLayers(ofxHapPlayer *play, ofPoint pos) {
        ofxParallaxLayer * pnew =  new ofxParallaxLayer();
        pnew->setup(play, pos);
        layer.push_back(pnew);
    }
    
    void setup() {
        
        //  scale = ofVec3f(0.5f,0.5f,0.5f);
        scale = ofVec3f(1.0f,1.0f,1.0f);
        dynamicScale = ofVec3f(1.0f,1.0f,1.0f);

    }
    
    void update(float impulse) {
        
//        int number = layer.size()-1;
        if(layer.size() != 0) {
            lastFrameCollision = false;
            if(impulse != 0.0) {
                offset.y = offset.y - (impulse / (speed * ofGetLastFrameTime()));
            }
           
            if(offset.y <= collision.x) {
                offset.y = collision.x;
                lastFrameCollision = true;
            } else if(offset.y >= collision.y) {
                offset.y = collision.y;
                lastFrameCollision = true;
            }
            
//            for(int i = 0; i<=number;i++){
//                ofxParallaxLayer* theLayer = layer[i];
//                theLayer->update();
//            }
        }
        
        transformation.makeIdentityMatrix();
        transformation.preMultTranslate(offset - zoomTopLeft);
        transformation.preMultScale(scale * dynamicScale);
        
        
    }
    
    
    void updateTarget(float target) {
        
        if(layer.size() != 0) {
            lastFrameCollision = false;
            offset.y = origin.x - target * ofGetWidth() * speed;
            blurAmount = blurStart + target * blurSpeed;
            
            if(offset.y <= collision.x) {
                offset.y = collision.x;
                lastFrameCollision = true;
            } else if(offset.y >= collision.y) {
                offset.y = collision.y;
                lastFrameCollision = true;
            }

        }
        
        transformation.makeIdentityMatrix();
        transformation.preMultTranslate(offset - zoomTopLeft);
        transformation.preMultScale(scale * dynamicScale);
        
        
    }
    
    
    void draw() {
        ofPushMatrix();
        ofMultMatrix(transformation);
        if(layer.size() != 0) {
/*            if(hasRenderedTexture == true) {
                fboLayer->draw(ofVec2f(0,0));
            } else {
            if(isBlurred && hasRenderedTexture == false) {
                beginShader();
            }
            int number = layer.size()-1;
            for(int i = 0; i<=number;i++) {
                ofxParallaxLayer* theLayer = layer[i];
                theLayer->draw(ofVec2f(0,0));
            }
            
            if(isBlurred && hasRenderedTexture == false) {
                endShader();
                if(hasRenderedTexture == true) {
                    fboLayer->draw(ofVec2f(0,0));
                }
            }
            }
 */
                int number = layer.size()-1;
                for(int i = 0; i<=number;i++) {
                    ofxParallaxLayer* theLayer = layer[i];
                    theLayer->setBlur(blurAmount);
                    theLayer->draw(ofVec2f(0,0));
                }

                
        }
        ofPopMatrix();
    }
    
    void beginShader() {
 /*
        fboLayer->begin();
        ofClear(255, 0);
        shader->begin();
        shader->setUniform1f("fade", 1.0);
        shader->setUniform1f("blurAmnt", blurAmount);
        shader->setUniform1f("textureWidth", ofGetWidth());
        shader->setUniform1f("bloom", isBlurred);
        ofPushMatrix();
       // ofTranslate(offset.y * -1, 0);
        
        //transformation.makeIdentityMatrix();
        //ofPushMatrix();
        //transformation.preMultTranslate(ofVec2f(0,0));
*/
    }
    
    void endShader() {
 /*       ofPopMatrix();
        shader->end();
        fboLayer->end();
        hasRenderedTexture = true;
   */
    }
    
    
    void initShader() {
/*        if(shader != NULL) {
            shader->bindDefaults();
            shader->linkProgram();
        }
 */
    }
    
    bool hasCollided() {
        return lastFrameCollision;
    }
    
    void setBlur(bool isOn) {
        if(isOn) {
            hasRenderedTexture = false;
            isBlurred = true;
        } else {
            hasRenderedTexture = false;
            isBlurred = false;
        }
    }
    
    void setZoom(ofVec3f newScale, ofVec2f zoomOrigin) {
        dynamicScale = newScale;
        zoomTopLeft = ofVec2f(zoomOrigin.x, zoomOrigin.y);
        
//        ofMatrix4x4 max;
//        max = max.newTranslationMatrix(zoomOrigin.x, zoomOrigin.y, 0);
//        max.preMult(transformation);
//        ofVec3f trans = max.getTranslation();
//        zoomTopLeft = ofVec2f(trans.x, trans.y);
    }
    
    //-----------------
    vector<ofxParallaxLayer*> layer;

    ofVec2f offset;
    ofVec2f origin;
    ofVec2f size;
    ofPoint collision;
    ofMatrix4x4 transformation;
    ofVec3f scale;
    ofVec3f dynamicScale;
    ofVec2f zoomTopLeft;
    
    bool isBlurred;
    bool hasRenderedTexture;
    float speed;
    float blurAmount;
    bool lastFrameCollision;
    float blurStart;
    float blurSpeed;
};


class ofxParallax {
public:
    ofxParallax() : numberOflayers(0) {
        
    }
    ~ofxParallax() {
        removeLayers();
    };
    
    void setup() {
        
    }
    
    void removeLayers() {
        if(layers.size() != 0) {
            int size = layers.size()-1;
            for(int i=size; i>=0; i--){
                ofxParallaxLayers* toRemove = layers[i];
                delete toRemove;
                toRemove = NULL;
                layers.pop_back();
            }
        }
        layers.clear();
    }
    
    bool loadDirectory(string directory) {
        
        vector<string> imageSequencePaths;
        ofDirectory dir(directory);
        int numOfFiles = dir.listDir();
        for(int i=0; i<numOfFiles; i++) {
            ofLog(OF_LOG_NOTICE, "loading... " + dir.getPath(i));
            imageSequencePaths.push_back(dir.getPath(i));
        }
        
        bool bLoaded = imageSequencePaths.size() > 0;
        if(bLoaded == false) {
            return false;
        }
        
        if(layers.size() != 0) {
            removeLayers();
        }
        
        for(int i=0; i<imageSequencePaths.size(); i++) {
            
            ofImage * imageLoader = new ofImage();
            ofTexture * texture = NULL;
            bool completed = imageLoader->loadImage(imageSequencePaths[i]);
            if(!completed) {
                ofLog(OF_LOG_WARNING, "ofxParallaxLayer did not load image: " + imageSequencePaths[i]);
            } else {
                texture = new ofTexture();
                texture->allocate(imageLoader->getPixelsRef());
                texture->loadData(imageLoader->getPixelsRef());
                
                if(!texture->isAllocated()) {
                    completed = false;
                }
                
                imageLoader->clear();
                delete imageLoader;
                imageLoader = NULL;
            }
            
            if(completed) {
                addNewLayer(i,                              // layer index
                            ofVec2f(0, 0),                  // origin point (topleft)
                            ofVec2f(texture->getWidth(), texture->getHeight()),         // size
                            getLayerSpeed(i),                         // index speed;
                            ofVec2f(-(texture->getWidth()/2), +(texture->getWidth()/2))   // collision left and collision right
                            );
                layers[i]->setupLayersWithTexture(texture, ofPoint(0,0));
            }
        }
        return bLoaded;
    }
    
    void addNewLayer(int layer, ofVec2f origin, ofVec2f size, float speed, ofVec2f collision = ofVec2f::zero()) {
        if(layers.size()-1 == layer) {
            ofLog(OF_LOG_WARNING, "ofxParallax::addNewLayer : Layer already created.");
        } else if (layers.size() != layer) {
            ofLog(OF_LOG_WARNING, "ofxParallax::addNewLayer : Layed one step to many");
        } else {

            ofxParallaxLayers* theLayer = new ofxParallaxLayers();
            if(collision == ofVec2f::zero()) {
                collision = ofPoint((origin.y - size.y/2), origin.y + size.y);
            }
            theLayer->collision = collision;
            theLayer->offset = origin;
            theLayer->origin = origin;
            theLayer->size = size;
            theLayer->speed = speed;
            theLayer->setup();
            
            layers.push_back(theLayer);
        }
    }

    void addNewLayer(int layer, ofVec2f origin, ofVec2f size, float speed, ofVec2f collision, float blurStart, float blurSpeed) {
        addNewLayer(layer, origin, size, speed, collision);
        layers.back()->blurStart = blurStart;
        layers.back()->blurSpeed = blurSpeed;
    }
    
    void addImageToLayer(int layer, string imagePath, ofPoint pos) {
        if(layer <= layers.size()-1) {
            // all good
            layers[layer]->addToLayers(imagePath, pos);
        }
    }
    
    void addVideoToLayer(int layer, ofxHapPlayer *play, ofPoint pos) {
        if(layer <= layers.size()-1) {
            // all good
            layers[layer]->addToLayers(play, pos);
        }
    }
    
    void exit() {
        
    }
    
    void update(float impulse) {
        if(layers.size() != 0) {
            int number = layers.size()-1;
            bool aboveHasStopped = false;
            for(int i = number; i>=0;i--) {
                if(aboveHasStopped == false) {
                    ofxParallaxLayers* theLayer = layers[i];
                    theLayer->update(impulse);
                    aboveHasStopped = theLayer->hasCollided();
                }
            }
        }
    }
    
    void updateTarget(float target) {
        if(layers.size() != 0) {
            int number = layers.size()-1;
            bool aboveHasStopped = false;
            for(int i = number; i>=0;i--) {
                if(aboveHasStopped == false) {
                    ofxParallaxLayers* theLayer = layers[i];
                    theLayer->updateTarget(target);
                    aboveHasStopped = theLayer->hasCollided();
                }
            }
        }
    }
    
    void draw() {
        if(layers.size() != 0) {
            int number = layers.size()-1;
            for(int i = 0; i<=number;i++) {
                ofxParallaxLayers* theLayer = layers[i];
                theLayer->draw();
            }
        }
    }
    
    void unblurAll() {
        if(layers.size() != 0) {
            int number = layers.size()-1;
            for(int i = 0; i<=number;i++) {
                layers[i]->setBlur(false);
            }
        }
    }
    
    void blurAll() {
        if(layers.size() != 0) {
            int number = layers.size()-1;
            for(int i = 0; i<=number;i++) {
                layers[i]->setBlur(true);
            }
        }
    }
    
    void blurLayer(int layerID) {
        if(layers.size() != 0) {
            int number = layers.size()-1;
            if(layerID <= number){
                layers[layerID]->setBlur(true);
            }
        }
    }
    
    void blurAllExceptLayer(int layerID) {
        if(layers.size() != 0) {
            blurAll();
            int number = layers.size()-1;
            if(layerID <= number){
                layers[layerID]->setBlur(false);
                
            }
        }
    }
    
    int getTotalLayersSize() {
        return layers.size();
    }
    
    void zoomAll(ofVec3f zoom, ofVec2f zoomOrigin) {
        if(layers.size() != 0) {
            int number = layers.size()-1;
            for(int i = 0; i<=number;i++) {
                layers[i]->setZoom(zoom, zoomOrigin);
            }
        }
    }
    
    virtual const float getLayerSpeed(int layerIndex) {
        // with 6 layers.
        float layerSpeed = 0.0f;
        switch (layerIndex) {
            case 0:
                layerSpeed = 5 * 360;
                break;
            case 1:
                layerSpeed = 5 * 330;
                break;
            case 2:
                layerSpeed = 5 * 300;
                break;
            case 3:
                layerSpeed = 5 * 130;
                break;
            case 4:
                layerSpeed = 5 * 90;
                break;
            case 5:
                layerSpeed = 5 * 80;
                break;
            case 6:
                layerSpeed = 5 * 50;
                break;
            default:
                layerSpeed = 0.0f;
                ofLog(OF_LOG_WARNING, "ofxParallax:: getLayerSpeed: Outside of default set speeds. Please implement your own.");
                break;
        }
        return layerSpeed;
    }

    
    
protected:
    int numberOflayers;
    vector<ofxParallaxLayers*> layers;
    
};

#endif /* defined(__ofxParallax__) */
