#include "Engine_pch.h"
#include "Level.h"
#include "BuilderSystem.h"
#include "GameInstance.h"

CLevel::CLevel(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: m_pGameInstance(CGameInstance::GetInstance())
	, m_pDevice(pDevice)
	, m_pDeviceContext(pDeviceContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);
	Safe_AddRef(m_pGameInstance);
}

HRESULT CLevel::Initialize()
{
	m_pBuilderSystem = CBuilderSystem::Create();
	return S_OK;
}

HRESULT CLevel::Awake(const _uint iLevelID)
{
	if (FAILED(m_pGameInstance->Awake_GameObjects(iLevelID)))
		return E_FAIL;

	return S_OK;
}

void CLevel::Update(_float fTimeDelta)
{
}

void CLevel::Update_Picking()
{
}

HRESULT CLevel::Render()
{
	return S_OK;
}

HRESULT CLevel::Ready_Builder(DTO::ECategory eCategory, CBuilderBase* pBuilder)
{
	return m_pBuilderSystem->Ready_Builder(eCategory, pBuilder);
}

HRESULT CLevel::Build_File(_uint iLevelID, DTO::ECategory eCategory, string strFileKey)
{
	return m_pBuilderSystem->Build_File(iLevelID, eCategory, strFileKey);
}

void CLevel::Free()
{
	Safe_Release(m_pBuilderSystem);
	Safe_Release(m_pGameInstance);
	Safe_Release(m_pDeviceContext);
	Safe_Release(m_pDevice);
	Super::Free();
}
