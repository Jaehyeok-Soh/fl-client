#include "pch.h"
#include "Xibi_GimmikController.h"
#include "Model.h"
#include "GameObject.h"
#include "ModelAnimation.h"
#include "GameInstance.h"

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

	return S_OK;
}

HRESULT CXibi_GimmikController::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CXibi_GimmikController::Awake(const _uint iCurLevelIndex)
{
	Bind_ModelAnimNotify();

	if (FAILED(Set_Event()))
		return E_FAIL;

	return S_OK;
}

HRESULT CXibi_GimmikController::Set_Event()
{
	if (m_pOwnerModel == nullptr)
		return E_FAIL;

	// Teleport Random
	{
		_int iXibiSkill03_AnimIndex = Get_Owner()->Get_AnimationIndex(L"Animation_Xibi_Skill03");
		if (iXibiSkill03_AnimIndex == -1)
			return E_FAIL;

		CModelAnimation* pAnim = m_pOwnerModel->Get_Animation(iXibiSkill03_AnimIndex);
		if (pAnim == nullptr)
			return E_FAIL;

		// Teleport Random Disppear
		{
			AnimNotifyKey key{};
			key.eID = EAnimNotifyId::Trigger_Gimmik;
			key.fTrackPosition = 14.f;
			key.iParam0 = ENUM_TO_UINT(EGimmikType::TeleportRandom_Disappear);
			key.fParam0 = 5.f;
			pAnim->Pushback_Notifies(EAnimNotifyPhase::Late, key);
		}

		// Teleport Random Appear
		{
			AnimNotifyKey key{};
			key.eID = EAnimNotifyId::Trigger_Gimmik;
			key.fTrackPosition = 19.f;
			key.iParam0 = ENUM_TO_UINT(EGimmikType::TeleportRandom_Appear);
			key.fParam0 = 5.f;
			pAnim->Pushback_Notifies(EAnimNotifyPhase::Late, key);
		}
	}

	// Teleport Center
	{
		_int iXibiSkill10_AnimIndex = Get_Owner()->Get_AnimationIndex(L"Animation_Xibi_Skill10");
		if (iXibiSkill10_AnimIndex == -1)
			return E_FAIL;

		CModelAnimation* pAnim = m_pOwnerModel->Get_Animation(iXibiSkill10_AnimIndex);
		if (pAnim == nullptr)
			return E_FAIL;

		// Teleport Center Appear
		{
			AnimNotifyKey key{};
			key.eID = EAnimNotifyId::Trigger_Gimmik;
			key.fTrackPosition = 6.f;
			key.iParam0 = ENUM_TO_UINT(EGimmikType::TeleportCenter_Disappear);
			key.fParam0 = 5.f;
			pAnim->Pushback_Notifies(EAnimNotifyPhase::Late, key);
		}

		// Teleport Center Disappear
		{
			AnimNotifyKey key{};
			key.eID = EAnimNotifyId::Trigger_Gimmik;
			key.fTrackPosition = 22.f;
			key.iParam0 = ENUM_TO_UINT(EGimmikType::TeleportCenter_Appear);
			key.fParam0 = 5.f;
			pAnim->Pushback_Notifies(EAnimNotifyPhase::Late, key);
		}
	}	
	return S_OK;
}

void CXibi_GimmikController::On_TeleportRandom_Disappear()
{
	Get_Owner()->Set_Render(false);

	// Todo. BattleField에서 랜덤 위치 계산
}

void CXibi_GimmikController::On_TeleportRandom_Appear()
{
	Get_Owner()->Set_Render(true);
}

void CXibi_GimmikController::On_TeleportCenter_Disappear()
{
	Get_Owner()->Set_Render(false);

	// Todo. BattleField에서 센터 위치 계산
}

void CXibi_GimmikController::On_TeleportCenter_Appear()
{
	Get_Owner()->Set_Render(true);
}

void CXibi_GimmikController::On_ModelAnimNotify(const AnimNotifyKey& key)
{
	if (key.eID != EAnimNotifyId::Trigger_Gimmik)
		return;

	EGimmikType eType = static_cast<EGimmikType>(key.iParam0);
	if (eType >= EGimmikType::COUNT)
		return;

	switch (eType)
	{
	case Client::CXibi_GimmikController::EGimmikType::TeleportRandom_Disappear:
		On_TeleportRandom_Disappear();
		break;
	case Client::CXibi_GimmikController::EGimmikType::TeleportRandom_Appear:
		On_TeleportRandom_Appear();
		break;
	case Client::CXibi_GimmikController::EGimmikType::TeleportCenter_Disappear:
		On_TeleportCenter_Disappear();
		break;
	case Client::CXibi_GimmikController::EGimmikType::TeleportCenter_Appear:
		On_TeleportCenter_Appear();
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
