#pragma once
#include "UIProgress_Bar.h"
#include "DataStruct_UI.h"

NS_BEGIN(Client)
class CStatCom_Player;
class CUIPlayerAmmo_Progress final : public CUIProgress_Bar
{
	using Super = CUIProgress_Bar;
public:
	typedef struct tagUIPlayerAmmoProgressDesc : public PROGRESS_BAR_DESC
	{
	}PLAYER_AMMO_PROGRESS_DESC;

private:
	CUIPlayerAmmo_Progress(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CUIPlayerAmmo_Progress(const CUIPlayerAmmo_Progress& rhs);
	virtual ~CUIPlayerAmmo_Progress() = default;

public:
	HRESULT Initialize_Prototype() override;
	HRESULT Initialize(void* pArg) override;

	HRESULT Attach_Personal_Info();

public:
	virtual HRESULT Awake(const _uint iCurrentLevelID) override;
	virtual void Update_Priority(const _float fTimeDelta) override;
	virtual void Update(const _float fTimeDelta) override;
	virtual void Update_Late(const _float fTimeDelta) override;
	virtual void Ready_Before_Render(const _float fTimeDelta) override;
	virtual HRESULT Render() override;

	void OnUIEvent(ETriggerEventType eEvent, CGenericUI* pSender)override;
	void Initialize_Visible_Event()override;
	void Initialize_InVisible_Event()override;
	_bool Tick_Visible_Event(const _float fTimeDelta)override;
	_bool Tick_InVisible_Event(const _float fTimeDelta)override;

private:
	HRESULT Ready_Components(PLAYER_AMMO_PROGRESS_DESC* pDesc);
	HRESULT Bind_ShaderResources();

	HRESULT Convert_Stat_To_Ratio();

private:
	CStatCom_Player* m_pPlayerStatCom = { nullptr };

public:
	static CUIPlayerAmmo_Progress* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CGameObject* Clone(void* pArg);
	virtual void Free()override;
};

NS_END