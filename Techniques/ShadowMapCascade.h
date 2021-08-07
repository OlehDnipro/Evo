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

#include "RenderTask.h"
#define SHADOW_MAP_CASCADE_COUNT 4


struct SShadow
{
	static const char* GetName() { return "ShadowConst"; }
	float4x4 lightViewProjection[SHADOW_MAP_CASCADE_COUNT];
	float4 cascadePlanes;
};
struct SReflect
{
	static const char* GetName() { return "ReflectConst"; }
	float4x4 reflectMatrix;
	float4x4 reflectMatrixTInv;
	float4 plane;
};


class CReflectProvider : public CParameterProviderBase<CReflectProvider>
{
public:
	CConstantParameter<SReflect> m_Const;
	static void CreateParameterMap()
	{
		CReflectProvider p;
		m_Layout.AddParameter(SReflect::GetName(), (uint8_t*)p.m_Const.GetPtr() - (uint8_t*)&p.m_pBase);
	}
	SReflect& Get() { return m_Const.Get(); }
	void PrepareConstantBuffer(Context context, SResourceDesc* param) { m_Const.PrepareBuffer(context); }
};


class CShadowParameterProvider: public CParameterProviderBase<CShadowParameterProvider>
{
public:
	SResourceDesc m_ShadowCascades;
	CConstantParameter<SShadow> m_Const;
	static void CreateParameterMap()
	{
		CShadowParameterProvider p;
		m_Layout.AddParameter(SShadow::GetName(), (uint8_t*)p.m_Const.GetPtr() - (uint8_t*)&p.m_pBase);
		m_Layout.AddParameter("ShadowMapCascades", (uint8_t*)&p.m_ShadowCascades - (uint8_t*)&p.m_pBase);
	}
	SShadow& Get() { return m_Const.Get(); }
	void PrepareConstantBuffer(Context context, SResourceDesc* param) { m_Const.PrepareBuffer(context); }

};

class ShadowMapCascade: public CRenderTask
{
public:
	~ShadowMapCascade();
	enum PassEnum
	{
		ShadowPass,
		MainPass,
		NoShadow,
		Reflection,
		Count
	};
	void SetShadowMap(Texture shadowMap){m_ShadowProvider.m_ShadowCascades = shadowMap;};
	bool CreateResources(Device device);
	void SetPassParameters(Context context, PassEnum passId, int cascade = -1);
	void Draw(Context context);
	void Update(Context context);
	void SetCameraLookAt(vec3 eye, vec3 target,vec3 up);
	void SetCubeProjection(bool cube);
	void SetPlanarReflectionParam(float4x4 mtx, float4 plane);
	Camera const& GetCamera() { return m_Camera; }
private:


	Pipeline m_Pipeline[PassEnum::Count];
	PassEnum m_currentPass;

	CViewportParameterProvider m_ViewportProvider;
	CViewportParameterProvider m_ShadowViewportProvider[SHADOW_MAP_CASCADE_COUNT];

	CShadowParameterProvider m_ShadowProvider;
	CReflectProvider m_ReflectProvider;
	SPerFrame m_perFrame;
	SamplerTable m_SamplerTable;
	int m_curCascade;
};
