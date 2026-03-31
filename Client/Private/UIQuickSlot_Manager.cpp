#include "pch.h"
#include "UIQuickSlot_Manager.h"

#include "Player.h"
#include "Canvas.h"
#include "GenericUI.h"
#include "Weapon.h"

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
				m_arrWeapons[m_iCurrentSlotIndex].iWeaponTypeIndex = ENUM_TO_UINT(CPlayer::EWEAPON::MELEE);
				m_arrWeapons[m_iCurrentSlotIndex].iWeaponIndex = (_uint)i;
				m_iCurrentSlotIndex++;
			}
				break;
			case ENUM_TO_UINT(Client::CPlayer::MELEE::DUAL):
			{
				m_arrWeapons[m_iCurrentSlotIndex].eWeaponType = EUIWeaponTypes::DUAL;
				m_arrWeapons[m_iCurrentSlotIndex].iWeaponTypeIndex = ENUM_TO_UINT(CPlayer::EWEAPON::MELEE);
				m_arrWeapons[m_iCurrentSlotIndex].iWeaponIndex = (_uint)i;
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
				m_arrWeapons[m_iCurrentSlotIndex].iWeaponTypeIndex = ENUM_TO_UINT(CPlayer::EWEAPON::RANGE);
				m_arrWeapons[m_iCurrentSlotIndex].iWeaponIndex = (_uint)i;
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

	// quick slot 열 수 있는지 체크
	m_bQuickSlotOpen = m_pPlayer->Get_CanQuickSlotOpen();
}

void CUIQuickSlot_Manager::Request_Change_Weapon(_uint iSlotIndex)
{
	if (iSlotIndex >= m_arrWeapons.size())
		return;
	if (m_arrWeapons[iSlotIndex].eWeaponType == EUIWeaponTypes::END)
		return;

	// 각 무기 종류별로 main으로 들고 있을 인덱스 교체 
	m_pPlayer->Change_MainWeapon(m_arrWeapons[iSlotIndex].iWeaponTypeIndex, m_arrWeapons[iSlotIndex].iWeaponIndex);
	// 그 선택된 종류의 무기를 hold로 교체
	m_pPlayer->Change_WeaponState(m_arrWeapons[iSlotIndex].iWeaponTypeIndex, ENUM_TO_UINT(CWeapon::State::HOLD));
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