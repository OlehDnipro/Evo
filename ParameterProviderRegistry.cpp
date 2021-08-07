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
