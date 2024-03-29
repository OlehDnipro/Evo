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

#include "ShadowMapCascade.h"
#if GRAPHICS_API_VULKAN
#include "Shaders\Tech\vulkan\ShadowMapCascade.pipeline.h"
#else
#include "Shaders\Tech\d3d12\ShadowMapCascade.pipeline.h"
#endif
CParameterProviderLayout CParameterProviderBase<CShadowParameterProvider>::m_Layout = CParameterProviderLayout(&CShadowParameterProvider::CreateParameterMap);
CParameterProviderLayout CParameterProviderBase<CReflectProvider>::m_Layout = CParameterProviderLayout(&CReflectProvider::CreateParameterMap);


void ShadowMapCascade::SetCameraLookAt(vec3 eye, vec3 target, vec3 up)
{
	CRenderTask::SetCameraLookAt(eye, target, up);
	m_ViewportProvider.Get().view = m_Camera.GetViewTransform();
	m_perFrame = m_ViewportProvider.Get();
	if (m_currentPass == NoShadow)
		return;
	vec4 frustumView[] = {
		{ -1,  1, 0, 1},
		{  1,  1, 0, 1},
		{  1, -1, 0, 1},
		{ -1, -1, 0, 1},
		{ -1,  1, 1, 1},
		{  1,  1, 1, 1},
		{  1, -1, 1, 1},
		{ -1, -1, 1, 1}
	};
	const int points = sizeof(frustumView) / sizeof(vec4);
	float4x4 toWorld = inverse(mul(m_ViewportProvider.Get().projection, m_ViewportProvider.Get().view));


	for (int i = 0; i < points; i++)
	{
		frustumView[i] = mul(toWorld, frustumView[i]);
		frustumView[i] /= frustumView[i].w;
	}
	float farPlane = m_Camera.GetFarPlane();
	float nearPlane = m_Camera.GetNearPlane();
	for (int i = 0; i < SHADOW_MAP_CASCADE_COUNT; i++)
	{
		float uni = nearPlane + (farPlane - nearPlane) * ( i + 1.0f ) / SHADOW_MAP_CASCADE_COUNT;
		float log = nearPlane * pow(farPlane / nearPlane, ( i + 1.0f) / SHADOW_MAP_CASCADE_COUNT);
		const float lambda = 0.9;
		float lerped = lambda * log + (1 - lambda) * uni;
		m_ShadowProvider.Get().cascadePlanes[i] = (lerped - nearPlane) / (farPlane - nearPlane);
	}
	for (int casc = 0; casc < SHADOW_MAP_CASCADE_COUNT; casc++)
	{
		float4 frustumCascade[8];
		float4 diff[4];
		float lastSplit = 0;
		vec4 center(0, 0, 0, 0);
		float radius = 0;
		for (int i = 0; i < 4; i++)
		{
			diff[i] = frustumView[i + 4] - frustumView[i];
			frustumCascade[i] = frustumView[i] + diff[i] * lastSplit;
			frustumCascade[i + 4] = frustumView[i] + diff[i] * m_ShadowProvider.Get().cascadePlanes[casc];
			center += frustumCascade[i];
			center += frustumCascade[i + 4];
		}
		lastSplit = m_ShadowProvider.Get().cascadePlanes[casc];
		m_ShadowProvider.Get().cascadePlanes[casc] = nearPlane + (farPlane - nearPlane) * m_ShadowProvider.Get().cascadePlanes[casc];
		center /= 8;
		for (int i = 0; i < points; i++)
		{
			float dist = length(center - frustumCascade[i]);
			radius = radius > dist ? radius:dist;
		}
		vec3 _center(center.x, center.y, center.z);
		vec3 light_eye = _center - m_ViewportProvider.Get().lightDir * radius;
		Camera cam;
		cam.lookat(light_eye, _center, vec3(0, 1, 0));
		m_ShadowProvider.Get().lightViewProjection[casc] = mul(ProjectiveMatrix(2 * radius, 2 * radius, 0, 2 * radius), cam.GetViewTransform());
	}

	float4 vec(0, 0, m_ShadowProvider.Get().cascadePlanes[1], 1);
	vec = mul(m_Camera.GetProjection(), vec);
	vec /= vec.w;
}


SRootResourceAttrs GetResourceAttrs(uint slot, uint binding)
{
	SRootResourceAttrs attrs;
	attrs.name = NShadowMapCascade::RootItemNames[slot][binding];
	attrs.type = NShadowMapCascade::RootItemTypes[slot][binding];
	return attrs;
}

