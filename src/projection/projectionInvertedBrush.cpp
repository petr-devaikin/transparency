//
//  projectionInvertedBrush.cpp
//  transparency
//
//  Created by Petr Devaikin on 23.07.18.
//

#include "projectionInvertedBrush.hpp"


projectionInvertedBrush::projectionInvertedBrush(touchArea * t, float t1, float expSpeed, float bluredR) : baseProjection(t) {
    timer = ofGetElapsedTimef();
    
    thresholdSensitive = t1;
    expansionSpeed = expSpeed;
    bluredRadius = bluredR;
    
    // zero block
    onesAndZerosAllocated = false;
    
    // load shaders
    shaderExpansion.load("shadersGL3/expansion");
    shaderExpansionAdder.load("shadersGL3/expansionMaskAdder");
    shaderTransparency.load("shadersGL3/transparency");
    
    /*
    ofImage img;
    img.allocate(4001, 4001, OF_IMAGE_COLOR_ALPHA);
    for (int i = 0; i < 4001; i++)
        for (int j = 0; j < 4001; j++) {
            int dist = int(round(sqrt((i - 2001) * (i - 2001) + (j - 2001) * (j - 2001))));
            img.setColor(i, j, ofColor(dist / 256, dist % 256, 0));
        }
    img.update();
    ofSaveImage(img, "exp_brush_4001.png", OF_IMAGE_QUALITY_BEST);
    */
    
    touchBrush.load("exp_brush_4001.png");
}

projectionInvertedBrush::~projectionInvertedBrush() {
    // clean the memory
    if (onesAndZerosAllocated) {
        delete[] onesBlock;
    }
}

void projectionInvertedBrush::addImage(const std::string &imgPath) {
    ofImage newImg;
    newImg.load(imgPath);
    originalImages.push_back(newImg);
}

bool projectionInvertedBrush::setSize(int width, int height) {
    if (baseProjection::setSize(width, height)) {
        // resize images
        scaledImages.clear();
        for (int i = 0; i < originalImages.size(); i++) {
            ofImage newImage;
            newImage.clone(originalImages[i]);
            newImage.resize(width, height);
            scaledImages.push_back(newImage);
        }
        
        // allocate one bytes for check function
        if (onesAndZerosAllocated) {
            delete[] onesBlock;
        }
        onesAndZerosAllocated = true;
        onesBlock = new unsigned char[width * height]; // only b-channel
        memset(onesBlock, 255, width * height);
        
        // prepare touch brush
        touchBrushResized.allocate(2 * width + 1, 2 * height + 1, GL_RGBA);
        touchBrushResized.begin();
        ofClear(0);
        ofSetColor(255);
        touchBrush.draw(width - round(touchBrush.getWidth() / 2) + 1, height - round(touchBrush.getHeight() / 2) + 1);
        touchBrushResized.end();
        
        // prepare touch area for blob search
        touchAreaImage.allocate(width, height);
        
        // reset layers
        resetLayers();
        
        return true;
    }
    else
        return false;
}

// Helpers

void projectionInvertedBrush::calculateTouchBlobs() {
    ofPixels touchPixels;
    (touch->getDepth()).readToPixels(touchPixels);
    touchAreaImage.setFromPixels(touchPixels.getChannel(0));
    touchAreaImage.threshold(255 * thresholdSensitive);
    
    contourFinder.findContours(touchAreaImage, 4, size[0] * size[1], 4, false);
}

int projectionInvertedBrush::detectLayerIndex(ofPoint point) {
    // check all layers starting from the top one
    int touchedLayer = 0;
    for (touchedLayer = layers.size() - 1; touchedLayer > 0; touchedLayer--) {
        if (layers[touchedLayer].checkIfTouched(point)) break;
    }
    
    return touchedLayer;
}

// Update

void projectionInvertedBrush::update() {
    if (!onesAndZerosAllocated) return; // size is not set. not ready
    
    // expand touched areas on all layers
    float currentTime = ofGetElapsedTimef();
    float expansionRadius = expansionSpeed * (currentTime - timer);
    if (expansionRadius >= 1) {
        float floorRadius = floor(expansionRadius);
        timer = currentTime - (expansionRadius - floorRadius) / expansionSpeed;
        
        for (int i = 0; i < layers.size(); i++) {
            layers[i].expand(floorRadius);
        }
    }
    
    // check if we can remove the last layer
    if (layers.size() > 1 && layers[1].isFull()) {
        layers.erase(layers.begin());
        cout << "Layer removed. Number of layers: " << layers.size() << "\n";
    }
    
    // Check if the canvas is touched right now and calculate blobs
    calculateTouchBlobs();
    
    // compare new blobs with previous touches
    vector<layerTouch> newTouches;
    for (int i = 0; i < contourFinder.nBlobs; i++) {
        ofxCvBlob blob = contourFinder.blobs[i];
        bool previousTouchDetected = false;
        for (int j = 0; j < currentTouches.size(); j++) {
            if (blob.boundingRect.intersects(currentTouches[j].area)) {
                // same gesture. Copy layer from the previous touch
                layerTouch newTouch(currentTouches[j].layer, blob.boundingRect);
                newTouches.push_back(newTouch);
                
                // touch the layer
                newTouch.layer->addTouch(blob.centroid, bluredRadius);
                
                previousTouchDetected = true;
                break;
            }
        }
        
        if (!previousTouchDetected) {
            // new touch, detect layer
            int touchedLayerIndex = detectLayerIndex(blob.centroid);
            cout << "Layer touched: " << touchedLayerIndex << "\n";
            
            if (touchedLayerIndex == layers.size() - 1) {
                int newLayerImageIndex = (layers[touchedLayerIndex].getImageIndex() + 1) % originalImages.size();
                addImageToLayers(newLayerImageIndex);
                cout << "New layer " << newLayerImageIndex << " added. Number of layers: " << layers.size() << "\n";
            }
            
            layerTouch newTouch(&(layers[touchedLayerIndex + 1]), blob.boundingRect);
            newTouches.push_back(newTouch);
            
            // touch the layer
            newTouch.layer->addTouch(blob.centroid, bluredRadius);
        }
    }
    currentTouches = newTouches;
}

void projectionInvertedBrush::draw() {
    if (!onesAndZerosAllocated) return; // size is not set. not ready
    
    ofSetColor(255);
    shaderTransparency.begin();
    shaderTransparency.setUniform1i("radius", bluredRadius);
    
    for (int i = 0; i < layers.size(); i++) {
        shaderTransparency.setUniformTexture("mask", layers[i].getMask().getTexture(), 1);
        layers[i].getImage().draw(position[0], position[1]);
    }
    
    shaderTransparency.end();
    
    /*
     // draw blobs
    for (int i = 0; i < contourFinder.nBlobs; i++) {
        ofRectangle rect = contourFinder.blobs.at(i).boundingRect;
        ofSetColor(255, 0, 0);
        ofDrawRectangle(position[0] + rect.getLeft(), position[1] + rect.getTop(), rect.width, rect.height);
    }
     */
}

void projectionInvertedBrush::resetLayers() {
    layers.clear(); // need to clean memory?
    addImageToLayers(0);
    layers[0].setFull();
    
    cout << "Reset layers. Number of layers: " << layers.size() << "\n";
}

void projectionInvertedBrush::addImageToLayers(int i) {
    layerWithMask newLayer(scaledImages[i], i, onesBlock, shaderExpansion, shaderExpansionAdder, touchBrushResized);
    layers.push_back(newLayer);
}
