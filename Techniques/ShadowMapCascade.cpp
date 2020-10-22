
/* * * * * * * * * * * * * Author's note * * * * * * * * * * * *\
*   _       _   _       _   _       _   _       _     _ _ _ _   *
*  |_|     |_| |_|     |_| |_|_   _|_| |_|     |_|  _|_|_|_|_|  *
*  |_|_ _ _|_| |_|     |_| |_|_|_|_|_| |_|     |_| |_|_ _ _     *
*  |_|_|_|_|_| |_|     |_| |_| |_| |_| |_|     |_|   |_|_|_|_   *
*  |_|     |_| |_|_ _ _|_| |_|     |_| |_|_ _ _|_|  _ _ _ _|_|  *
*  |_|     |_|   |_|_|_|   |_|     |_|   |_|_|_|   |_|_|_|_|    *
*                                                               *
*                     http://www.humus.name                     *
*                                                                *
* This file is a part of the work done by Humus. You are free to   *
* use the code in any way you like, modified, unmodified or copied   *
* into your own work. However, I expect you to respect these points:  *
*  - If you use this file and its contents unmodified, or use a major *
*    part of this file, please credit the author and leave this note. *
*  - For use in anything commercial, please request my approval.     *
*  - Share your work and ideas too as much as you can.             *
*                                                                *
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "ShadowMapCascade.h"
#include "Shaders\Tech\ShadowMapCascade.pipeline.h"

//const uint VB_SIZE = 16*1024;
CParameterProviderLayout CParameterProviderBase<CViewportParameterProvider>::m_Layout = CParameterProviderLayout(&CViewportParameterProvider::CreateParameterMap);
CParameterProviderLayout CParameterProviderBase<CShadowParameterProvider>::m_Layout = CParameterProviderLayout(&CShadowParameterProvider::CreateParameterMap);
CParameterProviderLayout CParameterProviderBase<CModelParameterProvider>::m_Layout = CParameterProviderLayout(&CModelParameterProvider::CreateParameterMap);

//#define PROV

ShadowMapCascade::ShadowMapCascade()
{
	
}

ShadowMapCascade::~ShadowMapCascade()
{
}
bool ShadowMapCascade::SimpleObject::Init(Device device, VertexLayout layout,const char* model, const char* texture, float scale)
{
	m_Model.loadFromFile(model, layout, scale, device);
	m_Texture = CreateTexture(device, texture);
	return true;
}
ShadowMapCascade::SimpleObjectInstance::SimpleObjectInstance(const SimpleObject& object, Device device, RootSignature root, float4x4 mtx) :m_Object(object)
{
	SBufferParams cb_params = { sizeof(SPerModel), HeapType::HEAP_DEFAULT, Usage::CONSTANT_BUFFER, "" };
	m_Provider.Init(device);
	m_Provider.Get().model = mtx;
	m_Provider.m_ModelTexture = m_Object.m_Texture ;
}
void ShadowMapCascade::SimpleObjectInstance::Draw(Context context)
{
	SetVertexSetup(context, m_Object.m_Model.GetVertexSetup());
	DrawIndexed(context, 0, m_Object.m_Model.indexCount);
}
void ShadowMapCascade::SetCameraLookAt(vec3 eye, vec3 target, vec3 up)
{
	m_Camera.lookat(eye, target, up);
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
void ShadowMapCascade::FindRootResource(ItemType type, uint slot, uint binding, uint first_item_of_table_with_size, void* receiver)
{
	ShadowMapCascade* shadow = (ShadowMapCascade*)receiver;
	if (first_item_of_table_with_size)
	{
		TableUpdate update;
		update.slot = slot;
		update.m_Descriptors.resize(first_item_of_table_with_size);
		shadow->m_TableUpdates.push_back(update);
	}
	const char* name = NShadowMapCascade::RootItemNames[slot][binding];
	uint32_t layout, offset;
	bool new_search = true;
	while(CParameterProviderRegistry::GetInstane()->FindNextLayout(name, new_search, layout, offset))
	{
		new_search = false;
		shadow->m_ProviderUsage[layout].push_back({ type, offset, slot, binding });
	}
}

bool ShadowMapCascade::CreateResources(Device device, Texture shadowMap)
{
	for (auto& prov : m_ShadowViewportProvider)
	{
		prov.Init(device);
	}
	m_ViewportProvider.Init(device);
	m_ShadowProvider.Init(device);

	if ((m_RootSig = CreateRootSignature(device, NShadowMapCascade::RootSig)) == nullptr) return false;
	SBufferParams cb_params = { sizeof(SPerFrame), HeapType::HEAP_DEFAULT, Usage::CONSTANT_BUFFER, "" };
	m_ShadowProvider.m_ShadowCascades = shadowMap;

	float4x4 mtx; mtx.identity();
	m_Objects.resize(3);
	m_Objects[0].Init(device, vertexLayout, "../../models/terrain_simple.dae", "../../Textures/gridlines.dds", 1.0f);
	m_Objects[1].Init(device, vertexLayout, "../../models/oak_trunk.dae", "../../Textures/oak_bark.dds", 2.0f);
	m_Objects[2].Init(device, vertexLayout, "../../models/oak_leafs.dae", "../../Textures/oak_leafs.dds", 2.0f);
	m_ObjectInstances.resize(11);
	m_ObjectInstances[0] = new SimpleObjectInstance(m_Objects[0], device, m_RootSig, mtx);

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
			m_ObjectInstances[i] = new SimpleObjectInstance(m_Objects[1], device, m_RootSig, mtx);
		}
		else
		{
			m_ObjectInstances[i] = new SimpleObjectInstance(m_Objects[2], device, m_RootSig, mtx);
		}
	}

	const SSamplerDesc samplers[] = { { FILTER_TRILINEAR, 1, AM_WRAP, AM_WRAP, AM_WRAP, ALWAYS }, { FILTER_LINEAR, 1, AM_WRAP, AM_WRAP, AM_WRAP, LESS } };
	if ((m_SamplerTable = CreateSamplerTable(device, m_RootSig, NShadowMapCascade::Samplers, samplers)) == nullptr) return false;

	m_ResourceTable = CreateResourceTable(device, m_RootSig, NShadowMapCascade::Resources, nullptr, 0);

	IterateRootSignature(m_RootSig, &FindRootResource, this);


	vec3 lightPos = { 6.18, 20, -19 };
	m_ViewportProvider.Get().projection = m_Camera.ProjectPerspective(PI / 4, 720.0f / 1280.0f, 0.5, 20);
	m_ViewportProvider.Get().lightDir = normalize(-lightPos);
		
	/*
	SResourceDesc resoures[] = { m_ShadowMap };
	if ((m_FrameResourcesMain = CreateResourceTable(device, m_RootSig, NShadowMapCascade::FrameResources, resoures)) == nullptr) return false;
	
	STextureParams db_params;
	db_params.m_Width = 4;
	db_params.m_Height = 4;
	db_params.m_Format = IMGFMT_RGBA8;
	db_params.m_MSAASampleCount = 1;
	m_defTexture = CreateTexture(device, db_params);

	SResourceDesc resouresSM[] = { m_defTexture };
	if ((m_FrameResourcesSM = CreateResourceTable(device, m_RootSig, NShadowMapCascade::FrameResources, resouresSM)) == nullptr) return false;
	*/
	return true;
}

