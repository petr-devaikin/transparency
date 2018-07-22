#version 150
// leave the brightest pixels

// these are our textures
uniform sampler2DRect tex0;
uniform float angle;

// this comes from the vertex shader
in vec2 texCoordVarying;

// this is the output of the fragment shader
out vec4 outputColor;

vec3 hsv2rgb(vec3 c) {
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

void main()
{
    // get rgb from tex0
    vec3 picOne = texture(tex0, texCoordVarying).rgb;
    
    float v = (cos(angle) * 0.3 + 1) * picOne.r;
    if (v > 255) v = 255;

    vec3 color = hsv2rgb(vec3(v * 5 / 6, 1, 1)); // scale hue a bit to end up with purple, not red again
    
    //mix the rgb from tex0 with the alpha of the mask
    outputColor = vec4(
                       color.r,
                       color.g,
                       color.b,
                       1);
}
