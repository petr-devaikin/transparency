//
//  projection.cpp
//  transparency
//
//  Created by Petr Devaikin on 10.02.18.
//

#include "projectionTransparency.h"

projectionTransparency::projectionTransparency() {
    finalShader.load("shadersGL3/shader");
}

void projectionTransparency::init(const string& img_1_path, const string& img_2_path) {
    // load images
    image1_original.load(img_1_path);
    image2_original.load(img_2_path);
}

void projectionTransparency::setSize(int width, int height) {
    if ((width != size[0] && width > 0) || (height != size[1] && height > 0)) {
        
        // upload image again!
        image1.clone(image1_original);
        image2.clone(image2_original);
        image1.resize(width, height);
        image2.resize(width, height);
        
        // reallocate buffer
        resultFbo.allocate(width, height, GL_RGBA);
        resultFbo.begin();
        {
            ofClear(100, 0, 0, 255);
        }
        resultFbo.end();
    }
    
    projectionFading::setSize(width, height);
}


void projectionTransparency::update() {
    projectionFading::update();
    
    // update images // why???
    image1.update();
    image2.update();
    

    // final drawing
    resultFbo.begin();
    {
        ofClear(0, 0, 0, 0);
        ofSetColor(255);
        
        //maskFbo.draw(0, 0); // to test how mask is transformed
        
        finalShader.begin();
        {
            finalShader.setUniformTexture("bgTex", image1.getTexture(), 1);
            finalShader.setUniformTexture("maskTex", maskFbo.getTexture(), 2);
            image2.draw(0, 0);
        }
        finalShader.end();
    }
    resultFbo.end();
}

void projectionTransparency::draw() {
    ofSetColor(255);
    resultFbo.draw(position[0], position[1]);
}
