#include "Engine_pch.h"
#include "Resource_Manager.h"
#include "TextureBase.h"

CResource_Manager::CResource_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: m_pDevice(pDevice)
	, m_pDeviceContext(pDeviceContext)
{
	Safe_AddRef(m_pDeviceContext);
	Safe_AddRef(m_pDevice);
}

HRESULT CResource_Manager::Initialize()
{
	return S_OK;
}

void CResource_Manager::Clear()
{
	for (Key_Resource_Map& Element : m_Resources)
	{
		for (auto& Pair : Element)
		{
			Safe_Release(Pair.second);
		}
		Element.clear();
	}
}

CTextureBase* CResource_Manager::GetOrAddTexture(const wstring& wstrKey, void* pArg)
{
	// 내부에서 Ref올림
	CTextureBase* pTexture = Get<CTextureBase>(wstrKey);

	if (!pTexture)
	{
		pTexture = CTextureBase::Create(m_pDevice, m_pDeviceContext, pArg);

		Key_Resource_Map& ResourceMap = m_Resources[static_cast<_uint>(EResourceType::TEXTURE)];
		ResourceMap.insert(map<wstring, CResourceBase*>::value_type(wstrKey, pTexture));
		Safe_AddRef(pTexture);
	}

	return pTexture;
}

CResource_Manager* CResource_Manager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CResource_Manager* pInstance = new CResource_Manager(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("CResource_Manager::Create, Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CResource_Manager::Free()
{
	Clear();
	Safe_Release(m_pDeviceContext);
	Safe_Release(m_pDevice);
	Super::Free();
}