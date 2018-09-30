#version 150

uniform sampler2DRect tex0;
uniform float expansionRadius;

in vec2 texCoordVarying;
out vec4 outputColor;

void main()
{
    float currentColor = texture(tex0, texCoordVarying).rgb;
    
    if (currentColor.r == 1) { // nothing to do, already covered
        outputColor = vec4(1, 1, 1, 1);
    }
    else {
        int radiusCeil = int(ceil(expansionRadius - 0.5)); // 0.5 - center of the pixel
        
        float additionalColor = 0; // how much to add to the current pixel
        
        for (int x = -radiusCeil; x <= radiusCeil; x++) {
            for (int y = -radiusCeil; y <= radiusCeil; y++) {
                if (x == 0 && y == 0) continue; // skip the current pixel
                
                vec3 color = texture(tex0, texCoordVarying + vec2(x, y)).rgb;
                if (color.r < 1) continue; // skip if the pixel is not covered in color completely yet
                
                int absX = abs(x);
                int andY = abs(y);
                
                // simple cases. on the axis
                if (absX == 0) {
                    additionalColor = max(additionalColor, expansionRadius - absY + 0.5);
                }
                else if (absY == 0) {
                    additionalColor = max(additionalColor, expansionRadius - absX + 0.5);
                }
                else {
                    float distance = sqrt(x * x + y * y);
                    if (absX >= absY) {
                        additionalColor = max(additionalColor, expansionRadius * absX / distance - absX + 0.5);
                    }
                    else {
                        additionalColor = max(additionalColor, expansionRadius * absY / distance - absY + 0.5);
                    }
                }
                
                if (additionalColor + currentColor.r >= 1)
                    break;
            }
            if (additionalColor + currentColor.r >= 1) // covered completely, can break the loop
                break;
        }
        
        additionalColor = min(1, additionalColor + currentColor.r); // if brighter than 1
        outputColor = vec4(additionalColor, additionalColor, additionalColor, additionalColor);
    }
}
