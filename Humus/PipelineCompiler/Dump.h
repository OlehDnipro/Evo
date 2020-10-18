
SamplerState ColorSampler : register(s0);
SamplerComparisonState  ShadowSampler : register(s1);
cbuffer cbperFrame : register(b0) { SPerFrame perFrame; }
cbuffer cbperModel : register(b1) { SPerModel perModel; }
Texture2D Texture : register(t0);
Texture2DArray ShadowMapCascades : register(t1);

