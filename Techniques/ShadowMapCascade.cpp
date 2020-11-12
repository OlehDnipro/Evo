

#include "ShadowMapCascade.h"
#include "Shaders\Tech\ShadowMapCascade.pipeline.h"

CParameterProviderLayout CParameterProviderBase<CViewportParameterProvider>::m_Layout = CParameterProviderLayout(&CViewportParameterProvider::CreateParameterMap);
CParameterProviderLayout CParameterProviderBase<CShadowParameterProvider>::m_Layout = CParameterProviderLayout(&CShadowParameterProvider::CreateParameterMap);

//#define PROV


void ShadowMapCascade::SetCameraLookAt(vec3 eye, vec3 target, vec3 up)
{
	CRenderTask::SetCameraLookAt(eye, target, up);
	m_ViewportProvider.Get().view = m_Camera.GetViewTransform();
	m_perFrame = m_ViewportProvider.Get();

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

void ShadowMapCascade::CreateObjects(Device device)
{
	float4x4 mtx; mtx.identity();
	m_Objects.resize(3);
	m_Objects[0].Init(device, vertexLayout, "../../models/terrain_simple.dae", "../../Textures/gridlines.dds", 1.0f);
	m_Objects[1].Init(device, vertexLayout, "../../models/oak_trunk.dae", "../../Textures/oak_bark.dds", 2.0f);
	m_Objects[2].Init(device, vertexLayout, "../../models/oak_leafs.dae", "../../Textures/oak_leafs.dds", 2.0f);
	m_ObjectInstances.resize(11);
	m_ObjectInstances[0] = new SimpleObjectInstance(m_Objects[0], device, m_Cache.m_RootSig, mtx);

	const std::vector<vec3> positions = {
	vec3(0.0f, 0.0f, 0.0f),
	vec3(1.25f, -0.25f, 1.25f),
	vec3(-1.25f, 0.2f, 1.25f),
	vec3(1.25f, -0.1f, -1.25f),
	vec3(-1.25f, 0.25f, -1.25f),
	};
	for (int i = 1; i < 11; i++)
	{
		mtx = translate(positions[(i - 1) / 2]);
		if (i % 2 == 0)
		{
			m_ObjectInstances[i] = new SimpleObjectInstance(m_Objects[1], device, m_Cache.m_RootSig, mtx);
		}
		else
		{
			m_ObjectInstances[i] = new SimpleObjectInstance(m_Objects[2], device, m_Cache.m_RootSig, mtx);
		}
	}

}
const char* GetResourceName(uint slot, uint binding)
{
	return NShadowMapCascade::RootItemNames[slot][binding];
}

bool ShadowMapCascade::CreateResources(Device device)
{
	if ((m_Cache.m_RootSig = CreateRootSignature(device, NShadowMapCascade::RootSig)) == nullptr) 
		return false;
	m_Cache.m_getResourceName = &GetResourceName;
	IterateRootSignature(m_Cache.m_RootSig, &CShaderCache::FindRootResource, &m_Cache);

	CreateObjects(device);
	
	const SSamplerDesc samplers[] = { { FILTER_TRILINEAR, 1, AM_WRAP, AM_WRAP, AM_WRAP, ALWAYS }, { FILTER_LINEAR, 1, AM_WRAP, AM_WRAP, AM_WRAP, LESS } };
	if ((m_SamplerTable = CreateSamplerTable(device, m_Cache.m_RootSig, NShadowMapCascade::Samplers, samplers)) == nullptr) return false;

	vec3 lightPos = { 6.18, 20, -19 };
	m_ViewportProvider.Get().projection = m_Camera.ProjectPerspective(PI / 4, 720.0f / 1280.0f, 0.5, 20);
	m_ViewportProvider.Get().lightDir = normalize(-lightPos);
	
	return true;
}

void ShadowMapCascade::DestroyResources(Device device)
{
	DestroyRootSignature(device, m_Cache.m_RootSig);
}

void ShadowMapCascade::SetPassParameters(Device device, RenderPass pass, PassEnum passId, int cascade)
{
	m_currentPass = passId;
	m_curCascade = cascade;
	if (m_Pipeline[m_currentPass] == VK_NULL_HANDLE)
	{
		const AttribDesc format[] =
		{
			{ 0, VF_FLOAT3, "Position"   },
			{ 0, VF_FLOAT2, "TexCoord" },
			{ 0, VF_FLOAT3, "Color" },
			{ 0, VF_FLOAT3, "Normal" },
		};

		SPipelineParams p_params;
		p_params.m_Name = "ShadowMapCascade";
		p_params.m_RootSignature = m_Cache.m_RootSig;
		p_params.m_RenderPass = pass;
		if (passId == PassEnum::MainPass)
		{
			p_params.m_VS =  NShadowMapCascade::VSMainPass ;
			p_params.m_PS =  NShadowMapCascade::PSMainPass;
		}
		else
		{
			p_params.m_VS =  NShadowMapCascade::VSShadowPass;
			p_params.m_PS =  NShadowMapCascade::PSShadowPass;
		}
		p_params.m_Attribs = format;
		p_params.m_AttribCount = 4;

		p_params.m_PrimitiveType = PRIM_TRIANGLES;

		p_params.m_BlendState = GetDefaultBlendState(device);
		p_params.m_DepthTest = true;
		p_params.m_DepthWrite = true;
		p_params.m_DepthFunc = EComparisonFunc::LESS;
		m_Pipeline[m_currentPass] = CreatePipeline(device, p_params);
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
	SetRootSignature(context, m_Cache.m_RootSig);
	vector<IParameterProvider*> providers = { m_curCascade < 0 ? (IParameterProvider*)&m_ViewportProvider :
															(IParameterProvider*)&m_ShadowViewportProvider[m_curCascade],
															(IParameterProvider*)&m_ShadowProvider,
															nullptr
											};
	SetGraphicsSamplerTable(context, NShadowMapCascade::Samplers, m_SamplerTable);
	m_Cache.GatherParameters(providers.data(), context, 2);

	SetPipeline(context, m_Pipeline[m_currentPass]);
	for (int i = 0; i < 11; i++)
	{
		providers[2] = m_ObjectInstances[i]->GetModelProvider();
		m_Cache.GatherParameters(providers.data() +2 , context, 1);
		
		ResourceTable rt = CreateResourceTable(GetDevice(context), m_Cache.m_RootSig, NShadowMapCascade::Resources, nullptr, 0, context);

		UpdateResourceTable(GetDevice(context), m_Cache.m_RootSig, NShadowMapCascade::Resources, rt,
			m_Cache.m_TableUpdates[NShadowMapCascade::Resources].m_Descriptors.data(), 0, 5);

		SetGraphicsResourceTable(context, NShadowMapCascade::Resources, rt);

		DestroyResourceTable(GetDevice(context), rt);
		m_ObjectInstances[i]->Draw(context);
	}
}

