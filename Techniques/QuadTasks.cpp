#include "QuadTasks.h"
#include "Shaders\Tech\Water.pipeline.h"
#include "Shaders\Tech\Waterdrop.pipeline.h"
#include "Shaders\Tech\Polygon.pipeline.h"

CParameterProviderLayout CParameterProviderBase<CWaterParameterProvider>::m_Layout = CParameterProviderLayout(&CWaterParameterProvider::CreateParameterMap);

void CTexturedQuadGeometry::DefineVertexFormat(vector<AttribDesc>& format)
{
	format.push_back({ 0, VF_FLOAT3, "Position" }); 
	format.push_back({ 0, VF_FLOAT2, "TexCoord" });
};

void CTexturedQuadGeometry::Draw(Context context, CShaderCache& cache, int resources_slot)
{
	if (m_Texture)
	{
		cache.GatherParameters({ m_Texture }, resources_slot, m_TexDescIndex);
	}
	ResourceTable rt = CreateResourceTable(GetDevice(context), cache.GetRootSignature(), resources_slot, nullptr, 
		cache.GetDescriptorsCount(resources_slot), context);

	cache.UpdateResourceTable(GetDevice(context), resources_slot, rt);

	SetGraphicsResourceTable(context, resources_slot, rt);

	DestroyResourceTable(GetDevice(context), rt);

	SetVertexSetup(context, m_VertexSetup); 
	//PRIM_TRIANGLE_FAN
	::Draw(context, 0, 4);
}
void CTexturedQuadGeometry::Create(Device device)
{
	CTexturedQuadGeometry::Create(device, { { {-1,  1, 0},  {0, 0} },
											{ { 1,  1, 0},  {1, 0} },
											{ { 1, -1, 0},  {1, 1} },
											{ {-1, -1, 0},  {0, 1} }
										  });
}
void CTexturedQuadGeometry::Create(Device device,const QuadVertex (&vtx)[4])
{
	m_Device = device;
	SBufferParams params = { 4 * sizeof(QuadVertex), HEAP_DEFAULT, Usage::VERTEX_BUFFER, "" };
	m_VB = CreateBuffer(m_Device, params);
	m_VertexSetup = CreateVertexSetup(device, m_VB, sizeof(QuadVertex));
	SMapBufferParams mapParam = { GetMainContext(device), m_VB, 0, 4 * sizeof(QuadVertex) };
	QuadVertex* vertices = (QuadVertex*)MapBuffer(mapParam);
	//PRIM_TRIANGLE_FAN
	memcpy(vertices, &vtx[0], 4 * sizeof(QuadVertex));
	UnmapBuffer(mapParam);
	m_Texture = nullptr;
}

CTexturedQuadGeometry::~CTexturedQuadGeometry()
{
	DestroyBuffer(m_Device, m_VB);
	DestroyVertexSetup(m_Device, m_VertexSetup);
}

void CPolygonGeometry::Draw(Context context, CShaderCache& cache, int resources_slot)
{
	SetVertexSetup(context, m_VertexSetup);
	::Draw(context, 0, m_sideCount);
}

void CPolygonGeometry::DefineVertexFormat(vector<AttribDesc>& format)
{
	format.push_back({ 0, VF_FLOAT2, "Position" });
}

void CPolygonGeometry::Create(Device device)
{
	m_Device = device;
	SBufferParams params = { m_sideCount * sizeof(float2), HEAP_DEFAULT, Usage::VERTEX_BUFFER, "" };
	m_VB = CreateBuffer(m_Device, params);
	m_VertexSetup = CreateVertexSetup(device, m_VB, sizeof(float2));
}

