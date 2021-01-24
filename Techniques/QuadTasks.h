#include "RenderTask.h"
struct QuadVertex
{
	float3 pos;
	float2 uv;
};

class CTexturedQuadGeometry : public IGeometryCollection
{
public:
	~CTexturedQuadGeometry();
	void Draw(Context context, CShaderCache& cache, int resources_slot);
	void DefineVertexFormat(vector<AttribDesc>& format);
	PrimitiveType GetPrimType() { return PRIM_TRIANGLE_FAN; };
	void SetTexture(Texture tex, uint index = 0) { m_Texture = tex; m_TexDescIndex = index; };
	void Create(Device device);
	void Create(Device device, const QuadVertex (&vtx)[4]);
private:
	Texture m_Texture;
	uint m_TexDescIndex;
	Buffer m_VB;
	VertexSetup m_VertexSetup;
	Device m_Device;
};

class CPolygonGeometry : public IGeometryCollection
{
public:
	CPolygonGeometry(int count) :m_sideCount(count) {};
	void Draw(Context context, CShaderCache& cache, int resources_slot);
	void DefineVertexFormat(vector<AttribDesc>& format);
	PrimitiveType GetPrimType() { return PRIM_TRIANGLE_FAN; };
	void Create(Device device);
	void UpdatePos(float radius, float2 pos);

private:
	Buffer m_VB;
	VertexSetup m_VertexSetup;
	Device m_Device;
	uint m_sideCount;
};

class CPolygonTask : public CRenderTask
{
public:
	virtual bool CreateResources(Device device);
	virtual void Draw(Context context);
private:
	Pipeline m_Pipeline;
	void InitPipeline(Context context);

};

class CWaterDropTask : public CRenderTask
{
public:
	virtual bool CreateResources(Device device);
	virtual void Draw(Context context);
private:
	void InitPipeline(Context context);
	Pipeline m_Pipeline;
	SamplerTable m_SamplerTable;
};



struct SWaterConst
{
	static const char* GetName() { return "WaterConst"; }
	float4x4 mvp;
	float3 camPos;
	float streamOffset;
	float3 boxMax;
	float pad2;
	float3 boxMin;
	float pad3;
	float3 cubeCenter;
	float waveOffsetScale;
	float3 boxMaxFar;
	float pad5;
	float3 boxMinFar;
	float pad6;
};

class CWaterParameterProvider : public CParameterProviderBase<CWaterParameterProvider>
{
public:
	CConstantParameter<SWaterConst> m_Const;
	static void CreateParameterMap()
	{
		CWaterParameterProvider p;
		m_Layout.AddParameter(SWaterConst::GetName(), (uint8_t*)p.m_Const.GetPtr() - (uint8_t*)&p.m_pBase);
		m_Layout.AddParameter("WaveTex", (uint8_t*)&p.m_WaveTexture - (uint8_t*)&p.m_pBase);
		m_Layout.AddParameter("EnvTex", (uint8_t*)&p.m_EnvTexture - (uint8_t*)&p.m_pBase);
		m_Layout.AddParameter("PlanarReflection", (uint8_t*)&p.m_PlanarReflection - (uint8_t*)&p.m_pBase);
		m_Layout.AddParameter("WaterNormalTile", (uint8_t*)&p.m_NormalTile - (uint8_t*)&p.m_pBase);

	}
	SWaterConst& Get() { return m_Const.Get(); }
	void PrepareConstantBuffer(Context context, SResourceDesc* param) { m_Const.PrepareBuffer(context); }
	SResourceDesc m_WaveTexture;
	SResourceDesc m_EnvTexture;
	SResourceDesc m_PlanarReflection;
	SResourceDesc m_NormalTile;


};

class CWaterTask : public CRenderTask
{
public:
	virtual bool CreateResources(Device device);
	virtual void Draw(Context context);
	void SetCameraLookAt(vec3 eye, vec3 target, vec3 up);
	void SetBox(vec3 center, vec3 min, vec3 max, vec3 minFar, vec3 maxFar);
	void SetSpeed(float val) { m_Speed = val; };
	void SetWaveOffset(float val);

	void SetTextures(Texture env, Texture wave, Texture planar,Texture normalTile);
	void Update(float delta);
private:
	void InitPipeline(Context context);
	Pipeline m_Pipeline;
	SamplerTable m_SamplerTable;
	BlendState m_BlendState;
	CWaterParameterProvider m_WaterProvider;
	float m_StreamOffset = 0;
	float m_Speed = 0;
};