void ShadowMapCascade::DestroyResources(Device device)
{
/*for (uint i = 0; i < elementsof(m_Setup); i++)
	{
		for (int p = 0; p < 3; p++)
		{
			DestroyPipeline(device, m_Setup[i].m_Pipeline[p][0]);
			DestroyPipeline(device, m_Setup[i].m_Pipeline[p][1]);
		}

		DestroyVertexSetup(device, m_Setup[i].VertexSetup);

		DestroyBuffer(device, m_Setup[i].VertexBuffer);
	}

	DestroyBuffer(device, m_MatrixCB);
	DestroyBuffer(device, m_ColorCB);*/
	DestroyRootSignature(device, m_RootSig);
}



void ShadowMapCascade::SetMatrix(Context context, const float4x4 &matrix)
{
/*	SMapBufferParams map_cb(context, m_MatrixCB, 0, sizeof(float4x4));
	void* data = MapBuffer(map_cb);
	memcpy(data, &matrix, sizeof(float4x4));
	UnmapBuffer(map_cb);*/
}
void ShadowMapCascade::Update(Context  context, int cascade)
{
	if (cascade >= 0)
	{
		SPerFrame& localperFrame = m_ShadowViewportProvider[cascade].Get();
		float4x4 mat; mat.identity();
		localperFrame.projection = mat;
		localperFrame.view = m_ShadowProvider.Get().lightViewProjection[cascade];
		m_ViewportProvider.Get() = localperFrame;
	}
	else
	{
		m_ViewportProvider.Get() = m_perFrame;
	}

	/*SPerFrame localperFrame = m_perFrame;

	if (cascade >= 0)
	{
		float4x4 mat; mat.identity();
		localperFrame.projection = mat;
		localperFrame.view = m_perFrame.lightViewProjection[cascade];
	}
	int s = sizeof(SPerFrame);
	SMapBufferParams map_cb(context, m_FrameCB, 0, sizeof(SPerFrame));
	char* data = (char*)MapBuffer(map_cb);
	memcpy(data, &localperFrame, sizeof(SPerFrame));
	UnmapBuffer(map_cb);*/
}
void ShadowMapCascade::PrepareDraw(Device device,RenderPass pass, PassEnum passId)
{
	m_currentPass = passId;
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
		p_params.m_RootSignature = m_RootSig;
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

}

