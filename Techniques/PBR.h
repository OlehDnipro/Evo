#include "RenderTask.h"
struct SphereVertex
{
	float3 pos;
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
class CPBRModelParameterProvider : public CParameterProviderBase<CPBRModelParameterProvider>
{
public:
	CConstantParameter<SPBRModel> m_ModelConst;
	static void CreateParameterMap()
	{
		CPBRModelParameterProvider p;
		m_Layout.AddParameter(SPBRModel::GetName(), (uint8_t*)p.m_ModelConst.GetPtr() - (uint8_t*)&p.m_pBase);
	}
	SPBRModel& Get() { return m_ModelConst.Get(); }
	void PrepareConstantBuffer(Context context, SResourceDesc* param) { m_ModelConst.PrepareBuffer(context); }
};
class CSphereGeometry : public IGeometryCollection
{
public:
	~CSphereGeometry();
	void Draw(Context context, CShaderCache& cache, int resources_slot);
	void DefineVertexFormat(vector<AttribDesc>& format);
	PrimitiveType GetPrimType() { return PRIM_TRIANGLES; };
	void Create(Device device);
private:
	Buffer m_VB, m_IB;
	VertexSetup m_VertexSetup;
	Device m_Device;
	uint m_meshIndicesCount;
	CPBRModelParameterProvider m_Provider;

};

class CPBRTask : public CRenderTask
{
public:
	virtual bool CreateResources(Device device);
	virtual void Draw(Context context);
	virtual void SetCameraLookAt(vec3 eye, vec3 target, vec3 up);

private:
	Pipeline m_Pipeline;
	void InitPipeline(Context context);
	CViewportParameterProvider m_ViewportProvider;


};
