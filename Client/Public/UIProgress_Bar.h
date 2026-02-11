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
		DTO::EUIOwnerType eOwner;
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

private:
	HRESULT Ready_Components(PROGRESS_BAR_DESC* pDesc);
	HRESULT Bind_ShaderResources();

private:
	CStatComponent* m_pTargetStat = { nullptr };
	DTO::EUIOwnerType m_eOwnerType = {};

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
	static CUIProgress_Bar* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CGameObject* Clone(void* pArg);
	virtual void Free()override;
};

NS_END