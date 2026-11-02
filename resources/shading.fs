#version 330

// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord;
in vec4 fragColor;
in vec3 fragNormal;

// Input uniform values
uniform sampler2D texture0;
uniform vec4 colDiffuse;

// Brightness per face direction: x = X sides, y = top, z = Z sides, w = bottom
uniform vec4 faceShade;

// Output fragment color
out vec4 finalColor;

void main()
{
    vec4 texelColor = texture(texture0, fragTexCoord);
    vec4 tint = colDiffuse*fragColor;

    // Squared normal gives positive per-axis weights that sum to 1,
    // so opposite faces (+X/-X, +Z/-Z) always get the same shade
    vec3 n = normalize(fragNormal);
    vec3 w = n*n;
    float vertical = n.y > 0.0 ? faceShade.y : faceShade.w;
    float shade = w.x*faceShade.x + w.y*vertical + w.z*faceShade.z;

    finalColor = vec4(texelColor.rgb*tint.rgb*shade, texelColor.a*tint.a);
}
