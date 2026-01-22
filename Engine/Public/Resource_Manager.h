#pragma once
#include "Base.h"
#include "ModelAnimation.h"
#include "Material.h"
#include "MaterialInstance.h"
#include "TextureBase.h"

NS_BEGIN(Engine)

class CResource_Manager : public CBase
{
	using Super = CBase;
	using Key_Resource_Map = map<wstring, CResourceBase*>;
private:
	CResource_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CResource_Manager() = default;

	HRESULT Initialize();
	void Clear();
public:
	template<typename T>
	T* Load(const wstring& wstrKey, void* pArg);
	template<typename T>
	HRESULT Add(const wstring& wstrKey, T* pResource);
	template<typename T>
	T* Get(const wstring& wstrKey, void* pArg = nullptr);
	template<>
	CModelAnimation* Get(const wstring &wstrKey, void* pArg);
	template<typename T>
	void Remove(const wstring& wstrKey);
	class CTextureBase* GetOrAddTexture(const wstring& wstrKey, void* pArg);
private:
	template<typename T>
	EResourceType Get_Type();
private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pDeviceContext = { nullptr };
	array<Key_Resource_Map, g_ResourceTypeCount> m_Resources;
public:
	static CResource_Manager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual void Free() override;
};

template<typename T>
inline T* CResource_Manager::Load(const wstring& wstrKey, void* pArg)
{
	EResourceType eResourceType = Get_Type<T>();
	if (eResourceType == EResourceType::END)
		return nullptr;

	Key_Resource_Map& ResourceMap = m_Resources[static_cast<_uint>(eResourceType)];

	auto itr = ResourceMap.find(wstrKey);
	if (itr != ResourceMap.end())
	{
		Safe_AddRef(itr->second);
		return static_cast<T*>(itr->second);
	}

	T* pNewResource = T::Create(m_pDevice, m_pDeviceContext, pArg);
	ResourceMap.insert(map<wstring, CResourceBase*>::value_type(wstrKey, pNewResource));
	Safe_AddRef(pNewResource);
	return pNewResource;
}

template<typename T>
inline HRESULT CResource_Manager::Add(const wstring& wstrKey, T* pResource)
{
	if (!pResource)
		return E_FAIL;

	EResourceType eResourceType = Get_Type<T>();
	if (eResourceType == EResourceType::END)
		return E_FAIL;

	Key_Resource_Map& ResourceMap = m_Resources[static_cast<_uint>(eResourceType)];

	auto itr = ResourceMap.find(wstrKey);
	if (itr != ResourceMap.end())
	{
		Safe_Release(pResource);
		return S_OK;
	}

	pResource->Set_Name(wstrKey);
	ResourceMap.insert(map<wstring, CResourceBase*>::value_type(wstrKey, pResource));

	return S_OK;
}

template<typename T>
inline T* CResource_Manager::Get(const wstring& wstrKey, void* pArg)
{
	EResourceType eResourceType = Get_Type<T>();
	if (eResourceType == EResourceType::END)
		return nullptr;

	Key_Resource_Map& ResourceMap = m_Resources[static_cast<_uint>(eResourceType)];

	auto itr = ResourceMap.find(wstrKey);
	if (itr == ResourceMap.end())
		return nullptr;

	Safe_AddRef(itr->second);
	return static_cast<T*>(itr->second);
}

template<>
inline CModelAnimation* CResource_Manager::Get<CModelAnimation>(const wstring& wstrKey, void* pArg)
{
	if (wstrKey.empty())
		return nullptr;
	Key_Resource_Map& ResourceMap = m_Resources[static_cast<_uint>(EResourceType::MODEL_ANIMATION)];
	auto itr = ResourceMap.find(wstrKey);
	if (itr == ResourceMap.end())
		return nullptr;

	return static_cast<CModelAnimation*>(itr->second)->Clone();
}

template<typename T>
inline void CResource_Manager::Remove(const wstring& wstrKey)
{
	EResourceType eResourceType = Get_Type<T>();
	if (eResourceType == EResourceType::END)
		return;

	Key_Resource_Map& ResourceMap = m_Resources[static_cast<_uint>(eResourceType)];

	auto itr = ResourceMap.find(wstrKey);
	if (itr == ResourceMap.end())
		return;

	CResourceBase* pResource = itr->second;
	ResourceMap.erase(itr);
	Safe_Release(pResource);
}

template<typename T>
inline EResourceType CResource_Manager::Get_Type()
{
	if constexpr (std::is_same_v<T, class Engine::CTextureBase>) return EResourceType::TEXTURE;
	if constexpr (std::is_same_v<T, class Engine::CMaterial>) return EResourceType::MATERIAL;
	//if constexpr (std::is_same_v<T, class Engine::CAnimation>) return EResourceType::ANIMATION;
	if constexpr (std::is_same_v<T, class Engine::CModelAnimation>) return EResourceType::MODEL_ANIMATION;
	if constexpr (std::is_same_v<T, class Engine::CMaterialInstance>) return EResourceType::MATERIAL_INSTANCE;

	return EResourceType::END;
}

NS_END