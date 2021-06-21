#include "SH.h"
#include "Shaders\Tech\SH.pipeline.h"
CParameterProviderLayout CParameterProviderBase<CSHParameterProvider>::m_Layout = CParameterProviderLayout(&CSHParameterProvider::CreateParameterMap);

const char* GetResourceNameSH(uint slot, uint binding)
{
	return NSH::RootItemNames[slot][binding];
}

void CÑomputeSHTask::SetTextures(SResourceDesc env, SResourceDesc out)
{
	m_Provider.m_EnvTex = env;
	m_Provider.m_OutTex = out;
	m_Provider.m_TexSize.x = GetWidth((Texture)env.m_Resource);
	m_Provider.m_TexSize.y = GetHeight((Texture)env.m_Resource);
	m_Provider.m_SumBufferChannelSize = ((m_Provider.m_TexSize.x - 1)/ 32 + 1) * ((m_Provider.m_TexSize.y - 1) / 32 + 1) *6;
	m_Provider.m_SumBufferSize = m_Provider.m_SumBufferChannelSize * 3;
}
CÑomputeSHTask::~CÑomputeSHTask()
{
//	DestroyBuffer(m_Device, (Buffer)m_Provider.m_SumBuffer.m_Resource);
//	DestroyBuffer(m_Device, (Buffer)m_Provider.m_BaseBuffer.m_Resource);
}
bool CÑomputeSHTask::CreateResources(Device device)
{
    m_Device = device;
    bool root =  m_Cache.CreateRootSignature(m_Device, NSH::RootSig, nullptr);
	if (!root)
		return false;

	const SSamplerDesc samplers[] = { { FILTER_POINT, 1, AM_WRAP, AM_WRAP, AM_WRAP, ALWAYS }};
	m_SamplerTable = CreateSamplerTable(m_Device, m_Cache.GetRootSignature(), NSH::Samplers, samplers);

	SBufferParams params = { sizeof(ShBaseCoef) * m_Provider.m_TexSize.x * m_Provider.m_TexSize.y * 6 ,
							HeapType::HEAP_DEFAULT,
							Usage::SHADER_RESOURCE,
							nullptr };

	m_Provider.m_BaseBuffer = CreateBuffer(m_Device, params);
	
	params.m_Size = sizeof(ShPoly) * m_Provider.m_SumBufferSize;
	params.m_HeapType = HeapType::HEAP_DEFAULT;
	params.m_Usage = Usage::SHADER_RESOURCE;
	params.m_Name = nullptr;
	m_Provider.m_SumBuffer = CreateBuffer(m_Device, params);

	params.m_Size = sizeof(ShPoly) * m_Provider.m_SumBufferSize;
	params.m_HeapType = HeapType::HEAP_READBACK;
	params.m_Usage = Usage::NONE;
	params.m_Name = nullptr;
	m_SumBufferReadback = CreateBuffer(m_Device, params);
}

void CÑomputeSHTask::Execute(Context context)
{
	InitPipeline(context);
	SetRootSignature(context, m_Cache.GetRootSignature());
	SetPipeline(context, m_PipelineBase);
	SetComputeSamplerTable(context, NSH::Samplers, m_SamplerTable);

	m_Provider.PrepareConstantBuffer(context, m_Provider.m_ModelConst.GetPtr());
	m_Cache.GatherParameters({ 
								*m_Provider.m_ModelConst.GetPtr(),
								m_Provider.m_EnvTex,
								m_Provider.m_OutTex,
								m_Provider.m_BaseBuffer,
								m_Provider.m_SumBuffer,
							 },		
							NSH::Resources, 0);

	ResourceTable rt = CreateResourceTable(GetDevice(context), m_Cache.GetRootSignature(), NSH::Resources, nullptr, 0, context);

	m_Cache.UpdateResourceTable(GetDevice(context), NSH::Resources, rt);

	SetComputeResourceTable(context, NSH::Resources, rt);

	DestroyResourceTable(GetDevice(context), rt);

	Dispatch(context,( m_Provider.m_TexSize.x - 1) / 32 + 1, (m_Provider.m_TexSize.y - 1) / 32 + 1, 1);
	
}

void CÑomputeSHTask::InitPipeline(Context context)
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

		m_PipelineBase = CreatePipeline(m_Device, p_params);
	}
}

