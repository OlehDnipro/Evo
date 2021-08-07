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

CParameterProviderLayout CParameterProviderBase<CModelParameterProvider>::m_Layout = CParameterProviderLayout(&CModelParameterProvider::CreateParameterMap);
CParameterProviderLayout CParameterProviderBase<CViewportParameterProvider>::m_Layout = CParameterProviderLayout(&CViewportParameterProvider::CreateParameterMap);

bool SimpleObject::Init(Device device, VertexLayout layout,const char* model, const char* texture, float scale, bool depthApha)
{
	m_Model.loadFromFile(model, layout, scale, device);
	m_Texture = CreateTexture(device, texture);
	m_ShadowsAlphaCheck = depthApha;
	return true;
}
SimpleObjectInstance::SimpleObjectInstance(const SimpleObject& object, Device device,  float4x4 mtx) :m_Object(object)
{
	SBufferParams cb_params = { sizeof(SPerModel), HeapType::HEAP_DEFAULT, Usage::CONSTANT_BUFFER, "" };
	m_Provider.Get().model = mtx;
	m_Provider.Get().alphaCheck = object.ShadowsAlphaCheck();
	m_Provider.m_ModelTexture = m_Object.m_Texture ;
}
void SimpleObjectInstance::Draw(Context context)
{
	SetVertexSetup(context, m_Object.m_Model.GetVertexSetup());
	DrawIndexed(context, 0, m_Object.m_Model.indexCount);
}

void CShaderCache::FindRootResource(ItemType type, uint slot, uint binding, uint first_item_of_table_with_size, void* receiver)
{
	CShaderCache* cache = (CShaderCache*)receiver;
	if (first_item_of_table_with_size)
	{
		TableUpdate update;
		update.slot = slot;
		update.m_Descriptors.resize(first_item_of_table_with_size);
		update.m_ReflectionFlags.resize(first_item_of_table_with_size);
		for (uint8& flag : update.m_ReflectionFlags)
			flag = Refl_Flag_None;
		cache->m_TableUpdates.push_back(update);
	}
	if (cache->m_getResourceAttrs)
	{
		SRootResourceAttrs attrs = cache->m_getResourceAttrs(slot, binding);
		TableUpdate& update = cache->m_TableUpdates.back();
		if (strstr(attrs.type, "Cube"))
			update.m_ReflectionFlags[binding] |= Refl_Flag_Cube;
		if (strstr(attrs.type, "Array"))
			update.m_ReflectionFlags[binding] |= Refl_Flag_Array;


		uint32_t layout, offset;
		bool new_search = true;
		while (CParameterProviderRegistry::GetInstane()->FindNextLayout(attrs.name, new_search, layout, offset))
		{
			new_search = false;
			cache->m_ProviderUsage[layout].push_back({ type, offset, slot, binding });
		}
	}
}

void CShaderCache::GatherParameters(const vector<SResourceDesc>& resources, uint slot, uint offset)
{
	assert(m_TableUpdates[slot].m_Descriptors.size() <= resources.size());
	for (int i = 0; i < resources.size(); i++)
	{
		m_TableUpdates[slot].m_Descriptors[i + offset] = resources[i];
	}
}

void CShaderCache::GatherParameters(Context context, IParameterProvider** providers, uint count)
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
					write = *(SResourceDesc*)(providers[i]->GetBaseParameterPointer() + param.m_provMemOffset);

					break;

				case STRUCTUREDBUFFER:
				case RWSTRUCTUREDBUFFER:
					write = *(SResourceDesc*)(providers[i]->GetBaseParameterPointer() + param.m_provMemOffset);
				case CBV:
					SResourceDesc* constants = (SResourceDesc*)(providers[i]->GetBaseParameterPointer() + param.m_provMemOffset);
					providers[i]->PrepareConstantBuffer(context, constants);
					write = *constants;

				}
			}
		}
	}
}

void IObjectCollection::Draw(Context context, CShaderCache& cache,  int resources_slot)
{
	for (int i = 0; i < m_ObjectInstances.size(); i++)
	{
		if (m_ObjectInstances[i]->GetBase().GetMark() == m_FilteredMark)
			continue;
		IParameterProvider* prov = m_ObjectInstances[i]->GetModelProvider();
		cache.GatherParameters(context, &prov, 1);
		ResourceTable rt = CreateResourceTable(GetDevice(context), cache.GetRootSignature(), resources_slot, nullptr, 
			cache.GetDescriptorsCount(resources_slot), context);

		cache.UpdateResourceTable(GetDevice(context), resources_slot, rt);

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

bool CShaderCache::CreateRootSignature(Device device, const SCodeBlob& code, TGetResourceAttr getAttr)
{
	if ((m_RootSig = ::CreateRootSignature(device, code)) == nullptr)
		return false;
	m_getResourceAttrs = getAttr;
	IterateRootSignature(m_RootSig, &CShaderCache::FindRootResource, this);
	return true;
}
uint32 CShaderCache::GetDescriptorsCount(uint32 slot)
{
	return  m_TableUpdates[slot].m_Descriptors.size();
}
void CShaderCache::UpdateResourceTable(Device device, uint32 slot, ResourceTable table)
{
	::UpdateResourceTable(device, m_RootSig, slot, table,
		m_TableUpdates[slot].m_Descriptors.data(), 0, m_TableUpdates[slot].m_Descriptors.size(), m_TableUpdates[slot].m_ReflectionFlags.data());
}
void CShaderCache::DestroyRootSignature(Device device)
{
	::DestroyRootSignature(device, m_RootSig);
}
