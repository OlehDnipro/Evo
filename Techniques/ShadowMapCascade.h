

#pragma once

#include "RenderTask.h"
#define SHADOW_MAP_CASCADE_COUNT 4

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

class ShadowMapCascade: public CRenderTask
{
public:
	~ShadowMapCascade();
	enum PassEnum
	{
		ShadowPass,
		MainPass,
		Count
	};
	void SetShadowMap(Texture shadowMap){m_ShadowProvider.m_ShadowCascades = shadowMap;};
	bool CreateResources(Device device);
	void SetPassParameters(RenderPass pass, PassEnum passId, int cascade = -1);
	void Draw(Context context);
	void Update(Context context);
	void SetCameraLookAt(vec3 eye, vec3 target,vec3 up);
	Camera const& GetCamera() { return m_Camera; }
private:


	Pipeline m_Pipeline[PassEnum::Count];
	PassEnum m_currentPass;

	CViewportParameterProvider m_ViewportProvider;
	CViewportParameterProvider m_ShadowViewportProvider[SHADOW_MAP_CASCADE_COUNT];

	CShadowParameterProvider m_ShadowProvider;

	SPerFrame m_perFrame;
	SamplerTable m_SamplerTable;
	int m_curCascade;
};
