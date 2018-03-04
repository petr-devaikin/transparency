#version 150

// these are our textures
uniform sampler2DRect tex0;
uniform sampler2DRect bgTex;
uniform sampler2DRect maskTex;
uniform float multiplier;

// this comes from the vertex shader
in vec2 texCoordVarying;

// this is the output of the fragment shader
out vec4 outputColor;

void main()
{
    // get rgb from tex0
    vec3 background = texture(bgTex, texCoordVarying).rgb;
    vec3 foreground = texture(tex0, texCoordVarying).rgb;

    // get alpha from mask
    float mask = texture(maskTex, texCoordVarying).r * multiplier;
    if (mask > 1) mask = 1;
    
    //mix the rgb from tex0 with the alpha of the mask
    outputColor = vec4(
                       background.r * (1 - mask) + foreground.r * mask,
                       background.g * (1 - mask) + foreground.g * mask,
                       background.b * (1 - mask) + foreground.b * mask,
                       1);
}
