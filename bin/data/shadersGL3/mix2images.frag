#version 150

// special transparency based on the mask. see expansion

// these are our textures
uniform sampler2DRect tex0;
uniform sampler2DRect mask;
uniform sampler2DRect background;
uniform int radius;

// this comes from the vertex shader
in vec2 texCoordVarying;

// this is the output of the fragment shader
out vec4 outputColor;


void main()
{
    vec4 maskColor = texture(mask, texCoordVarying).rgba;
    vec4 color = texture(tex0, texCoordVarying).rgba;
    vec4 bgColor = texture(background, texCoordVarying).rgba;

    if (maskColor.b == 1)
        outputColor = color;
    else {
        float luminocity = maskColor.r * 255 * 256 + maskColor.g * 255;
        if (luminocity > radius)
            outputColor = vec4(bgColor.r, bgColor.g, bgColor.b, 1);
        else {
            // simple interpolation for now
            float v = (radius - luminocity) / radius;
            outputColor = vec4(bgColor.r + (color.r - bgColor.r) * v,
                               bgColor.g + (color.g - bgColor.g) * v,
                               bgColor.b + (color.b - bgColor.b) * v,
                               1);
        }
    }
}
