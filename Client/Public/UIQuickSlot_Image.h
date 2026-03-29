#pragma once
#include "UIDynamic_Image.h"

NS_BEGIN(Engine)
struct DelegateHandle;
NS_END

NS_BEGIN(Client)

enum class EQuickSlotVisibleState
{
	VISIBLE,
	INVISIBLE,
	END
};
class CUIQuickSlot_Image final : public  CUIDynamic_Image
{
	using Super = CUIDynamic_Image;
public:
	typedef struct tagUIQuickSlotImageDesc : public DIMAGE_DESC
	{
		_uint iNumbering = {}; // 0ÀÌ À§ÂÊ
	}QUICK_SLOT_IMAGE_DESC;
private:
	CUIQuickSlot_Image(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CUIQuickSlot_Image(const CUIQuickSlot_Image& rhs);
	virtual ~CUIQuickSlot_Image() = default;
public:
	HRESULT Initialize_Prototype() override;
	HRESULT Initialize(void* pArg) override;
public:
	virtual HRESULT Awake(const _uint iCurrentLevelID) override;
	virtual void Update_Priority(const _float fTimeDelta) override;
	virtual void Update(const _float fTimeDelta) override;
	virtual void Update_Late(const _float fTimeDelta) override;
	virtual void Ready_Before_Render(const _float fTimeDelta) override;
	virtual HRESULT Render() override;
private:
	HRESULT Ready_Components(QUICK_SLOT_IMAGE_DESC* pDesc);
	HRESULT Bind_ShaderResources();
	virtual HRESULT Attach_Personal_Info()override;
	virtual void Bind_Events()override;
	virtual void Tick_By_Type(const _float fTimeDelta)override;
private:
	virtual void Initialize_Visible_Event()override;
	virtual _bool Tick_Visible_Event(const _float fTimeDelta)override;
	virtual void Initialize_InVisible_Event()override;
	virtual _bool Tick_InVisible_Event(const _float fTimeDelta)override;
private:
	// WEAPON_QUIKSLOT_CENTER
	void Calc_Current_SlotIndex();
	void Tick_Follow_TargetSlot(const _float fTimeDelta);

	// WEAPON_QUIKSLOT_SIDE_BG_FX
	void Check_Current_Slot();

	// WEAPON_QUIKSLOT_SIDE_SLOT
	void WeaponType_To_Icon();

private:
	_uint m_iNumbering = {};
	EQuickSlotVisibleState m_eVisibleState = EQuickSlotVisibleState::INVISIBLE;

	// WEAPON_QUIKSLOT_CENTER
	_uint m_iTargetSlotIndex = {};
	_uint m_iCurSlotIndex = {};
	_float m_fSlotTimeAcc = {};

	// WEAPON_QUIKSLOT_SIDE_BG_FX
	_bool m_isSelected = { false };

public:
	static CUIQuickSlot_Image* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CGameObject* Clone(void* pArg);
	virtual void Free()override;
};

NS_END