#include "GameInstance.h"
#include "ResourceBase.h"

CResourceBase::CResourceBase(EResourceType eType, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: m_bClone(false)
	, m_eType(eType)
	, m_pDevice(pDevice)
	, m_pDeviceContext(pDeviceContext)
	, m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);
	Safe_AddRef(m_pGameInstance);
}

CResourceBase::CResourceBase(const CResourceBase& rhs)
	: m_bClone(true)
	, m_eType(rhs.m_eType)
	, m_pDevice(rhs.m_pDevice)
	, m_pDeviceContext(rhs.m_pDeviceContext)
	, m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);
	Safe_AddRef(m_pGameInstance);
	::lstrcpy(m_wszPath, rhs.m_wszPath);
	::lstrcpy(m_wszName, rhs.m_wszName);
}

HRESULT CResourceBase::Initialize(void* pArg)
{
	if (!pArg)
		return E_FAIL;

	RESOURCE_BASE_DESC* pDesc = static_cast<RESOURCE_BASE_DESC*>(pArg);
	::lstrcpy(m_wszPath, pDesc->wstrPath.c_str());
	::lstrcpy(m_wszName, pDesc->wstrName.c_str());

	return S_OK;
}

void CResourceBase::Free()
{
	Safe_Release(m_pGameInstance);
	Safe_Release(m_pDeviceContext);
	Safe_Release(m_pDevice);
	Super::Free();
}
