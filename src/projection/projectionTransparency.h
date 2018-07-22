//
//  projection.hpp
//  transparency
//
//  Created by Petr Devaikin on 10.02.18.
//

#ifndef projection_transparency_hpp
#define projection_transparency_hpp

#include "ofMain.h"
#include "projectionFading.h"

class projectionTransparency : public projectionFading {
private:
    ofImage image1_original, image2_original;
    ofImage image1, image2;
    
    ofFbo resultFbo;
    
    ofShader finalShader; // to combine two images
public:
    const float FADE_PERIOD = 2; // how long white color disappear
    
    projectionTransparency();
    
    void init(const string& image_1_path, const string& image_2_path);
    
    void setSize(int width, int height);
    
    void update();
    
    void draw();
};

#endif /* projection_transparency_hpp */
