//#version 430
//
//in vec3 Position;
//
//uniform vec4 Color;
//
//layout( location = 0 ) out vec4 FragColor;
//
//void main() {
//  FragColor = Color;
//}

#version 430

uniform sampler2D tex0;
in vec2 TexCoords;
out vec4 FragColor;
uniform vec4 Color;
uniform float DeltaT;

in flat float vertexID_gs;

layout(std430, binding=3) buffer Type{
  float ParticleType[];
};

// A single iteration of Bob Jenkins' One-At-A-Time hashing algorithm.
uint hash( uint x ) {
    x += ( x << 10u );
    x ^= ( x >>  6u );
    x += ( x <<  3u );
    x ^= ( x >> 11u );
    x += ( x << 15u );
    return x;
}

// Compound versions of the hashing algorithm I whipped together.
uint hash( uvec2 v ) { return hash( v.x ^ hash(v.y)                         ); }
uint hash( uvec3 v ) { return hash( v.x ^ hash(v.y) ^ hash(v.z)             ); }
uint hash( uvec4 v ) { return hash( v.x ^ hash(v.y) ^ hash(v.z) ^ hash(v.w) ); }

// Construct a float with half-open range [0:1] using low 23 bits.
// All zeroes yields 0.0, all ones yields the next smallest representable value below 1.0.
float floatConstruct( uint m ) {
    const uint ieeeMantissa = 0x007FFFFFu; // binary32 mantissa bitmask
    const uint ieeeOne      = 0x3F800000u; // 1.0 in IEEE binary32

    m &= ieeeMantissa;                     // Keep only mantissa bits (fractional part)
    m |= ieeeOne;                          // Add fractional part to 1.0

    float  f = uintBitsToFloat( m );       // Range [1:2]
    return f - 1.0;                        // Range [0:1]
}

float random( float x ) { return floatConstruct(hash(floatBitsToUint(x))); }
float random( vec2  v ) { return floatConstruct(hash(floatBitsToUint(v))); }
float random( vec3  v ) { return floatConstruct(hash(floatBitsToUint(v))); }
float random( vec4  v ) { return floatConstruct(hash(floatBitsToUint(v))); }


float circle()
{
	float a = sqrt((0.5 - TexCoords.x)*(0.5 - TexCoords.x) + (0.5 - TexCoords.y)*(0.5 - TexCoords.y));
	a = 1 - (a / 0.5);
	return a;
}

float column()
{
	if(TexCoords.x > 0.45 && TexCoords.x < 0.5)
		return 1;
	else
		return 0;
}

void main(void)
{
	vec4 texColor = texture(tex0, TexCoords);
	
	vec4 finalColor;
	vec4 initCol = vec4(0.0, 0.0, 0.0, 1.0);
	float type = ParticleType[int(vertexID_gs)];
	if(type == 1)
	{
		discard;
	}
	else if(type == 2)
	{
		finalColor = texColor * vec4(0.0, 1.0, 0.0, 1.0);
	}
	else if(type == 4)
	{
		finalColor = vec4(1.0, 1.0, 1.0, circle());
	}
	else if(type == 3)
	{
		finalColor = vec4(0.0, 0.0, 1.0, column());
	}

//	FragColor = Color;
//	FragColor = vec4(gl_PointCoord.s, gl_PointCoord.t, 0.0, 1.0);
	FragColor = finalColor;
}