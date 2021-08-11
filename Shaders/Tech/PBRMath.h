/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\ 
*Copyright 2021 Oleh Sopilniak																																	*
*																																								*
* Permission is hereby granted, free of charge, to any person obtaining a copy of this software																	*
* and associated documentation files (the "Software"), to deal in the Software without restriction,																*
* including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,															*
* and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,															*
*  subject to the following conditions:																															*
*																																								*
* The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.								*	
*																																								*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,			*
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, *
* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.    *
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  */
#define PI 3.14159
//reflectance 
//f = DGF/(4(n*v)(n*l));
// h = normalize(v+l)
//normal distribution D
float D_GGX(float3 n, float3 h, float roughness)
{
	//Trowbridge-Reitz r*r/(PI * ((n*h)^2 * (r*r-1) + 1)^2);
	//+ Lagrange's identity |a x b|^2 = |a|^2 * |b|^2  - (a*b)^2
	float NoH = dot(n, h);
	float3 NxH = cross(n,h);
	float a = NoH*roughness;
	float k = roughness/(dot(NxH,NxH) +a*a);
	return k*k/PI;
}
// Smith-GGX geometry + Heitz microfacet height
// this func = G/(4(n*v)(n*l))
// V =  0.5( (n*l)sqrt((n*v)^2 * (1-r*r) +r*r) + (n*v)sqrt((n*l)^2 * (1-r*r) +r*r))
float V_GGX(float NoL, float NoV, float roughness)
{
	float a2 = roughness*roughness;
	float GGXV = NoL*sqrt(NoV*NoV*(1-a2)+a2);
	float GGXL = NoV*sqrt(NoL*NoL*(1-a2)+a2);
	return 0.5/(GGXV+GGXL);
}

float D_GGX_Anisotropic(const float3 n, const float3 h,
        const float3 t, const float3 b, const float roughness) {
	float anisotropy = 0.75;
	float at = max(roughness * (1.0 + anisotropy), 0.001);
	float ab = max(roughness * (1.0 - anisotropy), 0.001);
	float NoH = dot(n, h);

    float ToH = dot(t, h);
    float BoH = dot(b, h);
    float a2 = at * ab;
    min16float3 v = min16float3(ab * ToH, at * BoH, a2 * NoH);
    min16float v2 = dot(v, v);
    float w2 = a2 / v2;
    return a2 * w2 * w2 * (1.0 / PI);
}
float V_SmithGGXCorrelated_Anisotropic(float ToV, float BoV,
        float ToL, float BoL, float NoV, float NoL, float roughness) {
	float anisotropy = 0.75;
	float at = max(roughness * (1.0 + anisotropy), 0.001);
	float ab = max(roughness * (1.0 - anisotropy), 0.001);

    float lambdaV = NoL * length(float3(at * ToV, ab * BoV, NoV));
    float lambdaL = NoV * length(float3(at * ToL, ab * BoL, NoL));
    float v = 0.5 / (lambdaV + lambdaL);
    return v;
}
//FresnelSchlick
//f0 - reflectance rate at 0 angle
//F = f0 +(1-f0)(1-v*h)^5
float3 FresnelSchlick(float VoH, float3 f0)
{
	float k = pow(1 - VoH,5);
	return k + f0*(1-k);	
}
//k for diffuse to set energy preserving; more complex way than 1-fresnel
float FresnelSchlickD(float _dot, float f0, float f90)
{
	return f0+(f90-f0)*pow(1-_dot,5); 
}
float  FD_Burley(float NoL, float NoV, float LoH, float roughness)
{
	float f90 = 0.5 + 2*roughness*LoH*LoH;
	return FresnelSchlickD(NoL, 1, f90)*FresnelSchlickD(NoV, 1, f90) / PI;
}

float V_Kelemen(float LoH) {
    return 0.25 / (LoH * LoH);
}
float D_Ashikhmin(float3 n, float3 h, float roughness) {
    // Ashikhmin 2007, "Distribution-based BRDFs"
	float NoH = dot(n, h);

	float a2 = roughness * roughness;
	float cos2h = NoH * NoH;
	float sin2h = max(1.0 - cos2h, 0.0078125); // 2^(-14/2), so sin2h^2 > 0 in fp16
	float sin4h = sin2h * sin2h;
	float cot2 = -cos2h / (a2 * sin2h);
	return 1.0 / (PI * (4.0 * a2 + 1.0) * sin4h) * (4.0 * exp(cot2) + sin4h);
}
float3 BRDF(float3 baseColor, float3 v, float3 l, float3 n, float roughness, float metallness, float reflectance, bool aniso, bool clearCoat)
{
	float3 diffuseColor = baseColor*(1-metallness);
	float3 fresnelColor = 0.16*reflectance*reflectance*(1-metallness)+ baseColor*metallness;
	float3 h = normalize(v+l);
	float3 t = normalize(cross(n, float3(0,1,0))); 
	float3 b = normalize(cross(n, t)); 

	float NoL = clamp(dot(n,l), 0,1);
	float NoV = clamp(dot(n,v), 0,1);
	float LoH = clamp(dot(l,h), 0,1);
	float VoH = clamp(dot(v,h), 0,1);
	float a = roughness*roughness;
	float3 Fd = FD_Burley(NoL, NoV, LoH, a)*diffuseColor;
	float3 Fr;
	if(!aniso)
		Fr = D_GGX(n,h,a)*V_GGX(NoL,NoV,a)*FresnelSchlick(VoH, fresnelColor);//isotropic
	else
		Fr = D_GGX_Anisotropic(n, h, t, b, a)*V_SmithGGXCorrelated_Anisotropic(dot(t,v),dot(b,v),dot(t,l),dot(b,l), NoV, NoL, a)*FresnelSchlick(VoH, fresnelColor);

	float3 baseLayer =	Fd+Fr;
	if(!clearCoat)
		return baseLayer;//no clearcoat
	else
	{
		// Clear coat

		// remapping and linearization of clear coat roughness
		float clearCoatPerceptualRoughness = 0.35;
		float clearCoatRoughness = clearCoatPerceptualRoughness * clearCoatPerceptualRoughness;
		float clearCoat = 1;
		// clear coat BRDF
		float  Dc = D_GGX(n,h, clearCoatRoughness);
		float  Vc = V_Kelemen(LoH);
		float3  Fc = FresnelSchlick(LoH, 0.04) * clearCoat; // clear coat strength
		float3 Frc = (Dc * Vc) * Fc;

		// account for energy loss in the base layer
		// return ((Fd + Fr * (1.0 - Fc)) * (1.0 - Fc) + Frc);
		return baseLayer*(1-Fc) + Frc;
	}
}