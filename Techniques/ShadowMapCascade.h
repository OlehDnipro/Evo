
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
#include "../Humus/Math/Vector.h"
#include "../Humus/Util/ModelDae.h"
using namespace Dae;
#include <vector>
class Camera
{
	float4x4 m_toViewSpace, m_toWorldSpace;
public:
		Camera() { m_toViewSpace.identity(); m_toWorldSpace.identity(); }
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
		float4x4 GetViewTransformInverse()const { return m_toWorldSpace; }

};
class ShadowMapCascade
{
public:
	ShadowMapCascade();
	~ShadowMapCascade();

	bool CreateResources(Device device, RenderPass pass);
	void DestroyResources(Device device);

	void SetMatrix(Context context, const float4x4 &matrix);
	//RootSignature GetRootSignature() const { return m_RootSig; }
	void Draw(Context context);
	void SetCameraLookAt(vec3 eye, vec3 target,vec3 up);
	Camera const& GetCamera() { return m_Camera; }

private:
	Dae::VertexLayout vertexLayout = Dae::VertexLayout({
										VERTEX_COMPONENT_POSITION,
										VERTEX_COMPONENT_UV,
										VERTEX_COMPONENT_COLOR,
										VERTEX_COMPONENT_NORMAL,
									 });
	struct SPerFrame
	{
		float4x4 projection;
		float4x4 view;
		float3 lightDir;
	};
	struct SPerModel
	{
		float4x4 model;
	};
	Camera m_Camera;
	class SimpleObjectInstance;
	class SimpleObject
	{
		Dae::Model m_Model;
		Texture m_Texture;
		friend class SimpleObjectInstance;
		public:
		bool Init(Device device, VertexLayout layout, const char* model, const char* texture, float scale);
	};
	class SimpleObjectInstance
	{
		const SimpleObject& m_Object;
		Buffer m_CB;
		float4x4 m_ModelMatrix;
		ResourceTable m_ConstantTable;
		ResourceTable m_ResourceTable;
	public:
		SimpleObjectInstance(const SimpleObject& object, Device device, RootSignature root, float4x4 mtx);
		void Draw(Context context);
	};
	std::vector<SimpleObject> m_Objects;
	std::vector<SimpleObjectInstance*> m_ObjectInstances;
	RootSignature m_RootSig;
	Pipeline m_Pipeline;

	float4x4 m_viewMatrix, m_projMatrix;
	float3 lightDir;
	Buffer m_FrameCB;
	ResourceTable m_FrameConstantTable;

	SamplerTable m_SamplerTable;
	
};
