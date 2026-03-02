#pragma once
#include "GenericUI.h"
#include "DataStruct_UI.h"

NS_BEGIN(Client)
class CStatComponent;
class CUIProgress_Bar abstract : public CGenericUI
{
	using Super = CGenericUI;
public:
	typedef struct tagUIProgressBarDesc : public GENERIC_UI_DESC
	{
		DTO::EUISubClassType eOwner;
	}PROGRESS_BAR_DESC;
protected:
	CUIProgress_Bar(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CUIProgress_Bar(const CUIProgress_Bar& rhs);
	virtual ~CUIProgress_Bar() = default;
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
protected:
	HRESULT Ready_Components(PROGRESS_BAR_DESC* pDesc);
	HRESULT Bind_ShaderResources();

	void Trigger_Ratio();
	void Tick_Ratio(const _float fTimeDelta);

protected:
	virtual HRESULT Spawn_FromPool(void* pArg) override;
	virtual HRESULT Despawn_FromPool()override;
protected:
	DTO::EUISubClassType m_eSubClassType = {};

	// Lerp Movement Values
	_float	m_fCurRatio		= {};
	_float	m_fPreRatio		= {};
	_float	m_fStartRatio	= {};
	_float	m_fTargetRatio	= {};
	_float	m_fDuration		= { 0.2f };
	_float	m_fTimeAcc		= {};
	_bool	m_isChangeRatio = { false };
	_float m_fDelayTimeAcc	= {};

public:
	virtual void Free()override;
};

NS_END