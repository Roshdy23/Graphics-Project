#version 330 core

out vec4 frag_color;

// In this shader, we want to draw a checkboard where the size of each tile is (size x size).
// The color of the top-left most tile should be "colors[0]" and the 2 tiles adjacent to it
// should have the color "colors[1]".

//TODO: (Req 1) Finish this shader.

uniform int size = 32;
uniform vec3 colors[2];

void main(){
    ivec2 pixel_pos = ivec2(gl_FragCoord.xy);

    // Determine which tile this pixel belongs to
    int tile_x = (pixel_pos.x / size) % 2;
    int tile_y = (pixel_pos.y / size) % 2;

    // Determine which color to use (checkerboard pattern)
    int color_index = (tile_x + tile_y) % 2;

    // Set the fragment color
    frag_color = vec4(colors[color_index], 1.0);
    // frag_color = vec4(colors[0], 1.0);
}