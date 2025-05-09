#version 330

// This vertex shader should be used to render a triangle whose normalized device coordinates are:
// (-0.5, -0.5, 0.0), ( 0.5, -0.5, 0.0), ( 0.0,  0.5, 0.0)
// And it also should send the vertex color as a varying to the fragment shader where the colors are (in order):
// (1.0, 0.0, 0.0), (0.0, 1.0, 0.0), (0.0, 0.0, 1.0)

out Varyings{
    vec3 color;
}vs_out;

// Currently, the triangle is always in the same position, but we don't want that.
// So two uniforms should be added: translation (vec2) and scale (vec2).
// Each vertex "v" should be transformed to be "scale * v + translation".
// The default value for "translation" is (0.0, 0.0) and for "scale" is (1.0, 1.0).

//TODO: (Req 1) Finish this shader
// Vertex positions in NDC space
const vec3 positions[3]=vec3[](
    vec3(-.5,-.5,0.),// First vertex
    vec3(.5,-.5,0.),// Second vertex
    vec3(0.,.5,0.)// Third vertex
);

// Vertex colors
const vec3 colors[3]=vec3[](
    vec3(1.,0.,0.),// Red
    vec3(0.,1.,0.),// Green
    vec3(0.,0.,1.)// Blue
);
uniform vec2 translation=vec2(0.,0.);
uniform vec2 scale=vec2(1.,1.);
void main(){
    vec3 position=positions[gl_VertexID];

    position.xy=scale*position.xy+translation;

    gl_Position=vec4(position,1.);
    
    vs_out.color=colors[gl_VertexID];
}