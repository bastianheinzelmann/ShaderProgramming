#version 330 core
out vec4 FragColor;

uniform sampler2D diffuseTexture;
uniform sampler2D shadowMap;

uniform vec3 lightPos;
uniform vec3 viewPos;

in vec2 texCoords;

void main()
{   

    vec3 color = texture(diffuseTexture, texCoords).rgb;

	//FragColor = vec4(1.0, 0.0, 0.0, 1.0);
	FragColor = vec4(color, 1.0);
}