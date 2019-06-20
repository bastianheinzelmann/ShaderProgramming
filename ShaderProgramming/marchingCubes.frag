#version 330 core
out vec4 FragColor;

flat in int layerID;

in float outValue;

uniform sampler3D screenTexture;
uniform sampler2D diffuseMap;
uniform sampler2D normalMap;
uniform sampler2D heightMap;

//in struct VS_OUT {
//	vec3 wsCoord;
//	vec3 uvw;
//	vec4 f0123;
//	vec4 f4567;
//	int mc_case;
//} vs_out;

in GS_OUT {
	vec4 wsCoord_Ambo;
	//vec3 wsNormal;
	vec3 uvw;
} fs_in;

in FRAG_IN
{
	vec3 position;
	vec2 texCoords;
	vec3 normal;
	vec3 tangentLightPos;
	vec3 tangentViewPos;
	vec3 tangentFragPos;
	vec3 uvw;
} frag_in;

float height_scale = 0.05;

//vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir)
//{ 
//    float height =  texture(heightMap, texCoords).r;    
//    vec2 p = viewDir.xy / viewDir.z * (height * height_scale);
//    return texCoords - p;    
//} 

vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir)
{ 
    // number of depth layers
    const float minLayers = 16;
    const float maxLayers = 64;
    float numLayers = mix(maxLayers, minLayers, abs(dot(vec3(0.0, 0.0, 1.0), viewDir)));
	numLayers = 32;
    // calculate the size of each layer
    float layerDepth = 1.0 / numLayers;
    // depth of current layer
    float currentLayerDepth = 0.0;
    // the amount to shift the texture coordinates per layer (from vector P)
    vec2 P = viewDir.xy / viewDir.z * height_scale; 
    vec2 deltaTexCoords = P / numLayers;
  
    // get initial values
    vec2  currentTexCoords     = texCoords;
    float currentDepthMapValue = texture(heightMap, currentTexCoords).r;
      
    while(currentLayerDepth < currentDepthMapValue)
    {
        // shift texture coordinates along direction of P
        currentTexCoords -= deltaTexCoords;
        // get depthmap value at current texture coordinates
        currentDepthMapValue = texture(heightMap, currentTexCoords).r;  
        // get depth of next layer
        currentLayerDepth += layerDepth;
		//currentLayerDepth += 100;
    }
    
    // get texture coordinates before collision (reverse operations) go one step back + deltaTexCoords
    vec2 prevTexCoords = currentTexCoords + deltaTexCoords;

    // get depth after and before collision for linear interpolation
    float afterDepth  = currentDepthMapValue - currentLayerDepth;
	// + layerDepth again we need to go a step back
    float beforeDepth = texture(heightMap, prevTexCoords).r - currentLayerDepth + layerDepth;
 
    // interpolation of texture coordinates
    float weight = afterDepth / (afterDepth - beforeDepth);
	
    vec2 finalTexCoords = prevTexCoords * weight + currentTexCoords * (1.0 - weight);

    return finalTexCoords;
}

void main()
{	
	vec3 viewDir = normalize(frag_in.tangentViewPos - frag_in.tangentFragPos);
	vec2 texCoords = frag_in.texCoords;

	texCoords = ParallaxMapping(frag_in.position.xy, viewDir);
	//if(texCoords.x > 1.0 || texCoords.y > 1.0 || texCoords.x < 0.0 || texCoords.y < 0.0) discard;

    vec3 normal = texture(normalMap, texCoords).rgb;
    normal = normalize(normal * 2.0 - 1.0);   
   
    vec3 color = texture(diffuseMap, texCoords).rgb;
	vec3 ambient = 0.1 * color;
    // diffuse
    vec3 lightDir = normalize(frag_in.tangentLightPos - frag_in.tangentFragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * color;
    // specular    
    vec3 reflectDir = reflect(-lightDir, normal);
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);

    vec3 specular = vec3(0.2) * spec;
    FragColor = vec4(ambient + diffuse + specular, 1.0f);
}