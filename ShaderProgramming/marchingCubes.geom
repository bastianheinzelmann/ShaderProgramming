#version 420 core

layout(points) in;						// INPUT TYPE
//layout(points, max_vertices = 15) out;	// OUTPUT TYPE		TESTING

// INPUT TYPE
layout(triangle_strip, max_vertices = 15) out;



// TESTING
in float[] geoValue;
//out float testValue;
out float outValue;

flat in int[] layerId;

flat out int layerID;

uniform mat4 model;
uniform mat4 projection;
uniform mat4 view;
uniform vec3 viewPos;
uniform vec3 lightPos;

uniform int yOffset;

// GEOMETRY SHADER INPUT
in struct VS_OUT {
	vec4 wsCoord;	//coords are for the LOWER-LEFT corner of the cell.
	vec3 uvw;		//coords are for the LOWER-LEFT corner of the cell.
	vec4 f0123;
	vec4 f4567;
	uint mc_case;
} vs_out[];

// GEOMETRY SHADER OUTPUT
out struct GS_OUT
{
//	vec3 position;
//	vec2 texCoords;
//	vec3 normal;
//	vec3 TangentViewPos;
//	vec3 TangentFragPod;
	vec4 wsCoord_Ambo;
	//vec3 wsNormal;
	vec3 uvw;
} gs_out;

out FRAG_IN
{
	vec3 position;
	vec2 texCoords;
	vec3 normal;
	vec3 tangentLightPos;
	vec3 tangentViewPos;
	vec3 tangentFragPos;
	vec3 uvw;
}frag_in;

layout (std140) uniform g_mc_lut { 
	//uint case_to_numpolys[256];
	uint case_to_numpolys[256];
	vec4 cornerAmask0123[12];
	//float abc[24];
	vec4 cornerAmask4567[12]; 
	vec4 cornerBmask0123[12]; 
	vec4 cornerBmask4567[12]; 
	vec3 vec_start[12]; 
	vec3 vec_dir[12]; 
};

layout (std140, binding = 1) uniform g_mc_lut2 { 
	ivec4 g_triTable[1280]; //5*256 
};



// our volume of density values
uniform sampler3D screenTexture;


float WorldSpaceVolumeHeight= 2.0f*(256.0f/96.0f); 
vec3 voxelDim = vec3(96.0f, 256.0f, 96.0f); 
vec3 voxelDimMinusOne = vec3(95.0f, 256.0f, 95.0f); 
float wsVoxelSize = 2.0f/95.0f; 
vec4 inv_voxelDim = vec4( 1.0f/voxelDim, 0.0f);
vec4 inv_voxelDimMinusOne = vec4( 1.0f/voxelDimMinusOne, 0.0f );

vec3 CalculateNormal(vec3 p1, vec3 p2, vec3 p3)
{
	vec3 normal;

	vec3 u = p2 - p1;
	vec3 v = p3 - p1;

	normal.x = (u.y * v.z) - (u.z * v.y);
	normal.y = (u.z * v.x) - (u.x * v.z);
	normal.z = (u.x * v.y) - (u.y * v.x);

	return normalize(normal);
}

vec2 CalculateUV(vec3 p)
{	
	return vec2(mod(p.x,1),mod(p.y,1));
}

vec3 CalculateTangent(vec3 p1, vec3 p2, vec3 p3)
{
	vec3 tangent;
	
	vec3 edge1 = p2 - p1;
	vec3 edge2 = p3 - p1;
	
	vec2 uv1 = p1.xy; //CalculateUV(p1);
	vec2 uv2 = p2.xy; //CalculateUV(p2);
	vec2 uv3 = p3.xy; //CalculateUV(p3);
    vec2 deltaUV1 = uv2 - uv1;
    vec2 deltaUV2 = uv3 - uv1;

    float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

    tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
    tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
    tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
    tangent = normalize(tangent);
	
	return tangent;
}
vec3 CalculateBiTangent(vec3 p1, vec3 p2, vec3 p3)
{
	vec3 biTangent;
	
	vec3 edge1 = p2 - p1;
	vec3 edge2 = p3 - p1;
	
	vec2 uv1 = p1.xy; //CalculateUV(p1);
	vec2 uv2 = p2.xy; //CalculateUV(p2);
	vec2 uv3 = p3.xy; //CalculateUV(p3);
    vec2 deltaUV1 = uv2 - uv1;
    vec2 deltaUV2 = uv3 - uv1;

    float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

	biTangent.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
    biTangent.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
    biTangent.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
    biTangent = normalize(biTangent);
	
	return biTangent;
}


vec3 ComputeNormal(sampler3D tex, vec3 uvw) 
{ 
    vec4 step = vec4(inv_voxelDim); 


    vec3 gradient = vec3( 
        texture(tex, uvw + step.xww, 0.0f).x -texture(tex, uvw - step.xww, 0.0f).x, 
        texture(tex, uvw + step.wwy, 0.0f).x -texture(tex, uvw - step.wwy, 0.0f).x, 
        texture(tex, uvw + step.wzw, 0.0f).x -texture(tex, uvw-  step.wzw, 0.0f).x 
    ); 

    return normalize(-gradient); 
}