void CPolygonGeometry::UpdatePos(float radius, float2 pos)
{
	SMapBufferParams mapParam = { GetMainContext(m_Device), m_VB, 0, m_sideCount * sizeof(float2) };
	float2* vertices = (float2*)MapBuffer(mapParam);
	float step = 2 * PI / m_sideCount, curAngle = 0;
	for (int i = 0; i < m_sideCount; i++)
	{
		vertices[i] = { pos.x + radius*cos(curAngle), pos.y - radius * sin(curAngle) };
		curAngle += step;
	}
	UnmapBuffer(mapParam);
}

bool CPolygonTask::CreateResources(Device device)
{
    m_Device = device;
    return  m_Cache.CreateRootSignature(m_Device, NPolygon::RootSig, nullptr);
}

void CPolygonTask::Draw(Context context)
{
	InitPipeline(context);
	SetRootSignature(context, m_Cache.GetRootSignature());
	SetPipeline(context, m_Pipeline);
	m_Collection->Draw(context, m_Cache, 0);
}
void CPolygonTask::InitPipeline(Context context)
{
	if (!m_Pipeline)
	{
		vector<AttribDesc> format;
		m_Collection->DefineVertexFormat(format);

		SPipelineParams p_params;
		p_params.m_Name = "Polygon";
		p_params.m_RootSignature = m_Cache.GetRootSignature();
		p_params.m_RenderPass = GetCurrentRenderPass(context);
	
		p_params.m_VS = NPolygon::VS;
		p_params.m_PS = NPolygon::PS;

		p_params.m_Attribs = format.data();
		p_params.m_AttribCount = format.size();

		p_params.m_PrimitiveType = m_Collection->GetPrimType();

		p_params.m_BlendState = GetDefaultBlendState(m_Device);
		p_params.m_DepthTest = false;
		p_params.m_DepthWrite = false;
		m_Pipeline = CreatePipeline(m_Device, p_params);
	}
}


bool CWaterDropTask::CreateResources(Device device)
{
    m_Device = device;

	if (m_Cache.CreateRootSignature(m_Device, NWaterdrop::RootSig, nullptr))
	{
		const SSamplerDesc samplers[] = { { FILTER_TRILINEAR, 1, AM_WRAP, AM_WRAP, AM_WRAP, ALWAYS } };
		if ((m_SamplerTable = CreateSamplerTable(m_Device, m_Cache.GetRootSignature(), NWaterdrop::Samplers, samplers)) == nullptr)
			return false;

		return true;
	}
	return false;
}

void CWaterDropTask::Draw(Context context)
{
	InitPipeline(context);
	SetRootSignature(context, m_Cache.GetRootSignature());
	SetPipeline(context, m_Pipeline);
	SetGraphicsSamplerTable(context, NWaterdrop::Samplers, m_SamplerTable);
	m_Collection->Draw(context, m_Cache, NWaterdrop::Resources);
}
void CWaterDropTask::InitPipeline(Context context)
{
	if (!m_Pipeline)
	{
		vector<AttribDesc> format;
		m_Collection->DefineVertexFormat(format);

		SPipelineParams p_params;
		p_params.m_Name = "WaterDrop";
		p_params.m_RootSignature = m_Cache.GetRootSignature();
		p_params.m_RenderPass = GetCurrentRenderPass(context);

		p_params.m_VS = NWaterdrop::VS;
		p_params.m_PS = NWaterdrop::PS;

		p_params.m_Attribs = format.data();
		p_params.m_AttribCount = format.size();

		p_params.m_PrimitiveType = m_Collection->GetPrimType();

		p_params.m_BlendState = GetDefaultBlendState(m_Device);
		p_params.m_DepthTest = false;
		p_params.m_DepthWrite = false;
		m_Pipeline = CreatePipeline(m_Device, p_params);
	}
}
SRootResourceAttrs GetWaterResourceAttrs(uint slot, uint binding)
{
	SRootResourceAttrs attrs;
	attrs.name = NWater::RootItemNames[slot][binding];
	attrs.type = NWater::RootItemTypes[slot][binding];
	return attrs;
}

