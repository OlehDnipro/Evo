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
#include "SH.h"
#if GRAPHICS_API_VULKAN
#include "Shaders\Tech\vulkan\SH.pipeline.h"
#else
#include "Shaders\Tech\d3d12\SH.pipeline.h"
#endif
CParameterProviderLayout CParameterProviderBase<CSHParameterProvider>::m_Layout = CParameterProviderLayout(&CSHParameterProvider::CreateParameterMap);

const char* GetResourceNameSH(uint slot, uint binding)
{
	return NSH::RootItemNames[slot][binding];
}

void CComputeSHTask::SetTextures(SResourceDesc env, SResourceDesc out)
{
	m_Provider.m_EnvTex = env;
	m_Provider.m_OutTex = out;
	m_Provider.m_TexSize.x = GetWidth((Texture)env.m_Resource);
	m_Provider.m_TexSize.y = GetHeight((Texture)env.m_Resource);
	m_Provider.m_SumBufferChannelSize = ((m_Provider.m_TexSize.x - 1)/ 32 + 1) * ((m_Provider.m_TexSize.y - 1) / 32 + 1) *6;
	m_Provider.m_SumBufferSize = m_Provider.m_SumBufferChannelSize * 3;
}
CComputeSHTask::~CComputeSHTask()
{
//	DestroyBuffer(m_Device, (Buffer)m_Provider.m_SumBuffer.m_Resource);
//	DestroyBuffer(m_Device, (Buffer)m_Provider.m_BaseBuffer.m_Resource);
}
SRootResourceAttrs GetResourceAttrsSH(uint slot, uint binding)
{
	SRootResourceAttrs attrs;
	attrs.name = NSH::RootItemNames[slot][binding];
	attrs.type = NSH::RootItemTypes[slot][binding];
	return attrs;
}
bool CComputeSHTask::CreateResources(Device device)
{
    m_Device = device;
    bool root =  m_Cache.CreateRootSignature(m_Device, NSH::RootSig, GetResourceAttrsSH);
	if (!root)
		return false;

	const SSamplerDesc samplers[] = { { FILTER_POINT, 1, AM_WRAP, AM_WRAP, AM_WRAP, ALWAYS }};
	m_SamplerTable = CreateSamplerTable(m_Device, m_Cache.GetRootSignature(), NSH::Samplers, samplers);

	SBufferParams params = { sizeof(ShPoly) * m_Provider.m_TexSize.x * m_Provider.m_TexSize.y * 6 ,
							HeapType::HEAP_DEFAULT,
							Usage::SHADER_RESOURCE,
							nullptr };
	params.m_Stride = sizeof(ShPoly);
	m_Provider.m_BaseBuffer = CreateBuffer(m_Device, params);
	
	params.m_Size = sizeof(ShPoly) * m_Provider.m_SumBufferSize;
	params.m_HeapType = HeapType::HEAP_DEFAULT;
	params.m_Usage = Usage::SHADER_RESOURCE;
	params.m_Name = nullptr;
	params.m_Stride = sizeof(ShPoly);
	m_Provider.m_SumBuffer = CreateBuffer(m_Device, params);

	params.m_Size = sizeof(ShPoly) * m_Provider.m_SumBufferSize;
	params.m_HeapType = HeapType::HEAP_READBACK;
	params.m_Usage = Usage::NONE;
	params.m_Name = nullptr;
	m_SumBufferReadback = CreateBuffer(m_Device, params);
}

void CComputeSHTask::Execute(Context context, Pass pass)
{
	if (pass == Pass::ComputeTex)
	{
		SMapBufferParams params(context, m_SumBufferReadback);

		ShPoly* perPixelArea = (ShPoly*)MapBuffer(params);
		ShPoly3Channel& result = m_Provider.m_ModelConst.Get();
		uint count;
		memset(&result, 0, sizeof(ShPoly3Channel));
		if (perPixelArea)
		{
			for (int ch = 0; ch < 3; ch++)
			{
				count = 0;
				for (int area = 0; area < m_Provider.m_SumBufferChannelSize; area++)
				{
					count += perPixelArea->count;
					for (int i = 0; i < 9; i++)
					{
						result.channels[ch].coef[i] += perPixelArea->coef[i] / 1000000.0f;
					}
					perPixelArea++;
				}
				for (int i = 0; i < 9; i++)
				{
					result.channels[ch].coef[i] *= (4 * PI / count);
				}
			}
			UnmapBuffer(params);
		}
	}

	InitPipeline(context);
	SetRootSignature(context, m_Cache.GetRootSignature());

	SetPipeline(context, pass == Pass::ComputeBase? m_PipelineBase: m_PipelineTex);

	SetComputeSamplerTable(context, NSH::Samplers, m_SamplerTable);

	Barrier(context, { { m_Provider.m_OutTex , EResourceState::RS_UNORDERED_ACCESS } });
	Barrier(context, { { m_Provider.m_SumBuffer , EResourceState::RS_UNORDERED_ACCESS } });
	int s = sizeof(int);
	m_Provider.PrepareConstantBuffer(context, m_Provider.m_ModelConst.GetPtr());
	m_Cache.GatherParameters({
								*m_Provider.m_ModelConst.GetPtr(),
								m_Provider.m_EnvTex,
								m_Provider.m_OutTex,
								m_Provider.m_BaseBuffer,
								m_Provider.m_SumBuffer,
		},
		NSH::Resources, 0);

	ResourceTable rt = CreateResourceTable(GetDevice(context), m_Cache.GetRootSignature(), NSH::Resources, nullptr, 
		m_Cache.GetDescriptorsCount(NSH::Resources), context);

	m_Cache.UpdateResourceTable(GetDevice(context), NSH::Resources, rt);

	SetComputeResourceTable(context, NSH::Resources, rt);

	DestroyResourceTable(GetDevice(context), rt);
	if (pass == Pass::ComputeBase)
	{
		Barrier(context, { { m_Provider.m_BaseBuffer , EResourceState::RS_UNORDERED_ACCESS } });

		Dispatch(context, (m_Provider.m_TexSize.x - 1) / 32 + 1, (m_Provider.m_TexSize.y - 1) / 32 + 1, 6);

		Barrier(context, { { m_Provider.m_SumBuffer , EResourceState::RS_TRANSFER_SRC } });

		Barrier(context, { { m_SumBufferReadback , EResourceState::RS_TRANSFER_DST } });

		CopyBuffer(context, m_SumBufferReadback, (Buffer)m_Provider.m_SumBuffer.m_Resource);
	}
	else
	{
		Barrier(context, { { m_Provider.m_BaseBuffer , EResourceState::RS_SHADER_READ } });

		Dispatch(context, (m_Provider.m_TexSize.x - 1) / 32 + 1, (m_Provider.m_TexSize.y - 1) / 32 + 1, 6);

	}
	
}

void CComputeSHTask::InitPipeline(Context context)
{
	if (!m_PipelineBase)
	{	
		SComputePipelineParams p_params;
		p_params.m_RootSignature = m_Cache.GetRootSignature();	
		p_params.m_CS = NSH::CSComputeBase;

		m_PipelineBase = CreatePipeline(m_Device, p_params);
	}
	if (!m_PipelineTex)
	{
		SComputePipelineParams p_params;
		p_params.m_RootSignature = m_Cache.GetRootSignature();
		p_params.m_CS = NSH::CSComputeTex;

		m_PipelineTex = CreatePipeline(m_Device, p_params);
	}
}

