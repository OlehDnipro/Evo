
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
class Camera
{
	float4x4 mtx;
	vec3 m_angle = {}, m_pos = {};
	void update()
	{
		mtx.identity();
		mtx = mul(mtx, RotateX(m_angle.x));
		mtx = mul(mtx, RotateY(m_angle.y));
		mtx = mul(mtx, RotateZ(m_angle.z));
		mtx = mul(translate(m_pos), mtx);//TRS for column-order
	}public:
	Camera() { mtx.identity(); }
	float4x4 lookat(vec3 eye, vec3 center, vec3 up)
	{	
		vec3 const zaxis(normalize(center - eye));
		vec3 const xaxis(normalize(cross(up, zaxis)));
		vec3 const yaxis(cross(zaxis, xaxis));
		float4x4 R; R.identity();
		R(0,0) = xaxis.x;
		R(0,1) = xaxis.y;
		R(0,2) = xaxis.z;
		R(1,0) = yaxis.x;
		R(1,1) = yaxis.y;
		R(1,2) = yaxis.z;
		R(2,0) = zaxis.x;
		R(2,1) = zaxis.y;
		R(2,2) = zaxis.z;
		R.transpose();//inverted=transposed for ortogonal
		float4x4 T; T.identity();//neg - as inverted
		T(0,3) = -eye.x;
		T(1,3) = -eye.y;
		T(2,3) = -eye.z;
		//for column-major TRS
		//(T*R*S)^(-1) = ((S)^(-1))*((R)^(-1))*((T)^(-1))
		mtx = mul(R,T);
		return mtx;
	}
	float4x4 SetRot(vec3 angles)
	{
		m_angle = angles;
		update();
		return mtx;
	}
	float4x4 SetPos(vec3 pos)
	{
		m_pos = pos;
		update();
		return mtx;
	}

	float4x4 get() { return mtx; }
};

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
bool ShadowMapCascade::CreateResources(Device device)
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
	vec3(1.25f, 0.25f, 1.25f),
	vec3(-1.25f, -0.2f, 1.25f),
	vec3(1.25f, 0.1f, -1.25f),
	vec3(-1.25f, -0.25f, -1.25f),
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
	vec3 lightPos = { -6.18, 20, -19 };
	lightDir = normalize(lightPos);
	Camera cam;
	cam.lookat(vec3(0, 5, 0), vec3(0, 0.382, 0), vec3(0, 0, -1));

	m_viewMatrix = cam.get();	
	m_projMatrix = PerspectiveMatrix(PI / 4, 600.0f / 800.0f, 0.001, 100);
	//float4x4 m = mul(m_projMatrix, m_viewMatrix );
	//m.transpose();
	//m_projMatrix = m;
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
	for (int i = 0; i < 11; i++)
	{
		m_ObjectInstances[i]->Draw(context);
	}

}

