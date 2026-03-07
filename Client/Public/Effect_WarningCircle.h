#pragma once
#include "Effect.h"

NS_BEGIN(Client)

class CEffect_WarningCircle :
    public Effect
{
	using Super = Effect;

protected:
	CEffect_WarningCircle(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CEffect_WarningCircle(const CEffect_WarningCircle& rhs);
	virtual ~CEffect_WarningCircle() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

public:
	virtual HRESULT Awake(const _uint iCurrentLevelID) override;
	virtual void Update_Priority(const _float fTimeDelta) override;
	virtual void Update(const _float fTimeDelta) override;
	virtual void Update_Late(const _float fTimeDelta) override;
	virtual void Ready_Before_Render(const _float fTimeDelta) override;
	virtual HRESULT Render() override;

protected:
	virtual HRESULT Spawn_FromPool(void* pArg) override;

public:
	virtual HRESULT Enable_VFX(void* pArg) override;
	virtual HRESULT Disable_VFX() override;

protected:
	virtual void Spawn_PositionCalculate(void* pArg) override;
	SimpleMath::Matrix WorldMatrix_Calculate(const EFFECT_WARNING_DESC* pArg);

public:
	static CEffect_WarningCircle* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

NS_END