#version 150

// special transparency based on the mask. see expansion

// these are our textures
uniform sampler2DRect tex0;
uniform sampler2DRect mask;
uniform int radius;

// this comes from the vertex shader
in vec2 texCoordVarying;

// this is the output of the fragment shader
out vec4 outputColor;

vec3 hsv2rgb(vec3 c) {
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

vec3 rgb2hsv(vec3 c)
{
    vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
    vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
    vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));
    
    float d = q.x - min(q.w, q.y);
    float e = 1.0e-10;
    return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
}

void main()
{
    vec4 maskColor = texture(mask, texCoordVarying).rgba;
    vec4 color = texture(tex0, texCoordVarying).rgba;

    if (maskColor.b == 1)
        outputColor = color;
    else {
        float luminocity = maskColor.r * 255 * 256 + maskColor.g * 255;
        if (luminocity > radius)
            outputColor = vec4(color.r, color.g, color.b, 0);
        else {
            // calculate wave effect
            float v = 1 - (radius - luminocity) / radius;
            //float waveEffect = sin(v * 3.14) * (1 - v);
            float waveEffect = cos((v - 0.5) * 2 * 3.14) / 2 + 0.5;
            
            vec3 hsv = rgb2hsv(vec3(color.r, color.g, color.b));
            //hsv.b /= 1 + 2 * waveEffect;
            hsv.b -= waveEffect;
            vec3 rgb = hsv2rgb(hsv);
            
            outputColor = vec4(rgb.r, rgb.g, rgb.b, 1 - v);
        }
    }
}
