

#include "ShadowMapCascade.h"
#include "Shaders\Tech\ShadowMapCascade.pipeline.h"

CParameterProviderLayout CParameterProviderBase<CViewportParameterProvider>::m_Layout = CParameterProviderLayout(&CViewportParameterProvider::CreateParameterMap);
CParameterProviderLayout CParameterProviderBase<CShadowParameterProvider>::m_Layout = CParameterProviderLayout(&CShadowParameterProvider::CreateParameterMap);


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


const char* GetResourceName(uint slot, uint binding)
{
	return NShadowMapCascade::RootItemNames[slot][binding];
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

bool ShadowMapCascade::CreateResources(Device device)
{
	m_Device = device;
	m_Cache.CreateRootSignature(m_Device, NShadowMapCascade::RootSig, &GetResourceName);
	
	const SSamplerDesc samplers[] = { { FILTER_TRILINEAR, 1, AM_WRAP, AM_WRAP, AM_WRAP, ALWAYS }, { FILTER_LINEAR, 1, AM_WRAP, AM_WRAP, AM_WRAP, LESS } };
	if ((m_SamplerTable = CreateSamplerTable(m_Device, m_Cache.GetRootSignature(), NShadowMapCascade::Samplers, samplers)) == nullptr) return false;

	vec3 lightPos = { 6.18, 20, -19 };
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
				p_params.m_VS = NShadowMapCascade::VSShadowPass;
				p_params.m_PS = NShadowMapCascade::PSShadowPass;
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
															nullptr
											};
	SetGraphicsSamplerTable(context, NShadowMapCascade::Samplers, m_SamplerTable);
	m_Cache.GatherParameters(context, providers.data(), 2);
	SetPipeline(context, m_Pipeline[m_currentPass]);
	m_Collection->Draw(context, m_Cache, NShadowMapCascade::Resources);
}

