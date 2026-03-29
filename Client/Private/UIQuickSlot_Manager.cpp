#include "pch.h"
#include "UIQuickSlot_Manager.h"
#include "Player.h"
#include "Canvas.h"
#include "GenericUI.h"
#include "GameInstance.h"

NS_BEGIN(Client)

IMPLEMENT_SINGLETON(CUIQuickSlot_Manager)
CUIQuickSlot_Manager::CUIQuickSlot_Manager()
	:m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}

void CUIQuickSlot_Manager::Tick_Player_Weapon_State(const _float fTimeDelta)
{
	if (FAILED(Cache_Player()))
		return;

	m_iCurrentSlotIndex = 0;

	for (size_t i = 0; i < ENUM_TO_SZET(CPlayer::MELEE::END); ++i)
	{
		if (m_pPlayer->Get_MeleeInfo()[i].bHave)
		{
			switch (i)
			{
			case ENUM_TO_UINT(Client::CPlayer::MELEE::SWORD):
			{
				if (m_iCurrentSlotIndex >= m_arrWeapons.size())
					break;

				m_arrWeapons[m_iCurrentSlotIndex].eWeaponType = EUIWeaponTypes::BASIC;
				m_iCurrentSlotIndex++;
			}
				break;
			case ENUM_TO_UINT(Client::CPlayer::MELEE::DUAL):
			{
				m_arrWeapons[m_iCurrentSlotIndex].eWeaponType = EUIWeaponTypes::DUAL;
				m_iCurrentSlotIndex++;
			}
				break;
			case ENUM_TO_UINT(Client::CPlayer::MELEE::END):
				break;
			default:
				break;
			}
		}
	}

	for (size_t i = 0; i < ENUM_TO_SZET(CPlayer::RANGE::END); ++i)
	{
		if (m_pPlayer->Get_RangeInfo()[i].bHave)
		{
			switch (i)
			{
			case ENUM_TO_UINT(Client::CPlayer::RANGE::MACHINE):
			{
				m_arrWeapons[m_iCurrentSlotIndex].eWeaponType = EUIWeaponTypes::GUN;
				m_iCurrentSlotIndex++;
			}
				break;
			case ENUM_TO_UINT(Client::CPlayer::RANGE::END):
				break;
			default:
				break;
			}
		}
	}
}

HRESULT CUIQuickSlot_Manager::Cache_Player()
{
	if (m_pPlayer != nullptr)
		return S_OK;

	CGameObject* pResult = m_pGameInstance->Get_GameObject_Front(ENUM_TO_UINT(ELevelType::STATIC), g_wszPlayerLayer);
	if (nullptr == pResult)
		return E_FAIL;

	m_pPlayer = dynamic_cast<CPlayer*>(pResult);
	if (nullptr == m_pPlayer)
		return E_FAIL;

	return S_OK;
}

void CUIQuickSlot_Manager::Free()
{
	Safe_Release(m_pGameInstance);
}

NS_END