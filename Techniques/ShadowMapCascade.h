
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

#pragma once

#include "../Humus/Graphics/Renderer.h"
#include "../Humus/Graphics/Vulkan/VulkanRootSignature.h"
#include "../Humus/Math/Vector.h"
#include "../Humus/Util/ModelDae.h"
#include "../ParameterProviderRegistry.h"
using namespace Dae;
#include <vector>
#include <map>
#define SHADOW_MAP_CASCADE_COUNT 4
class Camera
{
	float4x4 m_toViewSpace, m_toWorldSpace;
	float4x4 m_Project, m_ProjectInverse;
	float m_farPlane, m_nearPlane;
public:
		Camera() { m_toViewSpace.identity(); m_toWorldSpace.identity(); }
		
		float4x4 ProjectPerspective(const float fov, const float ratio, const float z_near, const float z_far) 
		{
			m_nearPlane = z_near;
			m_farPlane = z_far; 
			m_Project = PerspectiveMatrix(fov, ratio, z_near, z_far);
			m_ProjectInverse = inverse(m_Project);
			return m_Project;
		}

		float4x4 ProjectOrtho(const float w, const float h, const float z_near, const float z_far) 
		{
			m_nearPlane = z_near;
			m_farPlane = z_far;
			m_Project = ProjectiveMatrix(w, h, z_near, z_far);
			m_ProjectInverse = inverse(m_Project);
			return m_Project;
		}

		float4x4 lookat(vec3 eye, vec3 center, vec3 up)
		{
			vec3 const zaxis(normalize(center - eye));
			vec3 const xaxis(normalize(cross(up, zaxis)));
			vec3 const yaxis(cross(zaxis, xaxis));
			float4x4 R; R.identity();
			// put vectors to columns(as column-major) to get basis for camera 
			R(0, 0) = xaxis.x;
			R(1, 0) = xaxis.y;
			R(2, 0) = xaxis.z;
			R(0, 1) = yaxis.x;
			R(1, 1) = yaxis.y;
			R(2, 1) = yaxis.z;
			R(0, 2) = zaxis.x;
			R(1, 2) = zaxis.y;
			R(2, 2) = zaxis.z;
			R.transpose();//inverted=transposed for ortogonal to get view matrix
			float4x4 T; T.identity();//neg - as inverted
			T(0, 3) = -eye.x;
			T(1, 3) = -eye.y;
			T(2, 3) = -eye.z;
			//for column-major TRS
			//(T*R*S)^(-1) = ((S)^(-1))*((R)^(-1))*((T)^(-1))
			m_toViewSpace = mul(R, T);
			m_toWorldSpace = inverse(m_toViewSpace);
			return m_toViewSpace;
		}

		float4x4 GetViewTransform()const { return m_toViewSpace; }
		float4x4 GetViewTransformInverse()const { return m_Project; }
		float4x4 GetProjectionInverse()const { return m_ProjectInverse; }
		float4x4 GetProjection()const { return m_Project; }

		float GetFarPlane() { return m_farPlane; }
		float GetNearPlane() { return m_nearPlane; }

};
struct SPerFrame
{
	static const char* GetName() { return "ViewportConst"; }
	float4x4 projection;
	float4x4 view;
	float3 lightDir;
	double pad;
};
struct SShadow
{
	static const char* GetName() { return "ShadowConst"; }
	float4x4 lightViewProjection[SHADOW_MAP_CASCADE_COUNT];
	float4 cascadePlanes;
};
struct SPerModel
{
	float4x4 model;
	static const char* GetName() { return "ModelConst"; }
};
struct SConstantData
{
	Buffer buffer;
	uint offset,size;
};
template<class T>
class CConstantParameter
{
	SConstantData m_param;
	T m_data;
public:
	T& Get() { return m_data; }
	void PrepareBuffer(Context context)
	{
		m_param.offset = AllocateConstantsSlice(context, sizeof(T));
		m_param.buffer = GetConstantBuffer(context);
		m_param.size = sizeof(T);
		uint8* gpuDest =  GetConstantBufferData(context, m_param.offset);
		memcpy(gpuDest, &m_data, sizeof(T));
	}
	SConstantData* GetPtr() { return &m_param; }
};
class IParameterProvider
{
public:
	virtual uint32_t GetLayoutId() = 0;
	uint8_t* GetBaseParameterPointer() { return &m_pBase; }
	virtual void PrepareConstantBuffer(Context context, uint8_t* paramPtr) {};
protected:
	uint8_t m_pBase;
};
template<class T>
class CParameterProviderBase : public IParameterProvider
{
protected:
	static CParameterProviderLayout m_Layout;
public:
	uint32_t GetLayoutId() { return m_Layout.GetId(); };
};


