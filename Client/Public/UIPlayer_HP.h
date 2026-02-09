#pragma once
#include "GenericUI.h"
#include "DataStruct_UI.h"

NS_BEGIN(Client)
class CCanvas;
class CStatComponent;
class CUIPlayer_HP final : public CGenericUI
{
	using Super = CGenericUI;
public:
	typedef struct tagUIPlayerHPDesc : public GENERIC_UI_DESC
	{
		CStatComponent* pTargetStat;
	}PLAYER_HP_DESC;

private:
	CUIPlayer_HP(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CUIPlayer_HP(const CUIPlayer_HP& rhs);
	virtual ~CUIPlayer_HP() = default;

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
	HRESULT Ready_Components(PLAYER_HP_DESC* pDesc);
	HRESULT Bind_ShaderResources();

private:
	CStatComponent* m_pTargetStat = { nullptr };
	uint32_t m_iFillDir = {};

public:
	static CUIPlayer_HP* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CGameObject* Clone(void* pArg);
	virtual void Free()override;
};

NS_END