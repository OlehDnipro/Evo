

layout(push_constant) uniform pcb {
	layout(offset = 0) float4 ScaleBias;
};
layout(set = 0, binding = 0) uniform Texture2D Texture;
layout(set = 1, binding = 0) uniform SamplerState Filter;

struct PsIn
{
#ifdef GLSL
	vec2 TexCoord;
#else
	float4 Position: SV_Position;
	float2 TexCoord: TexCoord;
#endif
};

