

struct SConstants
{
	float4x4 WVP;
};

cbuffer cbColor : register(b0) { float4 Color; }
cbuffer cbConstants : register(b1) { SConstants Constants; }
Texture2D Texture : register(t0);
SamplerState Filter : register(s0);

struct PS_IN
{
#ifdef GLSL
	vec2 Tex;
#else
	float4 Pos : SV_Position;
	float2 Tex : TexCoord;
#endif
};

