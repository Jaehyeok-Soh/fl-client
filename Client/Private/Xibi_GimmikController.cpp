#include "pch.h"
#include "GimmikController.h"
#include "GameInstance.h"
#include "Xibi_GimmikController.h"

CXibi_GimmikController::CXibi_GimmikController()
	: Super()
{
}

CXibi_GimmikController::CXibi_GimmikController(const CXibi_GimmikController& rhs)
	: Super(rhs)
{
}

HRESULT CXibi_GimmikController::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;

	return E_NOTIMPL;
}

HRESULT CXibi_GimmikController::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CXibi_GimmikController::On_TeleportRandom()
{
}

void CXibi_GimmikController::On_TeleportCenter()
{
}

void CXibi_GimmikController::On_ModelAnimNotify(const AnimNotifyKey& key)
{
	if (key.eID != EAnimNotifyId::Trigger_Gimmik)
		return;

	EGimmikType eType = static_cast<EGimmikType>(key.iParam0);
	if (eType <= EGimmikType::COUNT)
		return;

	switch (eType)
	{
	case Client::CXibi_GimmikController::EGimmikType::TeleportRandom:
		On_TeleportRandom();
		break;
	case Client::CXibi_GimmikController::EGimmikType::TeleportCenter:
		On_TeleportCenter();
		break;
	}
}

CXibi_GimmikController* CXibi_GimmikController::Create()
{
	CXibi_GimmikController* pInstance = new CXibi_GimmikController();
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CXibi_GimmikController::Create, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CXibi_GimmikController::Clone(void* pArg)
{
	CXibi_GimmikController* pInstance = new CXibi_GimmikController(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CXibi_GimmikController::Clone, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CXibi_GimmikController::Free()
{
	Super::Free();
}
