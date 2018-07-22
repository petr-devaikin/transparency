#version 150
// leave the brightest pixels

// these are our textures
uniform sampler2DRect tex0;
uniform sampler2DRect currentState;
uniform float timeCoef;

// this comes from the vertex shader
in vec2 texCoordVarying;

// this is the output of the fragment shader
out vec4 outputColor;

void main()
{
    // get rgb from tex0
    vec3 picOne = texture(tex0, texCoordVarying).rgb;
    // get rgb from currentState
    vec3 picCurrentState = texture(tex0, texCoordVarying).rgb;
    
    float v = picCurrentState.r + timeCoef * picOne.r;
    v = v - floor(v); // leave only decimal part
    
    //mix the rgb from tex0 with the alpha of the mask
    outputColor = vec4(
                       v,
                       v,
                       v,
                       1);
}