GS_OUT PlaceVertOnEdge(VS_OUT input, int edgeNum)
{ 

	GS_OUT output;
	// Along this cell edge, where does the density value hit zero? 
	float str0= dot(cornerAmask0123[edgeNum], input.f0123) + 
				dot(cornerAmask4567[edgeNum], input.f4567);
	
	float str1= dot(cornerBmask0123[edgeNum], input.f0123) + 
				dot(cornerBmask4567[edgeNum], input.f4567); 
	
	float t = clamp( str0/(str0 - str1), 0.0f, 1.0f ); //saturate( str0/(str0 -str1) );  //0..1

	// use that to get wsCoord and uvw coords 
	vec3 pos_within_cell = vec_start[edgeNum] + t * vec_dir[edgeNum]; //[0..1] 
	


	vec3 wsCoord = input.wsCoord.xyz + pos_within_cell.xyz * wsVoxelSize; 
	// 256 * (2.0f/95.0f)
	wsCoord.y += (256.0f * (2.0f / 95.0f)) * yOffset;
	
	vec3 uvw = input.uvw + pos_within_cell * inv_voxelDimMinusOne.xzy;
	
	vec4 grad_ambo = texture(screenTexture, uvw, 0.0f);


	output.wsCoord_Ambo.xyz = wsCoord; 
	output.wsCoord_Ambo.w = texture(screenTexture, uvw, 0.0f).w; //grad_ambo_tex.SampleLevel(s, uvw, 0).w; 
	//	[!]	 output.wsNormal = ComputeNormal(tex, s, uvw); 
	
	return output;
}


void main() {


	uint cube_case = vs_out[0].mc_case;


	//GS_OUT output; 
	//uint num_polys = case_to_numpolys[vs_out[0].mc_case];
	uint num_polys = case_to_numpolys[vs_out[0].mc_case];
	uint table_pos = cube_case * 5; 


		// TESTING
	// -------
	for(int i = 0; i < num_polys; i++)
	{
		ivec4 polydata = g_triTable[ table_pos++];

		vec3 pos1 = PlaceVertOnEdge(vs_out[0], polydata.x).wsCoord_Ambo.xyz * 5;
		vec3 pos2 = PlaceVertOnEdge(vs_out[0], polydata.y).wsCoord_Ambo.xyz * 5;
		vec3 pos3 = PlaceVertOnEdge(vs_out[0], polydata.z).wsCoord_Ambo.xyz * 5;

		vec3 tangent = CalculateTangent(pos1,pos2,pos3);
		vec3 normal = ComputeNormal(screenTexture, vs_out[0].uvw);
		vec3 bitangent = CalculateBiTangent(pos1,pos2,pos3);

		vec3 T   = normalize(mat3(model) * tangent);
		vec3 B   = normalize(mat3(model) * bitangent);
		vec3 N   = normalize(mat3(model) * normal);
		mat3 TBN = transpose(mat3(T, B, N));

		layerID = layerId[0];

		gl_Position = projection * view * vec4(pos1,  1.0f);
		frag_in.position = pos1;
		frag_in.texCoords = CalculateUV(pos1);
		frag_in.normal = normal;
		frag_in.tangentLightPos = TBN * lightPos;
		frag_in.tangentViewPos  = TBN * viewPos;
		frag_in.tangentFragPos  = TBN * pos1;
		frag_in.uvw = vs_out[0].uvw;
		EmitVertex();

		gl_Position = projection * view * vec4(pos2,  1.0f);
		frag_in.position = pos2;
		frag_in.texCoords = CalculateUV(pos1);
		frag_in.normal = normal;
		frag_in.tangentLightPos = TBN * lightPos;
		frag_in.tangentViewPos  = TBN * viewPos;
		frag_in.tangentFragPos  = TBN * pos2;
		frag_in.uvw = vs_out[0].uvw;
		EmitVertex();

		gl_Position = projection * view * vec4(pos3, 1.0f);
		frag_in.position = pos3;
		frag_in.texCoords = CalculateUV(pos1);
		frag_in.normal = normal;
		frag_in.tangentLightPos = TBN * lightPos;
		frag_in.tangentViewPos  = TBN * viewPos;
		frag_in.tangentFragPos  = TBN * pos3;
		frag_in.uvw = vs_out[0].uvw;
		EmitVertex();

//		outValue = vs_out[0].wsCoord.y;
//		layerID = layerId[0];
//		gl_Position = projection * view * model * vec4(PlaceVertOnEdge(vs_out[0], polydata.x).wsCoord_Ambo.xyz, 1.0f);
//		EmitVertex();
//
//		gl_Position = projection * view * model * vec4(PlaceVertOnEdge(vs_out[0], polydata.y).wsCoord_Ambo.xyz, 1.0f);
//		EmitVertex();
//
//		gl_Position = projection * view * model * vec4(PlaceVertOnEdge(vs_out[0], polydata.z).wsCoord_Ambo.xyz, 1.0f);
//		EmitVertex();
//		EndPrimitive();
	}

	gs_out.uvw = vs_out[0].uvw;
	EmitVertex();
	EndPrimitive();
}