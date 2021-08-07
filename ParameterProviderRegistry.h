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
#pragma once
#include <map>
#include <string>
#include <vector>
using namespace std;
class CParameterProviderRegistry;
class CParameterProviderLayout// per provider class
{
public:
	typedef  void (*CreateMapCallback)();
	CParameterProviderLayout(CreateMapCallback ptr);
	void AddParameter(const char* name, uint32_t offset);
	bool FindParameter(const char* name, uint32_t& offset);
	uint32_t GetId() { return id; }
private:
	CreateMapCallback m_ResourceMapCallback;
	uint32_t id;
	map<string, uint32_t> m_ParameterMap;

	friend class CParameterProviderRegistry;

};
class CParameterProviderRegistry
{
	public:
		static CParameterProviderRegistry* CreateInstance() { if (!m_pInstance) m_pInstance = new CParameterProviderRegistry(); return m_pInstance; }
		static CParameterProviderRegistry* GetInstane() { return m_pInstance; }
		void Register(CParameterProviderLayout* layout);
		bool FindNextLayout(const char* name, bool new_search, uint32_t& layout_id, uint32_t& resource_offset);
	private:
		static CParameterProviderRegistry* m_pInstance;
		vector<CParameterProviderLayout*> m_Layouts;
};
