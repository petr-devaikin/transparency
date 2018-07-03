#version 150
// leave the brightest pixels

// these are our textures
uniform sampler2DRect tex0;
uniform sampler2DRect oldTex;
uniform float fadeLevel;

// this comes from the vertex shader
in vec2 texCoordVarying;

// this is the output of the fragment shader
out vec4 outputColor;

void main()
{
    // get rgb from tex0
    vec3 picOne = texture(tex0, texCoordVarying).rgb;
    vec3 picTwo = texture(oldTex, texCoordVarying).rgb;
    
    //mix the rgb from tex0 with the alpha of the mask
    outputColor = vec4(
                       max(picOne.r, picTwo.r - fadeLevel),
                       max(picOne.g, picTwo.g - fadeLevel),
                       max(picOne.b, picTwo.b - fadeLevel),
                       1);
}
