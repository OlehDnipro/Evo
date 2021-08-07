/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\ 
*Copyright 2021 Oleh Sopilniak																																	*
*																																								*
* Permission is hereby granted, free of charge, to any person obtaining a copy of this software																	*
* and associated documentation files (the "Software"), to deal in the Software without restriction,																*
* including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,															*
* and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,															*
*  subject to the following conditions:																															*
*																																								*
* The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.								*	
*																																								*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,			*
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, *
* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.    *
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  */

#pragma once

#include "../Humus/Graphics/Renderer.h"
#include "../Humus/Graphics/RootSignature.h"
#include "../Humus/Math/Vector.h"
#include "../Humus/Util/ModelDae.h"
#include "../ParameterProviderRegistry.h"
using namespace Dae;
#include <vector>
#include <map>
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
class IParameterProvider
{
public:
	virtual uint32_t GetLayoutId() = 0;
	uint8_t* GetBaseParameterPointer() { return &m_pBase; }
	virtual void PrepareConstantBuffer(Context context, SResourceDesc* paramPtr) {};
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

template<class T>
class CConstantParameter
{
	SResourceDesc m_param;
	T m_data;
public:
	T& Get() { return m_data; }
	void PrepareBuffer(Context context)
	{
		uint size = sizeof(T);
#if GRAPHICS_API_D3D12
		size = ((size - 1) / 256 + 1) * 256;
#endif
		uint offset = AllocateConstantsSlice(context, size );
		
		m_param = { GetConstantBuffer(context), { offset, size} };
		uint8* gpuDest =  GetConstantBufferData(context, m_param.m_bufRange.offset);
		memcpy(gpuDest, &m_data, sizeof(T));
	}
	SResourceDesc* GetPtr() { return &m_param; }
};
struct SPerModel
{
	float4x4 model;
	uint alphaCheck;
	uint pad[3];
	static const char* GetName() { return "ModelConst"; }
};

struct SPerFrame
{
	static const char* GetName() { return "ViewportConst"; }
	float4x4 projection;
	float4x4 view;
	float3 lightDir;
	float pad;
	float3 camPos;
	float pad1;
};
struct SPBRModel
{
	float4x4 model;
	float4 material;
	//roughness, metallness, reflectance,..
	float3 baseColor;
	float pad;
	static const char* GetName() { return "ModelConstPBR"; }
};
class CModelParameterProvider : public CParameterProviderBase<CModelParameterProvider>
{
public:
	SResourceDesc m_ModelTexture;
	CConstantParameter<SPerModel> m_ModelConst;
	static void CreateParameterMap()
	{
		CModelParameterProvider p;
		m_Layout.AddParameter(SPerModel::GetName(), (uint8_t*)p.m_ModelConst.GetPtr() - (uint8_t*)&p.m_pBase);
		m_Layout.AddParameter("ModelTexture", (uint8_t*)&p.m_ModelTexture - (uint8_t*)&p.m_pBase);
	}
	SPerModel& Get() { return m_ModelConst.Get(); }
	void PrepareConstantBuffer(Context context, SResourceDesc* param) { m_ModelConst.PrepareBuffer(context); }
};

class CViewportParameterProvider : public CParameterProviderBase<CViewportParameterProvider>
{
public:
	CConstantParameter<SPerFrame> m_Const;
	static void CreateParameterMap()
	{
		CViewportParameterProvider p;
		m_Layout.AddParameter(SPerFrame::GetName(), (uint8_t*)p.m_Const.GetPtr() - (uint8_t*)&p.m_pBase);
	}
	SPerFrame& Get() { return m_Const.Get(); }
	void PrepareConstantBuffer(Context context, SResourceDesc* param) { m_Const.PrepareBuffer(context); }
};
class SimpleObjectInstance;
class SimpleObject
{
	Dae::Model m_Model;
	Texture m_Texture;
	friend class SimpleObjectInstance;
	int m_Mark = 0;
	bool m_ShadowsAlphaCheck = false;
	public:
	bool Init(Device device, VertexLayout layout, const char* model, const char* texture, float scale, bool depthApha = false);
	int GetMark() const{ return m_Mark; }
	void SetMark(int mark) { m_Mark = mark; }
	Texture GetTexture() { return m_Texture; }
	bool ShadowsAlphaCheck() const { return m_ShadowsAlphaCheck; }
};
class SimpleObjectInstance
{
	const SimpleObject& m_Object;
	CModelParameterProvider m_Provider;
public:
	SimpleObjectInstance(const SimpleObject& object, Device device, float4x4 mtx);
	void Draw(Context context);
	const SimpleObject& GetBase() { return m_Object; }
	IParameterProvider* GetModelProvider() { return (IParameterProvider*)&m_Provider; };
};
struct SRootResourceAttrs
{
	const char* name;
	const char* type;
};

class CShaderCache
{
	typedef SRootResourceAttrs(*TGetResourceAttr)(uint slot, uint bind);
	TGetResourceAttr m_getResourceAttrs;
public:
	void GatherParameters(const vector<SResourceDesc> &resources, uint slot, uint offset = 0);
	void GatherParameters(Context context, IParameterProvider** providers,  uint count);
	bool CreateRootSignature(Device device, const SCodeBlob& code, TGetResourceAttr getAttr = nullptr);
	void DestroyRootSignature(Device device);
	RootSignature GetRootSignature() { return m_RootSig; }
	void UpdateResourceTable(Device, uint32 slot, ResourceTable table);
	uint32 GetDescriptorsCount(uint32 slot);
private:
	struct TableUpdate
	{
		uint32_t slot;
		vector<SResourceDesc> m_Descriptors;
		vector<uint8> m_ReflectionFlags;
	};
	vector<TableUpdate> m_TableUpdates;
	struct SRequiredParameter
	{
		ItemType m_type;
		uint32_t m_provMemOffset;
		uint32_t m_slot, m_binding;
	};
	std::map<uint32_t, vector<SRequiredParameter>> m_ProviderUsage;
	static void FindRootResource(ItemType type, uint slot, uint binding, uint first_item_of_table_with_size, void* receiver);