void ShadowMapCascade::GatherParameters(IParameterProvider** providers, uint count)
{
	for (int i = 0; i < count; i++)
	{
		uint32_t id = providers[i]->GetLayoutId();
		auto usage = m_ProviderUsage.find(id);
		if (usage != m_ProviderUsage.end())
		{
			for (auto& param : usage->second)
			{
				SResourceDesc& write = m_TableUpdates[param.m_slot].m_Descriptors[param.m_binding];
				switch (param.m_type)
				{
				case TEXTURE:
				case RWTEXTURE:
					write = *(Texture*)(providers[i]->GetBaseParameterPointer() + param.m_provMemOffset);

					break;

				case STRUCTUREDBUFFER:
				case RWSTRUCTUREDBUFFER:
					write = *(Buffer*)(providers[i]->GetBaseParameterPointer() + param.m_provMemOffset);
				case CBV:
					SConstantData& constants = *(SConstantData*)(providers[i]->GetBaseParameterPointer() + param.m_provMemOffset);
					write = constants.buffer;
					write.m_Desc.bView.offset = constants.offset;
					write.m_Desc.bView.size = constants.size;

				}
			}
		}
	}
}
void ShadowMapCascade::Draw(Context context, int cascade)
{
	SetRootSignature(context, m_RootSig);
	vector<IParameterProvider*> providers = { cascade < 0 ? (IParameterProvider*)&m_ViewportProvider :
															(IParameterProvider*)&m_ShadowViewportProvider[cascade],
															(IParameterProvider*)&m_ShadowProvider };
	SetGraphicsSamplerTable(context, NShadowMapCascade::Samplers, m_SamplerTable);
	GatherParameters(providers.data(), 2);
	UpdateResourceTable(GetDevice(context), m_RootSig, NShadowMapCascade::Resources, m_ResourceTable,
	m_TableUpdates[NShadowMapCascade::Resources].m_Descriptors.data(), 0, NShadowMapCascade::ModelConst);


	SetPipeline(context, m_Pipeline[m_currentPass]);
	for (int i = 0; i < 11; i++)
	{
		providers[0] = m_ObjectInstances[i]->GetModelProvider();
		GatherParameters(providers.data(), 1);
		UpdateResourceTable(GetDevice(context), m_RootSig, NShadowMapCascade::Resources, m_ResourceTable,
			m_TableUpdates[NShadowMapCascade::Resources].m_Descriptors.data(), NShadowMapCascade::ModelConst, 2);

			SetGraphicsResourceTable(context, NShadowMapCascade::Resources, m_ResourceTable);

		m_ObjectInstances[i]->Draw(context);
	}
}

