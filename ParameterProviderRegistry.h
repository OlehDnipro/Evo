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
class IParameterProvider
{
public:
	virtual uint32_t GetLayoutId() = 0;
	uint8_t* GetBaseParameterPointer() { return &m_pBase; }
protected:
	uint8_t m_pBase;
};
template<class T>
class CParameterProviderBase : public IParameterProvider
{
protected:
	static CParameterProviderLayout m_Layout;
public:
	uint32_t GetLayoutId() { return m_Layout.GetId(); };
};

