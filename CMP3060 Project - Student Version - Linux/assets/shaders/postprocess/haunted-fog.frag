#version 330

uniform sampler2D tex;
// The player's stealth level (0.0 = low stealth, 1.0 = high stealth)
uniform float stealthLevel;
uniform float time;

in vec2 tex_coord;
out vec4 frag_color;

// Noise function for distortion
float noise(vec2 p) {
    return fract(sin(dot(p, vec2(127.1, 311.7))) * 43758.5453);
}

void main() {
    // Map stealth level to effect intensity (low stealth = stronger effect)
    float intensity = 1.0 - stealthLevel; // 0.0 = minimal effect, 1.0 = max effect

    // Compute distortion using noise
    float distortionStrength = 0.01 * intensity; // Subtle distortion, stronger when stealth is low
    vec2 distortedCoord = tex_coord + vec2(
        noise(tex_coord + time * 0.1) * distortionStrength,
        noise(tex_coord + time * 0.2 + vec2(1.0)) * distortionStrength
    );

    vec4 sceneColor = texture(tex, distortedCoord);

    // Apply fog effect
    float fogStrength = 0.5 * intensity; 
    vec3 fogColor = vec3(0.1, 0.15, 0.2); 
    vec3 color = mix(sceneColor.rgb, fogColor, fogStrength);

    float gray = dot(color, vec3(0.299, 0.587, 0.114));
    color = mix(color, vec3(gray), 0.3 * intensity); 

    frag_color = vec4(color, sceneColor.a);
}