class CModelParameterProvider: public CParameterProviderBase<CModelParameterProvider>
{
public:
	Texture m_ModelTexture;
	CConstantParameter<SPerModel> m_ModelConst;
	static void CreateParameterMap()
	{
		CModelParameterProvider p;
		m_Layout.AddParameter(SPerModel::GetName(), (uint8_t*)p.m_ModelConst.GetPtr() - (uint8_t*)&p.m_pBase);
		m_Layout.AddParameter("ModelTexture", (uint8_t*)&p.m_ModelTexture - (uint8_t*)&p.m_pBase);
	}
	SPerModel& Get() { return m_ModelConst.Get(); }
	void PrepareConstantBuffer(Context context, uint8_t* param) { m_ModelConst.PrepareBuffer(context); }
};

class CViewportParameterProvider : public CParameterProviderBase<CViewportParameterProvider>
{
public:
	CConstantParameter<SPerFrame> m_ModelConst;
	static void CreateParameterMap()
	{
		CViewportParameterProvider p;
		m_Layout.AddParameter(SPerFrame::GetName(), (uint8_t*)p.m_ModelConst.GetPtr() - (uint8_t*)&p.m_pBase);
	}
	SPerFrame& Get() { return m_ModelConst.Get(); }
	void PrepareConstantBuffer(Context context, uint8_t* param) { m_ModelConst.PrepareBuffer(context); }
};

class CShadowParameterProvider: public CParameterProviderBase<CShadowParameterProvider>
{
public:
	Texture m_ShadowCascades;
	CConstantParameter<SShadow> m_ModelConst;
	static void CreateParameterMap()
	{
		CShadowParameterProvider p;
		m_Layout.AddParameter(SShadow::GetName(), (uint8_t*)p.m_ModelConst.GetPtr() - (uint8_t*)&p.m_pBase);
		m_Layout.AddParameter("ShadowMapCascades", (uint8_t*)&p.m_ShadowCascades - (uint8_t*)&p.m_pBase);
	}
	SShadow& Get() { return m_ModelConst.Get(); }
	void PrepareConstantBuffer(Context context, uint8_t* param) { m_ModelConst.PrepareBuffer(context); }

};
class ShadowMapCascade
{
public:

	ShadowMapCascade();
	~ShadowMapCascade();
	enum PassEnum
	{
		ShadowPass,
		MainPass,
		Count
	};
	bool CreateResources(Device device, Texture shadowMap);
	void DestroyResources(Device device);

	void SetMatrix(Context context, const float4x4 &matrix);
	//RootSignature GetRootSignature() const { return m_RootSig; }
	void Draw(Context context, int cascade = -1);
	void Update(Context context, int cascade = -1);
	void PrepareDraw(Device device, RenderPass pass, PassEnum passId);
	void SetCameraLookAt(vec3 eye, vec3 target,vec3 up);
	Camera const& GetCamera() { return m_Camera; }


private:
	Dae::VertexLayout vertexLayout = Dae::VertexLayout({
										VERTEX_COMPONENT_POSITION,
										VERTEX_COMPONENT_UV,
										VERTEX_COMPONENT_COLOR,
										VERTEX_COMPONENT_NORMAL,
									 });


	Camera m_Camera;
	class SimpleObjectInstance;
	class SimpleObject
	{
		Dae::Model m_Model;
		Texture m_Texture;
		friend class SimpleObjectInstance;
		public:
		bool Init(Device device, VertexLayout layout, const char* model, const char* texture, float scale);
		Texture GetTexture() { return m_Texture; }
	};
	class SimpleObjectInstance
	{
		const SimpleObject& m_Object;
		CModelParameterProvider m_Provider;
	public:
		SimpleObjectInstance(const SimpleObject& object, Device device, RootSignature root, float4x4 mtx);
		void Draw(Context context);
		IParameterProvider* GetModelProvider() { return (IParameterProvider*)&m_Provider; };
	};
	std::vector<SimpleObject> m_Objects;
	std::vector<SimpleObjectInstance*> m_ObjectInstances;
	struct TableUpdate
	{
		uint32_t slot;
		vector<SResourceDesc> m_Descriptors;
	};
	vector<TableUpdate> m_TableUpdates;
	struct SRequiredParameter
	{
		ItemType m_type;
		uint32_t m_provMemOffset;
		uint32_t m_slot, m_binding;
	};
	std::map<uint32_t, vector<SRequiredParameter>> m_ProviderUsage;
	void GatherParameters( IParameterProvider** providers, Context context, uint count);
	static void FindRootResource(ItemType type, uint slot, uint binding, uint first_item_of_table_with_size, void* receiver);

	RootSignature m_RootSig;
	Pipeline m_Pipeline[PassEnum::Count];
	PassEnum m_currentPass;

	CViewportParameterProvider m_ViewportProvider;
	CViewportParameterProvider m_ShadowViewportProvider[SHADOW_MAP_CASCADE_COUNT];

	CShadowParameterProvider m_ShadowProvider;

	SPerFrame m_perFrame;
	SamplerTable m_SamplerTable;	
};
