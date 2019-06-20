#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 TexCoordTest;

// texture samplers
uniform sampler2D texture1;
uniform sampler2D texture2;
//uniform sampler3D texture3;

uniform sampler2DArray texture4;


void main()
{
	float actualLayer = max(0.0f, min(4.0f - 1.0f, floor(1+ 0.5f)));
	lowp float a = texture(texture4, vec3(0.4f, 0.6f, 2)).r;
	// linearly interpolate between both textures (80% container, 20% awesomeface)
	FragColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), a);
}