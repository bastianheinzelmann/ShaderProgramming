#version 430

layout (location = 0) in vec4 VertexPosition;

out flat float vertexID;

uniform mat4 ModelViewMatrix;
uniform mat3 NormalMatrix;
uniform mat4 MVP;

void main()
{
	vertexID = gl_VertexID;
    // Position = (ModelViewMatrix * VertexPosition).xyz;
    gl_Position = MVP * VertexPosition;
	// gl_Position = vec4(VertexPosition);
}