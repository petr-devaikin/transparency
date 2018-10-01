#version 150

// add new touch (tex0) to mask according to mask structure

// these are our textures
uniform sampler2DRect tex0;
uniform sampler2DRect mask;
uniform vec2 touchPoint;

// this comes from the vertex shader
in vec2 texCoordVarying;

// this is the output of the fragment shader
out vec4 outputColor;

void main()
{
    vec4 touchColor = texture(tex0, texCoordVarying - touchPoint).rgba;
    vec4 maskColor = texture(mask, texCoordVarying).rgba;
    
    if (maskColor.b == 1) // already active. Cannot really happen
        outputColor = maskColor;
    else {
        float touchLuminocity = touchColor.r * 256 + touchColor.g;
        float maskLuminocity = maskColor.r * 256 + maskColor.g;
        
        int resultLuminocity = int(min(maskLuminocity, touchLuminocity) * 255);
        
        outputColor = vec4((resultLuminocity / 256) / 255.0,
                           (resultLuminocity % 256) / 255.0,
                           0,
                           1);
    }
}
