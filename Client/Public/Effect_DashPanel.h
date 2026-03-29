#pragma once
#include "Effect.h"

NS_BEGIN(Client)

class CEffect_DashPanel :
    public Effect
{
	using Super = Effect;

protected:
	CEffect_DashPanel(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CEffect_DashPanel(const CEffect_DashPanel& rhs);
	virtual ~CEffect_DashPanel() = default;

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

	// 풀 받아온 이펙트를 사용할 때 사용할 것.
protected:
	virtual HRESULT Spawn_FromPool(void* pArg) override;
	// 캐싱헤서 이펙트를 사용할 때 사용할 것.
public:
	virtual HRESULT Enable_VFX(void* pArg) override;
	virtual HRESULT Disable_VFX() override;

public:
	void DashFinished();

protected:
	virtual void Spawn_PositionCalculate(void* pArg) override;

public:
	static CEffect_DashPanel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

NS_END