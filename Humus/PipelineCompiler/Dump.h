
SamplerState ColorSampler : register(s0);
SamplerComparisonState  ShadowSampler : register(s1);
cbuffer cbViewportConst : register(b0) { SPerFrame ViewportConst; }
cbuffer cbShadowConst : register(b1) { SShadow ShadowConst; }
Texture2DArray ShadowMapCascades : register(t0);
cbuffer cbModelConst : register(b2) { SPerModel ModelConst; }
Texture2D ModelTexture : register(t1);

