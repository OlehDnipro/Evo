
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
//#include "ShadowMapCascade.pipeline.h"

//const uint VB_SIZE = 16*1024;

ShadowMapCascade::ShadowMapCascade()
{
	
}

ShadowMapCascade::~ShadowMapCascade()
{
}

bool ShadowMapCascade::CreateResources(Device device)
{
	m_Models.resize(3);
	m_Models[0].loadFromFile("../../models/terrain_simple.dae", vertexLayout, 1.0f, device);
	m_Models[1].loadFromFile("../../models/oak_trunk.dae", vertexLayout, 2.0f, device);
	m_Models[2].loadFromFile("../../models/oak_leafs.dae", vertexLayout, 2.0f, device);

	m_Textures.resize(3);
	m_Textures[0] = CreateTexture(device, "../../Textures/gridlines.dds");
	m_Textures[1] = CreateTexture(device, "../../Textures/oak_bark.dds");
	m_Textures[2] = CreateTexture(device, "../../Textures/oak_leafs.dds");

	const AttribDesc format[] =
	{
		{ 0, VF_FLOAT3, "Position"   },
		{ 0, VF_FLOAT2, "TexCoord" },
		{ 0, VF_FLOAT3, "Color" },
		{ 0, VF_FLOAT3, "Normal" },
	};
	/*
	if ((m_RootSig = CreateRootSignature(device, NShadowMapCascade::RootSig)) == nullptr) return false;

	SPipelineParams p_params;
	p_params.m_Name = "ShadowMapCascade";
	p_params.m_RootSignature = m_RootSig;
	p_params.m_RenderPass = GetBackBufferRenderPass(device);

	p_params.m_VS = NShadowMapCascade::VS;
	p_params.m_PS = NShadowMapCascade::PS;
		
	p_params.m_Attribs = format;
	p_params.m_AttribCount = 4;

	p_params.m_PrimitiveType = PRIM_TRIANGLES;

	p_params.m_BlendState = GetDefaultBlendState(device);
	m_Pipeline = CreatePipeline(device, p_params);

	SResourceDesc resources[] = {m_ColorCB, m_MatrixCB };
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
/*	if (setup.BufferPos + vertex_count >= VB_SIZE)
		setup.BufferPos = 0;

	SMapBufferParams map_vb(context, setup.VertexBuffer, setup.BufferPos * vertex_stride, vertex_count * vertex_stride);
	void* data = MapBuffer(map_vb);
	memcpy(data, vertices, vertex_count * vertex_stride);
	UnmapBuffer(map_vb);

	SetColor(context, color);

	SetRootSignature(context, m_RootSig);
	//SetGraphicsConstantBuffer(context, NShadowMapCascade::Constants, m_MatrixCB);
	SetGraphicsResourceTable(context, NShadowMapCascade::CB, m_ConstantTable);
	if (resources)
		SetGraphicsResourceTable(context, NShadowMapCascade::Resources, resources);
	if (samplers)
		SetGraphicsSamplerTable(context, NShadowMapCascade::Samplers, samplers);

	SetPipeline(context, setup.m_Pipeline[primitive_type][blend_mode]);

	SetVertexSetup(context, setup.VertexSetup);

	Draw(context, setup.BufferPos, vertex_count);


	setup.BufferPos += vertex_count;*/
}

