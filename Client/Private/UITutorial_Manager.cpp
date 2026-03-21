#include "pch.h"
#include "UITutorial_Manager.h"
#include "PlayerControlContext.h"
#include "Player.h"
#include "FileUtils.h"
#include "Engine_Utils.h"
#include "Canvas.h"
#include "GenericUI.h"
#include "GameInstance.h"

NS_BEGIN(Client)

IMPLEMENT_SINGLETON(CUITutorial_Manager)

CUITutorial_Manager::CUITutorial_Manager()
	:m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}

void CUITutorial_Manager::Initialize()
{
	CGameObject* pResult = m_pGameInstance->Get_GameObject_Front(ENUM_TO_UINT(ELevelType::STATIC), g_wszPlayerLayer);
	if (nullptr == pResult)
		return;

	auto* pPlayer = dynamic_cast<CPlayer*>(pResult);
	if (nullptr == pPlayer)
		return;

	auto* pControlContext = pPlayer->Get_Component<CControlContext>();
	if (nullptr == pControlContext)
		return;

	auto* pPlayerControlContext = dynamic_cast<CPlayerControlContext*>(pControlContext);
	if (nullptr == pPlayerControlContext)
		return;

	pPlayerControlContext->Set_AllKeyFlag(false);			
	pPlayerControlContext->Set_CheckKey(CPlayerControlContext::KEYFLAGS::MOVE, true);

}

void CUITutorial_Manager::Tutorial_Update(const _float fTimeDelta)
{
	if (m_isChangeState)
	{
		CGameObject* pResult = m_pGameInstance->Get_GameObject_Front(ENUM_TO_UINT(ELevelType::STATIC), g_wszPlayerLayer);
		if (nullptr == pResult)
			return;

		auto* pPlayer = dynamic_cast<CPlayer*>(pResult);
		if (nullptr == pPlayer)
			return;

		auto* pControlContext = pPlayer->Get_Component<CControlContext>();
		if (nullptr == pControlContext)
			return;

		auto* pPlayerControlContext = dynamic_cast<CPlayerControlContext*>(pControlContext);
		if (nullptr == pPlayerControlContext)
			return;

		switch (m_eTutorialToPlayerState)
		{
		case Client::EUITutorialTypeToPlayerState::UNLOCK_JUMP:
			pPlayerControlContext->Set_CheckKey(CPlayerControlContext::KEYFLAGS::JUMP, true);
			break;
		case Client::EUITutorialTypeToPlayerState::UNLOCK_SLIDE:
			pPlayerControlContext->Set_CheckKey(CPlayerControlContext::KEYFLAGS::SPECIAL, true);
			break;
		case Client::EUITutorialTypeToPlayerState::UNLOCK_LATTAK:
			pPlayerControlContext->Set_CheckKey(CPlayerControlContext::KEYFLAGS::COMBO, true);
			break;
		case Client::EUITutorialTypeToPlayerState::UNLOCK_RATTAK:
			pPlayerControlContext->Set_CheckKey(CPlayerControlContext::KEYFLAGS::GUN, true);
			break;
		case Client::EUITutorialTypeToPlayerState::UNLOCK_E:
			pPlayerControlContext->Set_CheckKey(CPlayerControlContext::KEYFLAGS::SKILL1, true);
			break;
		case Client::EUITutorialTypeToPlayerState::UNLOCK_Q:
			pPlayerControlContext->Set_CheckKey(CPlayerControlContext::KEYFLAGS::SKILL2, true);
			break;
		case Client::EUITutorialTypeToPlayerState::END:
			break;
		default:
			break;
		}
	}
}

void CUITutorial_Manager::Set_Current_Tutorial_Step(EUITutorialTypeToPlayerState eState)
{
	m_eTutorialToPlayerState = eState;
	m_isChangeState = true;
}

void CUITutorial_Manager::Free()
{
	Safe_Release(m_pGameInstance);
	Super::Free();
}

NS_END