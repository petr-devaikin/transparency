#version 150

// these are our textures
uniform sampler2DRect tex0;
uniform float threshold;

// this comes from the vertex shader
in vec2 texCoordVarying;

// this is the output of the fragment shader
out vec4 outputColor;

void main()
{
    // get rgb from tex0
    vec3 c = texture(tex0, texCoordVarying).rgb;
    
    if (c.r >= threshold)
        outputColor = vec4(1, 1, 1, 1);
    else
        outputColor = vec4(0, 0, 0, 0);
}
