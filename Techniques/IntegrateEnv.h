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
struct ConvConst
{
	float roughness;
	int size;
	int mip;

};


class CConvParameterProvider : public CParameterProviderBase<CConvParameterProvider>
{
public:
	CConstantParameter<ConvConst> m_ModelConst;
	static void CreateParameterMap() {};//not used
	ConvConst& Get() { return m_ModelConst.Get(); }
	void PrepareConstantBuffer(Context context, SResourceDesc* param) { m_ModelConst.PrepareBuffer(context); }
	SResourceDesc m_EnvTex, m_OutTex, m_Brdf;
	int2 m_TexSize;
};

class CIntegrateEnvTask 
{
public:
	enum Pass
	{
		ConvEnv,
		ConvBRDF
	};
	 bool CreateResources(Device device);
	 void Execute(Context context, Pass pass, int mip = -1);
	 void SetTextures(SResourceDesc env, SResourceDesc out, SResourceDesc brdf);
	
	 ~CIntegrateEnvTask();
private:
	Pipeline m_PipelineEnv, m_PipelineBRDF;
	void InitPipeline(Context context);
	CConvParameterProvider m_Provider;
	CShaderCache m_Cache;
	Device m_Device;
	SamplerTable m_SamplerTable;
	
};
