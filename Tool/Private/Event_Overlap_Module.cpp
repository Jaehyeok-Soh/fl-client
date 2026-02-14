#include "pch.h"
#include "Event_Overlap_Module.h"
#include "GameInstance.h"
#include "Model.h"
#include "AnimObj.h"

CEvent_Overlap_Module::CEvent_Overlap_Module(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: Super(),
	m_pDevice(pDevice),
	m_pDeviceContext(pDeviceContext),
	m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);
	Safe_AddRef(m_pGameInstance);
}

HRESULT CEvent_Overlap_Module::Initialize()
{
	return S_OK;
}

void CEvent_Overlap_Module::Update(const _float fTimeDelta)
{
	if (!m_pOverlap)
		return;

	m_pOverlap->Update(fTimeDelta);
}

void CEvent_Overlap_Module::Render()
{
	if (!m_pOverlap)
		return;

	m_pOverlap->Render();
}

void CEvent_Overlap_Module::SetAttackOverlap(CPhysicsAttackOverlap* pAttackOverlap, CAnimObj* pOwner)
{
	Safe_Release(m_pOverlap);

	m_pOverlap = static_cast<CPhysicsAttackOverlap*>(pAttackOverlap);
	m_pOverlap->Set_Owner(pOwner);
	m_pOverlap->Awake();
}

void CEvent_Overlap_Module::SetOwner(CAnimObj* pOwner)
{
	if (m_pOverlap)
	{
		m_pOverlap->Set_Owner(pOwner);
		m_pOverlap->Awake();
	}
}

CEvent_Overlap_Module* CEvent_Overlap_Module::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CEvent_Overlap_Module* pInstance = new CEvent_Overlap_Module(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CEvent_Overlap_Module");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEvent_Overlap_Module::Free()
{
	Super::Free();

	Safe_Release(m_pOverlap);

	Safe_Release(m_pDevice);
	Safe_Release(m_pDeviceContext);

	Safe_Release(m_pGameInstance);
}
