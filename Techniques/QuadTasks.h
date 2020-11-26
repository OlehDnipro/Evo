#include "RenderTask.h"

class CTexturedQuadGeometry : public IGeometryCollection
{
public:
	~CTexturedQuadGeometry();
	void Draw(Context context, CShaderCache& cache, int resources_slot);
	void DefineVertexFormat(vector<AttribDesc>& format);
	PrimitiveType GetPrimType() { return PRIM_TRIANGLE_FAN; };
	void SetTexture(Texture tex) { m_Texture = tex; };
	void Create(Device device);

private:
	Texture m_Texture;
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
	void SetPassParameters(RenderPass pass);
private:
	Pipeline m_Pipeline;
};

class CWaterDropTask : public CRenderTask
{
public:
	virtual bool CreateResources(Device device);
	virtual void Draw(Context context);
	void SetPassParameters(RenderPass pass);
private:
	Pipeline m_Pipeline;
	SamplerTable m_SamplerTable;
};