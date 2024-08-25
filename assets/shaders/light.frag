#version 300 es

precision highp float;

in vec2 TexCoord;
in vec3 FragPos;
in vec4 Color;
in vec2 Origin;
in vec2 LightPos;
in float Attenuation;
in float AttenuationSq;

out vec4 FragColor;

void main() {

    vec2 coord = TexCoord * 2.0 - vec2(1.0);

    float d = length(coord);
    float light = clamp(1.0 / (1.0 + Attenuation * d + AttenuationSq * d * d), 0.0, 1.0);

    if (d < 1.0) {
        FragColor = vec4(Color.xyz, light);
    }
}