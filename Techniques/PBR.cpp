#include "PBR.h"
#if GRAPHICS_API_VULKAN
#include "Shaders\Tech\vulkan\PBR.pipeline.h"
#else
#include "Shaders\Tech\d3d12\PBR.pipeline.h"
#endif
CParameterProviderLayout CParameterProviderBase<CPBRModelParameterProvider>::m_Layout = CParameterProviderLayout(&CPBRModelParameterProvider::CreateParameterMap);

void GenerateSphereMesh(SphereVertex*& vertices, uint16*& indices, uint& out_vertCount, uint& out_indCount, uint in_divisor)
{
	vector<vector<SphereVertex>> m_Storage;
	vector<uint16> m_Indices;
	m_Storage.resize(in_divisor + 1);
	float cur_co = 0;
	for (int i = 0; i < in_divisor + 1; i++)
	{
		if (i == 0)
		{
			m_Storage[i].push_back({ {0, 1 ,0} });
		}
		else
		{
			if (i == in_divisor)
			{
				m_Storage[i].push_back({ {0, -1, 0} });
			}
			else
			{
				float rad = cos(cur_co);
				float angle = 0;
				float dangle = 2*3.14159 / in_divisor;
				for (int j = 0; j < in_divisor; j++)
				{
					m_Storage[i].push_back({ {sinf(cur_co)*cosf(angle), cos(cur_co), sinf(cur_co)*sinf(angle)} });
					angle += dangle;
				}
			}
		}
		cur_co += 3.14159 / in_divisor;
	}
	uint16 offset_cur_line = 0;
	uint16 offset_next_line = 1;

	for (int i = 0; i < in_divisor; i++)
	{
		if (i == 0)
		{
			for (int j = 0; j < in_divisor; j++)
			{
				uint16 cur = j;
				uint16 next = j + 1; if (next == in_divisor) next = 0;
				m_Indices.push_back(offset_cur_line);
				m_Indices.push_back(offset_next_line + next);
				m_Indices.push_back(offset_next_line + cur);
			}
		}
		else
		{
			if (i == in_divisor - 1)
			{
				for (int j = 0; j < in_divisor; j++)
				{
					uint16 cur = j;
					uint16 next = j + 1; if (next == in_divisor) next = 0;
					m_Indices.push_back(offset_cur_line + cur);
					m_Indices.push_back(offset_cur_line + next);
					m_Indices.push_back(offset_next_line);
				}
				break;
			}
			else
			{
				for (int j = 0; j < in_divisor; j++)
				{
					uint16 cur = j;
					uint16 next = j + 1; if (next == in_divisor) next = 0;
					m_Indices.push_back(offset_cur_line + cur);
					m_Indices.push_back(offset_next_line + next);
					m_Indices.push_back(offset_next_line + cur);

					m_Indices.push_back(offset_cur_line + cur);
					m_Indices.push_back(offset_cur_line + next);
					m_Indices.push_back(offset_next_line + next);

				}

			}
		}
		offset_cur_line += (i==0?1:in_divisor);
		offset_next_line += in_divisor;
	}
	offset_next_line ++;
	vertices = new SphereVertex[offset_next_line];
	out_vertCount = offset_next_line;
	uint cur_vert = 0;
	for (auto& line : m_Storage)
	{
		for (auto& vert : line)
		{
			vertices[cur_vert++] = vert;
		}
	}
	out_indCount = m_Indices.size();
	indices = new uint16[m_Indices.size()];
	memcpy(indices, m_Indices.data(), m_Indices.size() * sizeof(uint16));
}
SRootResourceAttrs GetResourceAttrsPBR(uint slot, uint binding)
{
	SRootResourceAttrs attrs;
	attrs.name = NPBR::RootItemNames[slot][binding];
	attrs.type = NPBR::RootItemTypes[slot][binding];
	return attrs;
}

void CSphereGeometry::DefineVertexFormat(vector<AttribDesc>& format)
{
	format.push_back({ 0, VF_FLOAT3, "Position" }); 
};

void CSphereGeometry::Draw(Context context, CShaderCache& cache, int resources_slot)
{
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			float4x4 mtx;
			mtx.identity();
			mtx.rows[0].x = 0.1;
			mtx.rows[1].y = 0.1;
			mtx.rows[2].z = 0.1;
			mtx = mul(translate(float3(3 + 0.25 * i, 0, 2.5 - 0.25 * j)), mtx);
			m_Provider.Get().model = mtx;
			m_Provider.Get().material = float4(0.1 + 0.2 * i, 0.1 + 0.2 * j, 0.75, 0);
			m_Provider.Get().baseColor = float3(0.2, 0, 1);
			{
				IParameterProvider* prov[] = { &m_Provider };

				cache.GatherParameters(context, &prov[0], 1);

				ResourceTable rt = CreateResourceTable(GetDevice(context), cache.GetRootSignature(), resources_slot, nullptr,
					cache.GetDescriptorsCount(resources_slot), context);

				cache.UpdateResourceTable(GetDevice(context), resources_slot, rt);

				SetGraphicsResourceTable(context, resources_slot, rt);

				DestroyResourceTable(GetDevice(context), rt);

				SetVertexSetup(context, m_VertexSetup);

				::DrawIndexed(context, 0, m_meshIndicesCount);
			}
		}
	}
	
}


