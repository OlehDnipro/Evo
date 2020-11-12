
/* * * * * * * * * * * * * Author's note * * * * * * * * * * * *\
*   _       _   _       _   _       _   _       _     _ _ _ _   *
*  |_|     |_| |_|     |_| |_|_   _|_| |_|     |_|  _|_|_|_|_|  *
*  |_|_ _ _|_| |_|     |_| |_|_|_|_|_| |_|     |_| |_|_ _ _     *
*  |_|_|_|_|_| |_|     |_| |_| |_| |_| |_|     |_|   |_|_|_|_   *
*  |_|     |_| |_|_ _ _|_| |_|     |_| |_|_ _ _|_|  _ _ _ _|_|  *
*  |_|     |_|   |_|_|_|   |_|     |_|   |_|_|_|   |_|_|_|_|    *
*                                                               *
*                     http://www.humus.name                     *
*                                                                *
* This file is a part of the work done by Humus. You are free to   *
* use the code in any way you like, modified, unmodified or copied   *
* into your own work. However, I expect you to respect these points:  *
*  - If you use this file and its contents unmodified, or use a major *
*    part of this file, please credit the author and leave this note. *
*  - For use in anything commercial, please request my approval.     *
*  - Share your work and ideas too as much as you can.             *
*                                                                *
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "RenderTask.h"

CParameterProviderLayout CParameterProviderBase<CModelParameterProvider>::m_Layout = CParameterProviderLayout(&CModelParameterProvider::CreateParameterMap);

bool SimpleObject::Init(Device device, VertexLayout layout,const char* model, const char* texture, float scale)
{
	m_Model.loadFromFile(model, layout, scale, device);
	m_Texture = CreateTexture(device, texture);
	return true;
}
SimpleObjectInstance::SimpleObjectInstance(const SimpleObject& object, Device device,  float4x4 mtx) :m_Object(object)
{
	SBufferParams cb_params = { sizeof(SPerModel), HeapType::HEAP_DEFAULT, Usage::CONSTANT_BUFFER, "" };
	m_Provider.Get().model = mtx;
	m_Provider.m_ModelTexture = m_Object.m_Texture ;
}
void SimpleObjectInstance::Draw(Context context)
{
	SetVertexSetup(context, m_Object.m_Model.GetVertexSetup());
	DrawIndexed(context, 0, m_Object.m_Model.indexCount);
}

void CShaderCache::FindRootResource(ItemType type, uint slot, uint binding, uint first_item_of_table_with_size, void* receiver)
{
	CShaderCache* shadow = (CShaderCache*)receiver;
	if (first_item_of_table_with_size)
	{
		TableUpdate update;
		update.slot = slot;
		update.m_Descriptors.resize(first_item_of_table_with_size);
		shadow->m_TableUpdates.push_back(update);
	}
	const char* name = shadow->m_getResourceName(slot,binding);
	uint32_t layout, offset;
	bool new_search = true;
	while(CParameterProviderRegistry::GetInstane()->FindNextLayout(name, new_search, layout, offset))
	{
		new_search = false;
		shadow->m_ProviderUsage[layout].push_back({ type, offset, slot, binding });
	}
}


void CShaderCache::GatherParameters(IParameterProvider** providers,Context context, uint count)
{
	for (int i = 0; i < count; i++)
	{
		uint32_t id = providers[i]->GetLayoutId();
		auto usage = m_ProviderUsage.find(id);
		if (usage != m_ProviderUsage.end())
		{
			for (auto& param : usage->second)
			{
				SResourceDesc& write = m_TableUpdates[param.m_slot].m_Descriptors[param.m_binding];
				switch (param.m_type)
				{
				case TEXTURE:
				case RWTEXTURE:
					write = *(Texture*)(providers[i]->GetBaseParameterPointer() + param.m_provMemOffset);

					break;

				case STRUCTUREDBUFFER:
				case RWSTRUCTUREDBUFFER:
					write = *(Buffer*)(providers[i]->GetBaseParameterPointer() + param.m_provMemOffset);
				case CBV:
					SConstantBuffer* constants = (SConstantBuffer*)(providers[i]->GetBaseParameterPointer() + param.m_provMemOffset);
					providers[i]->PrepareConstantBuffer(context, (uint8_t*)constants);
					write = constants->buffer;
					write.m_Desc.bView.offset = constants->offset;
					write.m_Desc.bView.size = constants->size;

				}
			}
		}
	}
}

void IObjectCollection::Draw(Context context, CShaderCache& cache,  int resources_slot)
{
	for (int i = 0; i < m_ObjectInstances.size(); i++)
	{
		IParameterProvider* prov = m_ObjectInstances[i]->GetModelProvider();
		cache.GatherParameters(&prov, context, 1);
		ResourceTable rt = CreateResourceTable(GetDevice(context), cache.m_RootSig, resources_slot, nullptr, 0, context);

		UpdateResourceTable(GetDevice(context), cache.m_RootSig, resources_slot, rt,
			cache.m_TableUpdates[resources_slot].m_Descriptors.data(), 0, 5);

		SetGraphicsResourceTable(context, resources_slot, rt);

		DestroyResourceTable(GetDevice(context), rt);
		m_ObjectInstances[i]->Draw(context);
	}
}

IObjectCollection::~IObjectCollection()
{
	for (int i = 0; i < m_ObjectInstances.size(); i++)
	{
		delete m_ObjectInstances[i];
	}
}

void IObjectCollection::DefineVertexFormat(vector<AttribDesc>& format)
{
	for (int i = 0; i < m_vertexLayout.components.size(); i++)
	{
		switch (m_vertexLayout.components[i])
		{
		case VERTEX_COMPONENT_POSITION:
			format.push_back({ 0, VF_FLOAT3, "Position" });
			break;
		case VERTEX_COMPONENT_NORMAL:
			format.push_back({ 0, VF_FLOAT3, "Normal" });
			break;
		case VERTEX_COMPONENT_COLOR:
			format.push_back({ 0, VF_FLOAT3, "Color" });
			break;
		case VERTEX_COMPONENT_UV:
			format.push_back({ 0, VF_FLOAT2, "TexCoord" });
			break;
		}
	};
}