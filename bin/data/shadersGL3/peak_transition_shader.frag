#version 150
// leave the brightest pixels

// these are our textures
uniform sampler2DRect tex0;

// this comes from the vertex shader
in vec2 texCoordVarying;

// this is the output of the fragment shader
out vec4 outputColor;

void main()
{
    // get rgb from tex0
    vec3 picOne = texture(tex0, texCoordVarying).rgb;
    
    float v = (1 - cos(2 * 3.1416 * picOne.r)) / 2;
    
    //mix the rgb from tex0 with the alpha of the mask
    outputColor = vec4(
                       v,
                       v,
                       v,
                       1);
}
