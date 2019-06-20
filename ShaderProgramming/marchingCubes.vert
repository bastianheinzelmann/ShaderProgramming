#version 330 core

// INPUT
layout (location = 0) in vec2 aPos;				// uv
in int gl_InstanceID;						// instance ID

// OUTPUT
out struct VS_OUT {
	vec4 wsCoord;
	vec3 uvw;
	vec4 f0123;
	vec4 f4567;
	uint mc_case;
} vs_out;



uniform sampler3D screenTexture;	// our volume of density values. (+=rock, -=air) 


// TESTING
out float geoValue;
flat out int layerId;

out vec3 idk;

// vertex INPUT
// struct
// float2 uv
// int nInstance ID


// structvsOutputGsInput{  // per-vertex outputs: 
// float3 wsCoord: POSITION; // coordsfor LOWER-LEFT corner of the cell 
// float3 uvw: TEX;      
// float4 f0123: TEX1;     // the density values 
// float4 f4567: TEX2;     //   at the 8 cell corners 
// uintmc_case: TEX3;     // 0-255 };



float WorldSpaceVolumeHeight = 2.0f*(256.0f/96.0f);			// Volume Height
vec3 voxelDim = vec3(96.0f, 256.0f, 96.0f);					// Dimension of the whole cube
vec3 voxelDimMinusOne = vec3(95.0f, 256.0f, 95.0f);			// Dimension -1	->	actual number of voxels
float wsVoxelSize = 2.0f/95.0f;								// half	the voxel size?
vec4 inv_voxelDim = vec4( 1.0f/voxelDim, 0.0f);				// percent in 0 to 1
vec4 inv_voxelDimMinusOne = vec4( 1.0f/voxelDimMinusOne, 0.0f );	// percent in 0 to 1 but minus 1



// converts a point in world space to 3D texture space (for sampling the 3D texture): 
#define WS_to_UVW(ws) (vec3(ws.xz * 0.5f +0.5f, ws.y * WorldSpaceVolumeHeight).xzy)


layout(std140) uniform SliceInfos
{
	// Updated each frame.  To generate 5 slices this frame, 
	// app has to put their world-space Y coordsin slots [0..4] here. 
	float slice_world_space_Y_coord[256];
	float slice_world_space_Y_coord_above[256];

};



void main()
{

	//geoValue = textureSize(screenTexture, 1);
	//ivec3 size = textureSize(screenTexture​​,);
	//geoValue = aPos.x;

	// get world-space coordinates & UVW coordsof lower-left corner of this cell 

	int inst = gl_InstanceID;

	vec3 wsCoord; 
	wsCoord.xz = (aPos.xy * 2) -1; // get world space from whatever strange space we were before
	//wsCoord.y = (inst * 1.0f/256.0f *2.0f) -1; //(WorldSpaceVolumeHeight *  (inst/256))  - (256/96); //(inst * 1/256 *2)-1; // slice_world_space_Y_coord[inst]; can be from -1 to +1
	wsCoord.y =    inst * 1.0f/255.0f;


	//vec3 uvw= WS_to_UVW( wsCoord.xyz);

	vec3 uvw = vec3(
				wsCoord.xz * 0.5f + 0.5f, 
				wsCoord.y).xyz;

	// (wsCoord.y + 2.66)* 1/(2* 2.66);
	wsCoord.y =    (2.0f * (256.0f /96.0f ) * wsCoord.y) - (256.0f /96.0f );

	#define WS_to_UVW(ws) (vec3(ws.xz * 0.5f +0.5f, ws.y * WorldSpaceVolumeHeight).xzy)



	idk =  vec3(textureSize(screenTexture, 0));

	uint mc_case = 0u;

	// sample the 3D texture to get the density values at the 8 corners 
	vec4 steP = vec4(inv_voxelDimMinusOne.xzy, 0.0f); 


	vec4 f0123 = vec4(
		 texture(screenTexture, uvw + steP.www, 0.0f).x,
		 texture(screenTexture, uvw + steP.wwz, 0.0f).x,
		 texture(screenTexture, uvw + steP.xwz, 0.0f).x,
		 texture(screenTexture, uvw + steP.xww, 0.0f).x
	); 

	vec4 f4567 = vec4(  
		texture(screenTexture, uvw + steP.wyw, 0.0f).x,
		texture(screenTexture, uvw + steP.wyz, 0.0f).x,
		texture(screenTexture, uvw + steP.xyz, 0.0f).x,
		texture(screenTexture, uvw + steP.xyw, 0.0f).x
	);

	// determine which of the 256 marching cubes cases for this cell: 
	uvec4 n0123 = uvec4(clamp( f0123 * 99999, 0, 1));
	uvec4 n4567 = uvec4(clamp( f4567 * 99999, 0, 1));

	mc_case =  
		  (n0123.x     ) | (n4567.x << 4) 
		| (n0123.y << 1) | (n4567.y << 5) 
		| (n0123.z << 2) | (n4567.z << 6) 
		| (n0123.w << 3) | (n4567.w << 7);

	//v2fConnector v2f;
	vs_out.wsCoord = vec4(wsCoord, 0.0f); // inst * 1/256); //slice_world_space_Y_coord_above[inst]);
	vs_out.uvw = uvw.xyz;
	vs_out.f0123 = f0123;
	vs_out.f4567 = f4567;
	vs_out.mc_case = mc_case;

	geoValue = float(gl_InstanceID);


	// TESTING
	layerId = gl_InstanceID;
	//gl_Position = vec4(aPos.x, aPos.y, 0.0f, 1.0f);
	gl_Position = vec4(wsCoord.xz, 0.0f, 1.0f);
}