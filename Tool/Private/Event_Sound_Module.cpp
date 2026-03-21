#include "pch.h"
#include "Event_Sound_Module.h"
#include "AnimObj.h"
#include "GameInstance.h"

CEvent_Sound_Module::CEvent_Sound_Module()
	: Super()
	, m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}

void CEvent_Sound_Module::Set_Owner(CAnimObj* pOwner)
{
	if (m_pOwner != nullptr && m_pOwner == pOwner)
		return;

	Safe_Release(m_pOwner);
	m_pOwner = pOwner;

	if (m_pOwner == nullptr)
		return;

	Ensure_SoundPlayer();
}

HRESULT CEvent_Sound_Module::Initialize()
{
	return S_OK;
}

HRESULT CEvent_Sound_Module::Ensure_SoundPlayer()
{
	Safe_Release(m_pSoundPlayer);

	if (m_pOwner == nullptr)
		return E_FAIL;

	m_pSoundPlayer = static_cast<CToolAnimSoundPlayer*>(m_pOwner->Get_Script_Component(TEXT("ToolAnimSoundPlayer")));

	if (m_pSoundPlayer == nullptr)
	{
		if (FAILED(m_pOwner->Add_Script_Component(
			L"ToolAnimSoundPlayer",
			L"Prototype_Component_SoundPlayer",
			nullptr)))
		{
			return E_FAIL;
		}

		m_pSoundPlayer = static_cast<CToolAnimSoundPlayer*>(m_pOwner->Get_Script_Component(TEXT("ToolAnimSoundPlayer")));
	}

	if (m_pSoundPlayer == nullptr)
		return E_FAIL;

	Safe_AddRef(m_pSoundPlayer);
	return S_OK;
}

HRESULT CEvent_Sound_Module::Rebuild(const vector<DTO::SOUNDEVENT>& events)
{
	if (m_pOwner == nullptr)
		return E_FAIL;

	if (m_pSoundPlayer == nullptr)
	{
		if (FAILED(Ensure_SoundPlayer()))
			return E_FAIL;
	}

	return m_pSoundPlayer->Rebuild(events);
}

void CEvent_Sound_Module::Clear()
{
	if (m_pSoundPlayer == nullptr)
		return;

	vector<DTO::SOUNDEVENT> empty;
	m_pSoundPlayer->Rebuild(empty);
}

CEvent_Sound_Module* CEvent_Sound_Module::Create()
{
	CEvent_Sound_Module* pInstance = new CEvent_Sound_Module();
	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("CEvent_Sound_Module::Create(), Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEvent_Sound_Module::Free()
{
	Safe_Release(m_pSoundPlayer);
	Safe_Release(m_pGameInstance);
	Super::Free();
}