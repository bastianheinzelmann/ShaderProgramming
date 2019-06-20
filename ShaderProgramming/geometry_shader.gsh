#version 410 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 6) out;

in VS_OUT {
	vec2 texCoords;
} gs_in[];

out vec2 TexCoords;

void main() {

for(int i = 0; i < 3; i++)
{
    gl_Position = gl_in[i].gl_Position;
	TexCoords = gs_in[i].texCoords;
    EmitVertex();
}
  EndPrimitive();
}
