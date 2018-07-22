//
//  projection.hpp
//  transparency
//
//  Created by Petr Devaikin on 10.02.18.
//

#ifndef projection_flowers_hpp
#define projection_flowers_hpp

#include "ofMain.h"
#include "projectionFading.h"

class Flower {
private:
    float targetSize;
    
    float growSpeed; // px per second
    float decreaseSpeed;
    float maxSize;
    
    float lastUpdateTime;
protected:
    ofVec2f position;
    float currentSize;
public:
    Flower(float _maxSize = 50, float _growSpeed = 0, float _decreaseSpeed = 0);
    
    void setPosition(float x, float y);
    void setMaxSize(float size);
    void update(ofPixels image);
    
    void draw();
};

class EmojiFlower : public Flower {
private:
    ofImage img;
public:
    EmojiFlower(float _maxSize = 50, float _growSpeed = 0, float _decreaseSpeed = 0);
    
    void draw();
};

// -------

class projectionFlowers : public projectionFading {
private:
    ofFbo resultFbo;
    vector<EmojiFlower> flowers;
public:
    const float FADE_PERIOD = 2; // how long white color disappear
    
    projectionFlowers(int flowerNumber);
    
    void setSize(int width, int height);
    
    void update();
    
    void draw();
};

#endif /* projection_flowers_hpp */
