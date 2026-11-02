#version 330

// Input vertex attributes (from vertex shader)
in vec3 fragPosition;
in vec2 fragTexCoord;
in vec4 fragColor;
in vec3 fragNormal;

// Input uniform values
uniform sampler2D texture0;
uniform vec4 colDiffuse;

// Brightness per face direction
uniform vec3 shadePos; // +X (back), +Y (top),    +Z (right)
uniform vec3 shadeNeg; // -X (front), -Y (bottom), -Z (left)

// Directional light shadows
uniform vec3 lightDir;         // Direction the light travels (normalized)
uniform mat4 lightVP;          // World -> light clip space
uniform sampler2D shadowMap;   // Depth seen from the light
uniform float shadowStrength;  // Brightness kept inside a shadow

// Output fragment color
out vec4 finalColor;

float ShadowFactor(vec3 n)
{
    // Faces turned away from the light keep their own face shade
    float facing = dot(n, -lightDir);
    if (facing <= 0.0) return 1.0;

    // Push the sample off the surface to avoid shadow acne
    vec4 lightPos = lightVP*vec4(fragPosition + n*0.02, 1.0);
    vec3 uvz = lightPos.xyz/lightPos.w*0.5 + 0.5;
    if (any(lessThan(uvz, vec3(0.0))) || any(greaterThan(uvz, vec3(1.0)))) return 1.0;

    // 3x3 PCF for soft edges
    vec2 texel = 1.0/vec2(textureSize(shadowMap, 0));
    float shadowed = 0.0;
    for (int x = -1; x <= 1; x++)
    for (int y = -1; y <= 1; y++)
    {
        float depth = texture(shadowMap, uvz.xy + vec2(x, y)*texel).r;
        shadowed += (uvz.z - 0.001 > depth) ? 1.0 : 0.0;
    }

    return mix(1.0, shadowStrength, shadowed/9.0);
}

void main()
{
    vec4 texelColor = texture(texture0, fragTexCoord);
    vec4 tint = colDiffuse*fragColor;

    // Squared normal gives positive per-axis weights that sum to 1,
    // the sign of each axis picks the + or - side brightness
    vec3 n = normalize(fragNormal);
    vec3 w = n*n;
    vec3 side = mix(shadeNeg, shadePos, step(0.0, n));
    float shade = dot(w, side)*ShadowFactor(n);

    finalColor = vec4(texelColor.rgb*tint.rgb*shade, texelColor.a*tint.a);
}