void CSphereGeometry::Create(Device device)
{
	uint16* sphereIndices;
	SphereVertex* sphereVertices;
	uint vertCount;
	GenerateSphereMesh(sphereVertices, sphereIndices, vertCount, m_meshIndicesCount, 15);
	SBufferParams params = { sizeof(SphereVertex) * vertCount, HEAP_DEFAULT, Usage::VERTEX_BUFFER, "" };
	m_VB = CreateBuffer(device, params);
	params = { sizeof(uint16) * m_meshIndicesCount, HEAP_DEFAULT, Usage::INDEX_BUFFER, "" };
	m_IB = CreateBuffer(device, params);
	m_VertexSetup = CreateVertexSetup(device, m_VB, sizeof(SphereVertex), m_IB, sizeof(uint16));
	SMapBufferParams mapParam = { GetMainContext(device), m_VB, 0, sizeof(SphereVertex) * vertCount };
	SphereVertex* vertices = (SphereVertex*)MapBuffer(mapParam);

	memcpy(vertices, &sphereVertices[0], sizeof(SphereVertex) * vertCount);
	UnmapBuffer(mapParam);

	SMapBufferParams mapParam1 = { GetMainContext(device), m_IB, 0, sizeof(uint16) * m_meshIndicesCount };
	uint8* indices = MapBuffer(mapParam1);
	memcpy(indices, &sphereIndices[0], sizeof(uint16) * m_meshIndicesCount);
	UnmapBuffer(mapParam1);

	float4x4 mtx;
	mtx.identity();
	mtx.rows[0].x = 0.1;
	mtx.rows[1].y = 0.1;
	mtx.rows[2].z = 0.1;
	mtx = mul(translate(float3(0, 1, 0)), mtx);
	m_Provider.Get().model = mtx;
	m_Provider.Get().material = float4(0.1, 0.25, 0.75, 0);
	m_Provider.Get().baseColor = float3(1, 0, 0);
}

CSphereGeometry::~CSphereGeometry()
{
//	DestroyBuffer(m_Device, m_VB);
	DestroyVertexSetup(m_Device, m_VertexSetup);
}

bool CPBRTask::CreateResources(Device device)
{
    m_Device = device;
    return  m_Cache.CreateRootSignature(m_Device, NPBR::RootSig, &GetResourceAttrsPBR);
}

void CPBRTask::Draw(Context context)
{
	InitPipeline(context);
	SetRootSignature(context, m_Cache.GetRootSignature());
	SetPipeline(context, m_Pipeline);
	IParameterProvider* prov[] = { &m_ViewportProvider };
	m_Cache.GatherParameters(context, &prov[0], 1);
	
	m_Collection->Draw(context, m_Cache, 0);
}

void CPBRTask::InitPipeline(Context context)
{
	if (!m_Pipeline)
	{
		vector<AttribDesc> format;
		m_Collection->DefineVertexFormat(format);

		SPipelineParams p_params;
		p_params.m_Name = "Polygon";
		p_params.m_RootSignature = m_Cache.GetRootSignature();
		p_params.m_RenderPass = GetCurrentRenderPass(context);
	
		p_params.m_VS = NPBR::VS;
		p_params.m_PS = NPBR::PS;

		p_params.m_Attribs = format.data();
		p_params.m_AttribCount = format.size();

		p_params.m_PrimitiveType = m_Collection->GetPrimType();

		p_params.m_BlendState = GetDefaultBlendState(m_Device);
		p_params.m_DepthTest = true;
		p_params.m_DepthWrite = true;
		p_params.m_DepthFunc = LESS;
		m_Pipeline = CreatePipeline(m_Device, p_params);
	}
}
void CPBRTask::SetCameraLookAt(vec3 eye, vec3 target, vec3 up) 
{
	CRenderTask::SetCameraLookAt(eye, target, up);
	m_ViewportProvider.Get().view = m_Camera.GetViewTransform();
	vec3 lightPos = { 6.18, 15, -19 };
	m_ViewportProvider.Get().lightDir = normalize(-lightPos);
	m_ViewportProvider.Get().projection = m_Camera.ProjectPerspective(PI / 4, 720.0f / 1280.0f, 0.5, 20);
	m_ViewportProvider.Get().camPos = eye;
};


