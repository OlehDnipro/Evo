
SamplerState Filter : register(s0);
cbuffer cbperFrame : register(b0) { SPerFrame perFrame; }
cbuffer cbperModel : register(b1) { SPerModel perModel; }
Texture2D Texture : register(t0);

