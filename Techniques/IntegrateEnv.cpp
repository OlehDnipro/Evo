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
#include "IntegrateEnv.h"
#if GRAPHICS_API_VULKAN
#include "Shaders\Tech\vulkan\Conv.pipeline.h"
#else
#include "Shaders\Tech\d3d12\Conv.pipeline.h"
#endif
CParameterProviderLayout CParameterProviderBase<CConvParameterProvider>::m_Layout = CParameterProviderLayout(&CConvParameterProvider::CreateParameterMap);

const char* GetResourceNameConv(uint slot, uint binding)
{
	return NConv::RootItemNames[slot][binding];
}

void CIntegrateEnvTask::SetTextures(SResourceDesc env, SResourceDesc out, SResourceDesc brdf)
{
	m_Provider.m_EnvTex = env;
	m_Provider.m_OutTex = out;
	m_Provider.m_Brdf = brdf;
	m_Provider.m_TexSize.x = GetWidth((Texture)env.m_Resource);
	m_Provider.m_TexSize.y = GetHeight((Texture)env.m_Resource);

}
CIntegrateEnvTask::~CIntegrateEnvTask()
{
//	DestroyBuffer(m_Device, (Buffer)m_Provider.m_SumBuffer.m_Resource);
//	DestroyBuffer(m_Device, (Buffer)m_Provider.m_BaseBuffer.m_Resource);
}
SRootResourceAttrs GetResourceAttrsConv(uint slot, uint binding)
{
	SRootResourceAttrs attrs;
	attrs.name = NConv::RootItemNames[slot][binding];
	attrs.type = NConv::RootItemTypes[slot][binding];
	return attrs;
}
bool CIntegrateEnvTask::CreateResources(Device device)
{
    m_Device = device;
    bool root =  m_Cache.CreateRootSignature(m_Device, NConv::RootSig, GetResourceAttrsConv);
	if (!root)
		return false;

	const SSamplerDesc samplers[] = { { FILTER_POINT, 1, AM_WRAP, AM_WRAP, AM_WRAP, ALWAYS }};
	m_SamplerTable = CreateSamplerTable(m_Device, m_Cache.GetRootSignature(), NConv::Samplers, samplers);

}
#define MIPS 6
void CIntegrateEnvTask::Execute(Context context, Pass pass, int mip)
{
	uint sizeX = m_Provider.m_TexSize.x >> mip;
	uint sizeY = m_Provider.m_TexSize.y >> mip;
	{

		ConvConst& params = m_Provider.m_ModelConst.Get();
		params.size = m_Provider.m_TexSize.x;//equal
		params.mip = mip >=0 ? mip : 0;
		params.roughness = mip * 0.15;

		InitPipeline(context);
		SetRootSignature(context, m_Cache.GetRootSignature());

		SetPipeline(context, pass == Pass::ConvEnv ? m_PipelineEnv : m_PipelineBRDF);

		SetComputeSamplerTable(context, NConv::Samplers, m_SamplerTable);

		int s = sizeof(int);
		m_Provider.PrepareConstantBuffer(context, m_Provider.m_ModelConst.GetPtr());
		m_Cache.GatherParameters({
									*m_Provider.m_ModelConst.GetPtr(),
									m_Provider.m_EnvTex,
									{ (Texture)m_Provider.m_OutTex.m_Resource, {0, pass == Pass::ConvEnv?mip:-1, -1} },
									m_Provider.m_Brdf,

			},
			NConv::Resources, 0);

		ResourceTable rt = CreateResourceTable(GetDevice(context), m_Cache.GetRootSignature(), NConv::Resources, nullptr,
			m_Cache.GetDescriptorsCount(NConv::Resources), context);

		m_Cache.UpdateResourceTable(GetDevice(context), NConv::Resources, rt);

		SetComputeResourceTable(context, NConv::Resources, rt);

		DestroyResourceTable(GetDevice(context), rt);
		if (pass == Pass::ConvEnv)
		{
			Barrier(context, { { { (Texture)m_Provider.m_OutTex.m_Resource, {0, mip, -1} } , EResourceState::RS_UNORDERED_ACCESS } });

			Dispatch(context, ( sizeX - 1) / 32 + 1, (sizeY - 1) / 32 + 1, 6);
			Barrier(context, { { { (Texture)m_Provider.m_OutTex.m_Resource, {0, mip, -1} } , EResourceState::RS_SHADER_READ } });

		}
		else
		{
			Barrier(context, { { m_Provider.m_Brdf , EResourceState::RS_UNORDERED_ACCESS } });

			Dispatch(context, (m_Provider.m_TexSize.x - 1) / 32 + 1, (m_Provider.m_TexSize.y - 1) / 32 + 1, 1);

			Barrier(context, { { m_Provider.m_Brdf , EResourceState::RS_SHADER_READ } });


		}
	}
}

void CIntegrateEnvTask::InitPipeline(Context context)
{
	if (!m_PipelineEnv)
	{	
		SComputePipelineParams p_params;
		p_params.m_RootSignature = m_Cache.GetRootSignature();	
		p_params.m_CS = NConv::CSConvEnv;

		m_PipelineEnv = CreatePipeline(m_Device, p_params);
	}
	if (!m_PipelineBRDF)
	{
		SComputePipelineParams p_params;
		p_params.m_RootSignature = m_Cache.GetRootSignature();
		p_params.m_CS = NConv::CSConvBRDF;

		m_PipelineBRDF = CreatePipeline(m_Device, p_params);
	}
}

