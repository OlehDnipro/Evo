
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
#include "ShadowMapCascade.pipeline.h"

//const uint VB_SIZE = 16*1024;


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
ShadowMapCascade::SimpleObjectInstance::SimpleObjectInstance(const SimpleObject& object, Device device, RootSignature root, float4x4 mtx) :m_Object(object), m_ModelMatrix(mtx)
{
	SBufferParams cb_params = { sizeof(SPerModel), HeapType::HEAP_DEFAULT, Usage::CONSTANT_BUFFER, "" };
	m_CB = CreateBuffer(device, cb_params);

	SMapBufferParams map_cb(GetMainContext(device), m_CB, 0, sizeof(SPerModel));
	void* data = MapBuffer(map_cb);
	memcpy(data, &m_ModelMatrix, sizeof(SPerModel));
	UnmapBuffer(map_cb);

	SResourceDesc constants[] = { m_CB };
	m_ConstantTable = CreateResourceTable(device, root, NShadowMapCascade::ModelConst, constants);
	SResourceDesc resources[] = { m_Object.m_Texture };
	m_ResourceTable = CreateResourceTable(device, root, NShadowMapCascade::ModelResources, resources);

}
void ShadowMapCascade::SimpleObjectInstance::Draw(Context context)
{
	SetGraphicsResourceTable(context, NShadowMapCascade::ModelConst, m_ConstantTable);
	SetGraphicsResourceTable(context, NShadowMapCascade::ModelResources, m_ResourceTable);
	SetVertexSetup(context, m_Object.m_Model.GetVertexSetup());
	DrawIndexed(context, 0, m_Object.m_Model.indexCount);
}
void ShadowMapCascade::SetCameraLookAt(vec3 eye, vec3 target, vec3 up)
{
	m_Camera.lookat(eye, target, up);
	m_perFrame.view = m_Camera.GetViewTransform();

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
	float4x4 toWorld = inverse(mul(m_perFrame.projection, m_perFrame.view));


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
		m_perFrame.cascadePlanes[i] = (lerped - nearPlane) / (farPlane - nearPlane);
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
			frustumCascade[i + 4] = frustumView[i] + diff[i] * m_perFrame.cascadePlanes[casc];
			center += frustumCascade[i];
			center += frustumCascade[i + 4];
		}
		lastSplit = m_perFrame.cascadePlanes[casc];
		m_perFrame.cascadePlanes[casc] = nearPlane + (farPlane - nearPlane) * m_perFrame.cascadePlanes[casc];
		center /= 8;
		for (int i = 0; i < points; i++)
		{
			float dist = length(center - frustumCascade[i]);
			radius = max(radius, dist);
		}
		vec3 _center(center.x, center.y, center.z);
		vec3 light_eye = _center - m_perFrame.lightDir * radius;
		Camera cam;
		cam.lookat(light_eye, _center, vec3(0, 1, 0));
		m_perFrame.lightViewProjection[casc] = mul(ProjectiveMatrix(2 * radius, 2 * radius, 0, 2 * radius), cam.GetViewTransform());
	}
}

bool ShadowMapCascade::CreateResources(Device device, Texture shadowMap)
{
	if ((m_RootSig = CreateRootSignature(device, NShadowMapCascade::RootSig)) == nullptr) return false;
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
	
	SBufferParams cb_params = { sizeof(SPerFrame), HeapType::HEAP_DEFAULT, Usage::CONSTANT_BUFFER, "" };
	m_FrameCB = CreateBuffer(device, cb_params);
	vec3 lightPos = { 6.18, 20, -19 };
	m_perFrame.projection = m_Camera.ProjectPerspective(PI / 4, 720.0f / 1280.0f, 0.5, 20);
	m_perFrame.lightDir = normalize(-lightPos);
	
	SResourceDesc constants[] = { m_FrameCB };
	if ((m_FrameConstantTable = CreateResourceTable(device, m_RootSig, NShadowMapCascade::FrameConst, constants)) == nullptr) return false;

	m_ShadowMap = shadowMap;

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
	DestroyBuffer(device, m_ColorCB);
	DestroyRootSignature(device, m_RootSig);*/
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
	SPerFrame localperFrame = m_perFrame;

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
	UnmapBuffer(map_cb);
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
void ShadowMapCascade::Draw(Context context)
{
	SetRootSignature(context, m_RootSig);
	SetGraphicsResourceTable(context, NShadowMapCascade::FrameConst, m_FrameConstantTable);
	SetGraphicsResourceTable(context, NShadowMapCascade::FrameResources, m_FrameResourcesMain);
	SetGraphicsSamplerTable(context, NShadowMapCascade::Samplers, m_SamplerTable);
	SetPipeline(context, m_Pipeline[m_currentPass]);
	for (int i = 0; i < 11; i++)
	{
		m_ObjectInstances[i]->Draw(context);
	}
}

