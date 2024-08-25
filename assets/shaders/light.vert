#version 300 es

precision highp float;

layout (location = 0) in vec3 a_vertex;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 a_texture;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec4 color;
uniform vec2 origin;
uniform float attenuation;
uniform float attenuationSq;

out vec2 TexCoord;
out vec3 Normal;
out vec3 FragPos;
out vec4 Color;
out vec2 Origin;
out vec2 LightPos;
out float Attenuation;
out float AttenuationSq;

void main() {
    gl_Position = projection * view * model * vec4(a_vertex, 1.0);
    Origin = (projection * view * model * vec4(origin.xy, 0.0, 1.0)).xy;
    LightPos = a_vertex.xy;
    Normal = a_normal;
    FragPos = vec3(gl_Position);
    Color = color;
    TexCoord = a_texture;
    Attenuation = attenuation;
    AttenuationSq = attenuationSq;
}