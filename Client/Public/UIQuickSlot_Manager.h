#pragma once
#include "Base.h"

NS_BEGIN(Engine)
class CGameInstance;
NS_END

NS_BEGIN(Client)


class CCanvas;
class CGenericUI;

class CUIQuickSlot_Manager final : public CBase
{
	DECLARE_SINGLETON(CUIQuickSlot_Manager)

	enum class EUIWeaponTypes
	{
		BASIC, DUAL, GUN, END
	};
	_wstring UIWeaponTypeToTextureTag(EUIWeaponTypes eType)
	{
		switch (eType)
		{
		case CUIQuickSlot_Manager::EUIWeaponTypes::BASIC:
			return L"Texture_T_Walnut_Broadsword_Huipo";
			break;
		case CUIQuickSlot_Manager::EUIWeaponTypes::DUAL:
			return L"Texture_T_Walnut_Dualblade_Yousha";
			break;
		case CUIQuickSlot_Manager::EUIWeaponTypes::GUN:
			return L"Texture_T_Walnut_Machinegun_Chixing";
			break;
		default:
			return L"";
		}
	}
	typedef struct tagUIWeaponInfo
	{
		EUIWeaponTypes eWeaponType = { EUIWeaponTypes::END };

		_uint iWeaponTypeIndex = {};
		_uint iWeaponIndex = {};
	}UI_WEAPON_INFO;
private:
	CUIQuickSlot_Manager();
	virtual ~CUIQuickSlot_Manager() = default;

public:
	void Tick_Player_Weapon_State(const _float fTimeDelta);
	void Request_Change_Weapon(_uint iSlotIndex);


	// image가 받을 bool 값
public:
	_bool Get_QuickSlotOpen() const { return m_bQuickSlotOpen; }

private:
	HRESULT Cache_Player();

public:
	array<UI_WEAPON_INFO, 8>& Get_Weapons() { return m_arrWeapons; }
	const UI_WEAPON_INFO& Get_Weapons(_uint index) { return m_arrWeapons[index]; }

private:
	CGameInstance* m_pGameInstance = { nullptr };

	array<UI_WEAPON_INFO, 8> m_arrWeapons = {};

	class CPlayer* m_pPlayer = { nullptr };
	_uint m_iCurrentSlotIndex = {};

	// quick slot 열 수 있는지 -> player쪽에서 넘겨줌
	_bool m_bQuickSlotOpen = { false };

public:
	virtual void Free()override;
};

NS_END