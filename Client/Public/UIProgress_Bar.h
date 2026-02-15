#pragma once
#include "GenericUI.h"
#include "DataStruct_UI.h"

NS_BEGIN(Client)
class CStatComponent;
class CUIProgress_Bar final : public CGenericUI
{
	using Super = CGenericUI;
public:
	typedef struct tagUIProgressBarDesc : public GENERIC_UI_DESC
	{
		CStatComponent* pTargetStat;
		DTO::EUISubClassType eOwner;
	}PROGRESS_BAR_DESC;

private:
	CUIProgress_Bar(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CUIProgress_Bar(const CUIProgress_Bar& rhs);
	virtual ~CUIProgress_Bar() = default;

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
	HRESULT Ready_Components(PROGRESS_BAR_DESC* pDesc);
	HRESULT Bind_ShaderResources();

private:
	// Player HP Func
	void Low_HP(const _float fTimeDelta);

private:
	CStatComponent* m_pTargetStat = { nullptr };
	DTO::EUISubClassType m_eSubClassType = {};

	// Lerp Movement Values
	_float	m_fCurRatio = {};
	_float	m_fPreRatio = {};
	_float	m_fStartRatio = {};
	_float	m_fTargetRatio = {};
	_float	m_fDuration = { 0.2f };
	_float	m_fTimeAcc = {};
	_bool	m_isChangeRatio = { false };

	_float m_fDelayTimeAcc = {};


	// Player HP Values
	_bool m_isStartLowHp = { FALSE };
	_bool m_isEndLowHp = { FALSE };
	_float m_fTickTimeAcc = {};
	Vec4 m_vOriginColor = {};
	Vec4 m_vLowHpColor = {};
	_bool m_isHPPaulse = {};

public:
	static CUIProgress_Bar* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CGameObject* Clone(void* pArg);
	virtual void Free()override;
};

NS_END