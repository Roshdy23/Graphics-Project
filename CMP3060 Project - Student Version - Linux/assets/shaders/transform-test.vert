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

void main(){
    // (Req 3) Apply the transformation matrix to the position
    gl_Position = transform * vec4(position, 1.0);
    
    // No need to change any of the following lines
    vs_out.position = (transform * vec4(position, 1.0)).xyz;
    vs_out.color = color;
    vs_out.tex_coord = tex_coord;
    vs_out.normal = mat3(transform) * normal; // Apply rotation part of the transform to the normal
}
