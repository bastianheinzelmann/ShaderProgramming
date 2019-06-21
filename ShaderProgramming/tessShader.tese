#version 430 core
layout (triangles, equal_spacing, cw) in;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform sampler2D heightMap;

out vec2 texCoords;

void main(void){ 
	vec4 newVertex = (gl_TessCoord.x*gl_in[0].gl_Position+gl_TessCoord.y*gl_in[1].gl_Position+gl_TessCoord.z*gl_in[2].gl_Position);
	texCoords = (newVertex.xz + 1) * 0.5;
	vec3 height = texture(heightMap, texCoords).rgb;
	newVertex.y += height.g * 0.3;
	gl_Position= projection * view * model * newVertex;
}