bool CWaterTask::CreateResources(Device device)
{
	m_Device = device;

	if (m_Cache.CreateRootSignature(m_Device, NWater::RootSig, GetWaterResourceAttrs))
	{
		const SSamplerDesc samplers[] = { { FILTER_TRILINEAR, 1, AM_WRAP, AM_WRAP, AM_WRAP, ALWAYS } };
		if ((m_SamplerTable = CreateSamplerTable(m_Device, m_Cache.GetRootSignature(), NWaterdrop::Samplers, samplers)) == nullptr)
			return false;
		m_WaterProvider.GetPBR().baseColor = float3(0.3, 0.4, 1.0);
		m_WaterProvider.GetPBR().material = float4(0.35*0.35, 0, 0.35, 0);
		m_Camera.ProjectPerspective(PI / 4, 720.0f / 1280.0f, 0.5, 20);
		return true;
	}


	return false;
}

void CWaterTask::SetCameraLookAt(vec3 eye, vec3 target, vec3 up)
{ 
	CRenderTask::SetCameraLookAt(eye, target, up);
	m_WaterProvider.Get().camPos = eye;
	m_WaterProvider.Get().mvp = mul(m_Camera.GetProjection(), m_Camera.GetViewTransform());
}
void CWaterTask::SetBox(vec3 center, vec3 min, vec3 max, vec3 minFar, vec3 maxFar)
{
	m_WaterProvider.Get().cubeCenter = center ;
	m_WaterProvider.Get().boxMax = max;
	m_WaterProvider.Get().boxMin = min;
	m_WaterProvider.Get().boxMaxFar = maxFar;
	m_WaterProvider.Get().boxMinFar = minFar;
}
void CWaterTask::SetWaveOffset(float val) 
{
	m_WaterProvider.Get().waveOffsetScale = val;
};

void CWaterTask::Update(float delta)
{
	m_WaterProvider.Get().streamOffset = m_StreamOffset;
	m_StreamOffset += delta * m_Speed;
}

void CWaterTask::SetTextures(Texture env, Texture wave, Texture planar, Texture normalTile)
{
	m_WaterProvider.m_EnvTexture = env;
	m_WaterProvider.m_WaveTexture = wave;
	m_WaterProvider.m_PlanarReflection = planar;
	m_WaterProvider.m_NormalTile = normalTile;
}

void CWaterTask::Draw(Context context)
{
	InitPipeline(context);
	SetRootSignature(context, m_Cache.GetRootSignature());
	SetPipeline(context, m_Pipeline);
	SetGraphicsSamplerTable(context, NWaterdrop::Samplers, m_SamplerTable);
	vector<IParameterProvider*> providers = { &m_WaterProvider };
	m_Cache.GatherParameters(context, providers.data(), 1);
	m_Collection->Draw(context, m_Cache, NWaterdrop::Resources);
}

void CWaterTask::InitPipeline(Context context)
{
	if (!m_Pipeline)
	{
		vector<AttribDesc> format;
		m_Collection->DefineVertexFormat(format);

		SPipelineParams p_params;
		p_params.m_Name = "Water";
		p_params.m_RootSignature = m_Cache.GetRootSignature();
		p_params.m_RenderPass = GetCurrentRenderPass(context);

		p_params.m_VS = NWater::VS;
		p_params.m_PS = NWater::PS;

		p_params.m_Attribs = format.data();
		p_params.m_AttribCount = format.size();

		p_params.m_PrimitiveType = m_Collection->GetPrimType();
		m_BlendState = CreateBlendState(m_Device, BF_SRC_ALPHA, BF_INV_SRC_ALPHA, BM_ADD, 0xF, false);

		p_params.m_BlendState = m_BlendState;
		p_params.m_DepthTest = true;
		p_params.m_DepthWrite = true;
		p_params.m_DepthFunc = EComparisonFunc::LESS;

		m_Pipeline = CreatePipeline(m_Device, p_params);
	}
}