void ShadowMapCascade::SetCubeProjection(bool cube)
{
	if (!cube)
	{
		m_ViewportProvider.Get().projection = m_Camera.ProjectPerspective(PI / 4, 720.0f / 1280.0f, 0.5, 20);
		m_currentPass = MainPass;

	}
	else
	{
		m_ViewportProvider.Get().projection = m_Camera.ProjectPerspective(PI / 2, 1, 0.25, 20);
		m_currentPass = NoShadow;
	}
}

void ShadowMapCascade::SetPlanarReflectionParam(float4x4 mtx, float4 plane)
{
	m_ReflectProvider.Get().reflectMatrix = mtx;
	m_ReflectProvider.Get().reflectMatrixTInv = transpose(inverse(mtx));
	m_ReflectProvider.Get().plane = plane;
}

bool ShadowMapCascade::CreateResources(Device device)
{
	m_Device = device;
	m_Cache.CreateRootSignature(m_Device, NShadowMapCascade::RootSig, &GetResourceAttrs);
	
	const SSamplerDesc samplers[] = { { FILTER_TRILINEAR, 1, AM_WRAP, AM_WRAP, AM_WRAP, ALWAYS }, { FILTER_LINEAR, 1, AM_WRAP, AM_WRAP, AM_WRAP, LESS } };
	if ((m_SamplerTable = CreateSamplerTable(m_Device, m_Cache.GetRootSignature(), NShadowMapCascade::Samplers, samplers)) == nullptr) return false;

	vec3 lightPos = { 6.18, 15, -19 };
	SetCubeProjection(false);
	m_ViewportProvider.Get().lightDir = normalize(-lightPos);

	return true;
}

ShadowMapCascade::~ShadowMapCascade()
{
	m_Cache.DestroyRootSignature(m_Device);
}

void ShadowMapCascade::SetPassParameters(Context context, PassEnum passId, int cascade)
{
	m_currentPass = passId;
	m_curCascade = cascade;
	if (m_Pipeline[m_currentPass] == VK_NULL_HANDLE)
	{

		vector<AttribDesc> format;
		m_Collection->DefineVertexFormat(format);

		SPipelineParams p_params;
		p_params.m_Name = "ShadowMapCascade";
		p_params.m_RootSignature = m_Cache.GetRootSignature();
		p_params.m_RenderPass = GetCurrentRenderPass(context);
		if (passId == PassEnum::MainPass)
		{
			p_params.m_VS =  NShadowMapCascade::VSMainPass ;
			p_params.m_PS =  NShadowMapCascade::PSMainPass;
		}
		else
		{
			if (passId == NoShadow)
			{
				p_params.m_VS = NShadowMapCascade::VSNoShadowPass;
				p_params.m_PS = NShadowMapCascade::PSNoShadowPass;
			}
			else
			{
				if (passId == Reflection)
				{
					p_params.m_VS = NShadowMapCascade::VSReflection;
					p_params.m_PS = NShadowMapCascade::PSReflection;
				}
				else
				{
					p_params.m_VS = NShadowMapCascade::VSShadowPass;
					p_params.m_PS = NShadowMapCascade::PSShadowPass;
				}
			}
		}
		p_params.m_Attribs = format.data();
		p_params.m_AttribCount = format.size();

		p_params.m_PrimitiveType = m_Collection->GetPrimType();

		p_params.m_BlendState = GetDefaultBlendState(m_Device);
		p_params.m_DepthTest = true;
		p_params.m_DepthWrite = true;
		p_params.m_DepthFunc = EComparisonFunc::LESS;
		m_Pipeline[m_currentPass] = CreatePipeline(m_Device, p_params);
	}
	
	if (cascade >= 0)
	{
		SPerFrame& localperFrame = m_ShadowViewportProvider[cascade].Get();
		float4x4 mat; mat.identity();
		localperFrame.projection = mat;
		localperFrame.view = m_ShadowProvider.Get().lightViewProjection[cascade];
	}
}


void ShadowMapCascade::Draw(Context context)
{
	SetRootSignature(context, m_Cache.GetRootSignature());
	vector<IParameterProvider*> providers = { m_curCascade < 0 ? (IParameterProvider*)&m_ViewportProvider :
															(IParameterProvider*)&m_ShadowViewportProvider[m_curCascade],
															(IParameterProvider*)&m_ShadowProvider,
															(IParameterProvider*)&m_ReflectProvider,
															nullptr
											};
	SetGraphicsSamplerTable(context, NShadowMapCascade::Samplers, m_SamplerTable);
	m_Cache.GatherParameters(context, providers.data(), 3);
	SetPipeline(context, m_Pipeline[m_currentPass]);
	m_Collection->Draw(context, m_Cache, NShadowMapCascade::Resources);
}

