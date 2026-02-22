#pragma once
#include "UIProgress_Bar.h"
#include "DataStruct_UI.h"

NS_BEGIN(Client)
class CStatComponent;
class CUILoading_Progress final : public CUIProgress_Bar
{
	using Super = CUIProgress_Bar;
public:
	typedef struct tagLoadingProgressDesc : public PROGRESS_BAR_DESC
	{
	}LOADING_PROGRESS_DESC;

private:
	CUILoading_Progress(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CUILoading_Progress(const CUILoading_Progress& rhs);
	virtual ~CUILoading_Progress() = default;

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

	void OnUIEvent(ETriggerEventType eEvent, CGenericUI* pSender)override;
	void Initialize_Visible_Event()override;
	void Initialize_InVisible_Event()override;
	_bool Tick_Visible_Event(const _float fTimeDelta)override;
	_bool Tick_InVisible_Event(const _float fTimeDelta)override;

private:
	HRESULT Ready_Components(LOADING_PROGRESS_DESC* pDesc);
	HRESULT Bind_ShaderResources();

	// Lerp Movement Values
	_float	m_fCurRatio = {};
	_float	m_fPreRatio = {};
	_float	m_fStartRatio = {};
	_float	m_fTargetRatio = {};
	_float	m_fDuration = { 0.2f };
	_float	m_fTimeAcc = {};
	_bool	m_isChangeRatio = { false };

	_float m_fDelayTimeAcc = {};

public:
	static CUILoading_Progress* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CGameObject* Clone(void* pArg);
	virtual void Free()override;
};

NS_END