#version 430

layout (points) in;
layout (triangle_strip, max_vertices = 4) out;
in flat float vertexID[];
out flat float vertexID_gs;
uniform mat4 ViewProjectionMatrix;
uniform vec3 CameraPos;
uniform vec4 MVP;

layout(std430, binding=4) buffer Size{
  float ParticleSize[];
};

out vec2 TexCoords;

void main(void)
{
	vec2 size = vec2(0.02);
	size = vec2(ParticleSize[int(vertexID[0])]);

	vec4 position = gl_in[0].gl_Position;

	vertexID_gs = vertexID[0];
	
	gl_Position = position + vec4(-size.x, -size.y, 0.0, 0.0);    // 1:bottom-left
	TexCoords = vec2(0,1);
    EmitVertex();   
    gl_Position = position + vec4( size.x, -size.y, 0.0, 0.0);    // 2:bottom-right
	TexCoords = vec2(1,1);
    EmitVertex();
    gl_Position = position + vec4(-size.x,  size.y, 0.0, 0.0);    // 3:top-left
	TexCoords = vec2(0,0);
    EmitVertex();
    gl_Position = position + vec4( size.x,  size.y, 0.0, 0.0);    // 4:top-right
	TexCoords = vec2(1,0);
    EmitVertex(); 
    EndPrimitive();

	/// Finally emit the whole square.
	EndPrimitive();
}