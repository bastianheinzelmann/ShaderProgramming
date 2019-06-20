#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
} fs_in;

uniform sampler2D diffuseTexture;
uniform sampler2D shadowMap;

uniform vec3 lightPos;
uniform vec3 viewPos;

float ShadowCalculation(vec4 fragPosLightSpace)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    float closestDepth = texture(shadowMap, projCoords.xy).r;
    float currentDepth = projCoords.z;
    vec3 normal = normalize(fs_in.Normal);
    vec3 lightDir = normalize(lightPos - fs_in.FragPos);
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
    
	float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;
    if(projCoords.z > 1.0)
        shadow = 0.0;
        
    return shadow;
}

float cheby(vec4 ShadowCoord)
{
    vec3 projCoords = ShadowCoord.xyz / ShadowCoord.w;
    projCoords = projCoords * 0.5 + 0.5;

    // We retrive the two moments previously stored (depth and depth*depth)
    vec2 moments = texture(shadowMap, projCoords.xy).rg; 

    float currentDepth = projCoords.z;
    float variance = max(0.0001f,(moments.y - moments.x * moments.x));
    float distanc = max(0, currentDepth - moments.x);
    float lightProbMax = variance /(variance+(distanc * distanc));


    vec3 lightDir = normalize(lightPos - fs_in.FragPos);
    vec3 normal = normalize(fs_in.Normal);
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
    if(currentDepth + bias > 1)
        lightProbMax = 1.0f;

    return lightProbMax;
}

void main()
{           
    vec3 color = texture(diffuseTexture, fs_in.TexCoords).rgb;
    vec3 normal = normalize(fs_in.Normal);
    vec3 lightColor = vec3(0.3);
    // ambient
    vec3 ambient = 0.3 * color;
    // diffuse
    vec3 lightDir = normalize(lightPos - fs_in.FragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * lightColor;
    // specular
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
    vec3 specular = spec * lightColor;    
    // calculate shadow
    //float shadow = ShadowCalculation(fs_in.FragPosLightSpace); 
	float shadow = cheby(fs_in.FragPosLightSpace);
    vec3 lighting = (ambient + (shadow) * (diffuse + specular)) * color;    
    
    FragColor = vec4(lighting, 1.0);
}