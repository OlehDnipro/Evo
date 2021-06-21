#include "RenderTask.h"
struct ShBaseCoef
{
	float coef[9];
	int face;
	int2 pad;
};
struct ShPoly
{
	int coef[9];// multiplied by 1000 000
	int count;
	int2 pad;
};
struct ShPoly3Channel
{
	ShBaseCoef channels[3];
	static const char* GetName() { return "ShPoly3Channel"; }

};

class CSHParameterProvider : public CParameterProviderBase<CSHParameterProvider>
{
public:
	CConstantParameter<ShPoly3Channel> m_ModelConst;
	static void CreateParameterMap() {};//not used
	ShPoly3Channel& Get() { return m_ModelConst.Get(); }
	void PrepareConstantBuffer(Context context, SResourceDesc* param) { m_ModelConst.PrepareBuffer(context); }
	SResourceDesc m_EnvTex, m_OutTex;
	SResourceDesc m_BaseBuffer, m_SumBuffer;
	int2 m_TexSize;
	int m_SumBufferSize, m_SumBufferChannelSize;
};

class CÑomputeSHTask 
{
public:
	 bool CreateResources(Device device);
	 void Execute(Context context);
	 void SetTextures(SResourceDesc env, SResourceDesc out);
	 ~CÑomputeSHTask();
private:
	Pipeline m_PipelineBase, m_PipelineTex;
	void InitPipeline(Context context);
	CSHParameterProvider m_Provider;
	Buffer m_SumBufferReadback;
	CShaderCache m_Cache;
	Device m_Device;
	SamplerTable m_SamplerTable;
	
};
