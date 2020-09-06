
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

bool ShadowMapCascade::CreateResources(Device device)
{
	if ((m_RootSig = CreateRootSignature(device, NShadowMapCascade::RootSig)) == nullptr) return false;

	m_Objects.resize(3);
	m_Objects[0].m_Model.loadFromFile("../../models/terrain_simple.dae", vertexLayout, 1.0f, device);
	m_Objects[1].m_Model.loadFromFile("../../models/oak_trunk.dae", vertexLayout, 2.0f, device);
	m_Objects[2].m_Model.loadFromFile("../../models/oak_leafs.dae", vertexLayout, 2.0f, device);

	m_Objects[0].m_Texture = CreateTexture(device, "../../Textures/gridlines.dds");
	m_Objects[1].m_Texture = CreateTexture(device, "../../Textures/oak_bark.dds");
	m_Objects[2].m_Texture = CreateTexture(device, "../../Textures/oak_leafs.dds");
	for (int i = 0; i < 3; i++)
	{
		SBufferParams cb_params = { sizeof(SPerModel), HeapType::HEAP_DEFAULT, Usage::CONSTANT_BUFFER, "" };
		m_Objects[i].m_CB = CreateBuffer(device, cb_params);
		float4x4 mtx; mtx.identity();
		
		SMapBufferParams map_cb(GetMainContext(device), m_Objects[i].m_CB, 0, sizeof(SPerModel));
		void* data = MapBuffer(map_cb);
		memcpy(data, &mtx, sizeof(SPerModel));
		UnmapBuffer(map_cb);

		SResourceDesc constants[] = { m_Objects[i].m_CB };
		if ((m_Objects[i].m_ConstantTable = CreateResourceTable(device, m_RootSig, NShadowMapCascade::ModelConst, constants)) == nullptr) return false;
		SResourceDesc resources[] = { m_Objects[i].m_Texture };
		if ((m_Objects[i].m_ResourceTable = CreateResourceTable(device, m_RootSig, NShadowMapCascade::ModelResources, resources)) == nullptr) return false;

	}
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
	p_params.m_RenderPass = GetBackBufferRenderPass(device);

	p_params.m_VS = NShadowMapCascade::VertexShader;
	p_params.m_PS = NShadowMapCascade::PixelShader;
		
	p_params.m_Attribs = format;
	p_params.m_AttribCount = 4;

	p_params.m_PrimitiveType = PRIM_TRIANGLES;

	p_params.m_BlendState = GetDefaultBlendState(device);
	m_Pipeline = CreatePipeline(device, p_params);

	const SSamplerDesc samplers[] = { FILTER_TRILINEAR, 1, AM_WRAP, AM_WRAP, AM_WRAP };
	if ((m_SamplerTable = CreateSamplerTable(device, m_RootSig, NShadowMapCascade::Samplers, samplers)) == nullptr) return false;
	
	SBufferParams cb_params = { sizeof(SPerFrame), HeapType::HEAP_DEFAULT, Usage::CONSTANT_BUFFER, "" };
	m_FrameCB = CreateBuffer(device, cb_params);

	m_viewMatrix = float4x4(
		 1, -0.04,  -0.15,   0 ,
		 0,      1, -0.3, 0,
		 0.15, 0.3, 2, 0,
		-0.5, 0.5, -2.5, 1
	);
	m_projMatrix = PerspectiveMatrix(PI / 4, 600.0f / 800.0f, 0.5, 48);
	lightDir = { -6.18, 20, -19 };
	SMapBufferParams map_cb(GetMainContext(device), m_FrameCB, 0, sizeof(SPerFrame));
	char* data = (char*)MapBuffer(map_cb);
	memcpy(data, &m_projMatrix, sizeof(float4x4));
	memcpy(data + sizeof(float4x4), &m_viewMatrix, sizeof(float4x4));
	memcpy(data + 2*sizeof(float4x4), &lightDir, sizeof(float3));
	UnmapBuffer(map_cb);

	SResourceDesc constants[] = { m_FrameCB };
	if ((m_FrameConstantTable = CreateResourceTable(device, m_RootSig, NShadowMapCascade::FrameConst, constants)) == nullptr) return false;
	
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

void ShadowMapCascade::Draw(Context context)
{

	SetRootSignature(context, m_RootSig);
	SetGraphicsResourceTable(context, NShadowMapCascade::FrameConst, m_FrameConstantTable);
	SetGraphicsSamplerTable(context, NShadowMapCascade::Samplers, m_SamplerTable);
	SetPipeline(context, m_Pipeline);
	for (int i = 0; i < 3; i++)
	{
		SetGraphicsResourceTable(context, NShadowMapCascade::ModelConst, m_Objects[i].m_ConstantTable);
		SetGraphicsResourceTable(context, NShadowMapCascade::ModelResources, m_Objects[i].m_ResourceTable);
		SetVertexSetup(context, m_Objects[i].m_Model.GetVertexSetup());
		DrawIndexed(context, 0, m_Objects[i].m_Model.indexCount);
	}

}

