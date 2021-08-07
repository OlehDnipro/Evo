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
#include "RenderTask.h"
struct SphereVertex
{
	float3 pos;
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