	RootSignature m_RootSig;

};
class IGeometryCollection
{
public:
	virtual ~IGeometryCollection() {};
	virtual void Create(Device device) = 0;
	virtual void Draw(Context context, CShaderCache& cache, int resources_slot) = 0;
	virtual void DefineVertexFormat(vector<AttribDesc>& format) = 0;
	virtual PrimitiveType GetPrimType() = 0;
};

class IObjectCollection:public IGeometryCollection
{
protected:
	std::vector<SimpleObject> m_Objects;
	std::vector<SimpleObjectInstance*> m_ObjectInstances;
	Dae::VertexLayout m_vertexLayout;
	int m_FilteredMark = -1;
public:
	IObjectCollection() {};
	virtual void Draw(Context context, CShaderCache& cache, int resources_slot);
	void SetFilter(uint filter) { m_FilteredMark = filter; };
	virtual ~IObjectCollection();
};

class CComputeTask
{
	virtual bool CreateResources(Device device) = 0;
protected:
	CShaderCache m_Cache;
	Device m_Device;
};
class CRenderTask
{
public:
	virtual bool CreateResources(Device device) = 0;
	virtual void Draw(Context context) = 0;
	virtual void SetCameraLookAt(vec3 eye, vec3 target,vec3 up){m_Camera.lookat(eye, target, up);};
	virtual void SetGeometry(IGeometryCollection* collection) { m_Collection = collection; };
	Camera const& GetCamera() { return m_Camera; }
	virtual ~CRenderTask() {};
protected:
	Camera m_Camera;
	IGeometryCollection* m_Collection = nullptr;
	CShaderCache m_Cache;
	Device m_Device;	
};
