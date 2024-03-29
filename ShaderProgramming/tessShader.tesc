#version 430 core
layout (vertices = 3) out;

uniform vec4 tessFactor;

void main(void){

    if (gl_InvocationID == 0){
        gl_TessLevelInner[0] = tessFactor.x;
        gl_TessLevelOuter[0] = 1.0;
        gl_TessLevelOuter[1] = 1.0;
        gl_TessLevelOuter[2] = 1.0;
    } 

    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
}