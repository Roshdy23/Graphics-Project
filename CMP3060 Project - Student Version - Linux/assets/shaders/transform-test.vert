#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec4 color;
layout(location = 2) in vec2 tex_coord;
layout(location = 3) in vec3 normal;

out Varyings {
    vec3 position;
    vec4 color;
    vec2 tex_coord;
    vec3 normal;
} vs_out;

uniform mat4 transform; 

void main() {
    gl_Position = transform * vec4(position, 1.0);
        vs_out.position = (transform * vec4(position, 1.0)).xyz;
vs_out.normal = normalize(mat3(transpose(inverse(transform))) * normal);
    
    vs_out.color = color;
    vs_out.tex_coord = tex_coord;
}
