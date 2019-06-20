#version 330 core
layout (location = 0) in vec3 aPos;

varying vec4 v_position;

uniform mat4 lightSpaceMatrix;
uniform mat4 model;

void main()
{
    gl_Position = lightSpaceMatrix * model * vec4(aPos, 1.0);
	v_position = gl_Position;
}