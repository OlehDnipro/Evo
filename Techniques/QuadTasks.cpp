#include "QuadTasks.h"
#include "Shaders\Tech\Waterdrop.pipeline.h"
#include "Shaders\Tech\Polygon.pipeline.h"

struct QuadVertex
{
	float2 pos, uv;
};
void CTexturedQuadGeometry::DefineVertexFormat(vector<AttribDesc>& format)
{
	format.push_back({ 0, VF_FLOAT2, "Position" }); 
	format.push_back({ 0, VF_FLOAT2, "TexCoord" });
};

void CTexturedQuadGeometry::Draw(Context context, CShaderCache& cache, int resources_slot)
{
	cache.GatherParameters({ m_Texture }, resources_slot);
	ResourceTable rt = CreateResourceTable(GetDevice(context), cache.GetRootSignature(), resources_slot, nullptr, 0, context);

	cache.UpdateResourceTable(GetDevice(context), resources_slot, rt);

	SetGraphicsResourceTable(context, resources_slot, rt);

	DestroyResourceTable(GetDevice(context), rt);

	SetVertexSetup(context, m_VertexSetup); 
	//PRIM_TRIANGLE_FAN
	::Draw(context, 0, 4);
}
void CTexturedQuadGeometry::Create(Device device)
{
	m_Device = device;
	SBufferParams params = { 4 * sizeof(QuadVertex), HEAP_DEFAULT, Usage::VERTEX_BUFFER, "" };
	m_VB = CreateBuffer(m_Device, params);
	m_VertexSetup = CreateVertexSetup(device, m_VB, sizeof(QuadVertex));
	SMapBufferParams mapParam = { GetMainContext(device), m_VB, 0, 4 * sizeof(QuadVertex) };
	QuadVertex* vertices = (QuadVertex*)MapBuffer(mapParam);
	//PRIM_TRIANGLE_FAN
	vertices[0] = { {-1,  1},  {0, 0} };
	vertices[1] = { { 1,  1},  {1, 0} };
	vertices[2] = { { 1, -1},  {1, 1} };
	vertices[3] = { {-1, -1},  {0, 1} };
	UnmapBuffer(mapParam);
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