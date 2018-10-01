#version 150

// expand area covered by 1 to expansionRadius in all directions

uniform sampler2DRect tex0;
uniform float expansionRadius; // in pixels

in vec2 texCoordVarying;
out vec4 outputColor;

// bytes
// r - current luminosity / 256
// g - current luminocity % 256
// b - processed pixel if 1
// a - mask pixels

void main()
{
    vec4 currentColor = texture(tex0, texCoordVarying).rgba;
    
    if (currentColor.b == 1)
        outputColor = currentColor;
    else {
        int currentLuminocity = int(currentColor.r * 255 * 256 + currentColor.g * 255);
        
        currentLuminocity -= int(expansionRadius);
        
        if (currentLuminocity <= 0) {
            outputColor = vec4(1, 1, 1, 1);
        }
        else {
            outputColor = vec4((currentLuminocity / 256) / 255.0,
                               (currentLuminocity % 256) / 255.0,
                               0,
                               1);
        }
    }
}
