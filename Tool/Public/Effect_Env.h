#pragma once
#include "Effect.h"

NS_BEGIN(Tool)

class CEffect_Env :
    public Effect
{
	using Super = Effect;

protected:
	CEffect_Env(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CEffect_Env(const CEffect_Env& rhs);
	virtual ~CEffect_Env() = default;

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

public:
	virtual void Update_CombinedWorldMatrix() override;

protected:
	virtual HRESULT Spawn_FromPool(void* pArg) override;

public:
	virtual HRESULT Enable_VFX(void* pArg) override;
	virtual HRESULT Disable_VFX() override;

protected:
	virtual void Spawn_PositionCalculate(void* pArg) override;
	SimpleMath::Matrix WorldMatrix_Calculate(const EFFECT_ENV_DESC* pArg);

private:
	void Spawn_PartsSetting(void* pArg);

public:
	static CEffect_Env* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

private:
	EFFECT_ENV_DESC m_tEnvDesc = {};
public:
	void Set_EnvDesc(const EFFECT_ENV_DESC& tDesc) { m_tEnvDesc = tDesc; }
	EFFECT_ENV_DESC& Get_EnvDesc() { return m_tEnvDesc; }
};

NS_END