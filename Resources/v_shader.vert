#version 330

uniform mat4 projection;
uniform mat4 model;
out vec2 uv_coord;
layout (location =0) in vec2 position;
layout (location =1) in vec2 coord;
void main(){
    gl_Position=projection*model*vec4(position,0,1);
    uv_coord=coord;
}