#include "ParameterProviderRegistry.h"

CParameterProviderRegistry* CParameterProviderRegistry::m_pInstance = nullptr;

CParameterProviderLayout::CParameterProviderLayout(CreateMapCallback ptr) :m_ResourceMapCallback(ptr)
{
	if (!CParameterProviderRegistry::GetInstane())
	{
		CParameterProviderRegistry::CreateInstance();
	}
	CParameterProviderRegistry* reg = CParameterProviderRegistry::GetInstane();
	m_ResourceMapCallback();
	reg->Register(this);
}
void CParameterProviderLayout::AddParameter(const char* name, uint32_t offset)
{
	m_ParameterMap.insert(map<string,uint32_t>::value_type(name, offset));
}
bool CParameterProviderLayout::FindParameter(const char* name, uint32_t& offset)
{
	auto it = m_ParameterMap.find(name);
	if(it!= m_ParameterMap.end())
	{
		offset = it->second;
		return true;
	}
	return false;
};

void CParameterProviderRegistry::Register(CParameterProviderLayout* layout)
{
	layout->id = m_Layouts.size();
	m_Layouts.push_back(layout);
}
bool CParameterProviderRegistry::FindNextLayout(const char* name , bool new_search, uint32_t& layout_id, uint32_t& offset)
{
	for (int i = new_search?0: layout_id + 1; i < m_Layouts.size(); i++)
	{
		if (m_Layouts[i]->FindParameter(name, offset))
		{
			layout_id = i;
			return true;
		}
	}
	return false;
}
