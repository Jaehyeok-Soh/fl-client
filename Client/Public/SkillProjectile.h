#pragma once
#include "SkillObjectBase.h"

NS_BEGIN(Client)

class CSkillProjectile abstract : public CSkillObjectBase
{
	using Super = CSkillObjectBase;
public:
	enum class EState : _uint
	{
		FLY = 0,
		IMPACT
	};
protected:
    CSkillProjectile(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CSkillProjectile(const CSkillProjectile& rhs);
    virtual ~CSkillProjectile() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;

    virtual HRESULT Awake(const _uint iCurrentLevelID) override;
    virtual void Update(const _float fTimeDelta) override;

    virtual void OnTrigger_Enter(_uint iMyLayer, _uint iOtherLayer, Engine::CGameObject* pOther, const COL_HIT_INFO& tHitInfo) final override;
    virtual void Try_Attack(const HIT_DESC& hitDesc) override;
protected:
    // 상태에 따른 override
    virtual void Update_HybridState(const _float fTimeDelta) override;

    // 상태 Enter / Exit
    virtual void On_StateEnter(_uint iState) override;
    virtual void On_StateExit(_uint iState) override;

    // 수명이 끝났을때 처리 파생에서 정의
    // 투사체의 경우 FLAG에 따라 Life가 다되거나 Distance가 멀어지면 죽는게 Default
    virtual void On_LifeOver() final override;

    // 기본 정책은 Map에 맞으면 FlyState의 Loop을 꺼버리고, ImpacState로 전환
    // 관통형, 원샷용 스킬로 충돌 처리만하고 LifeTime에 의해 죽게둘것인가 등을 파생클래스에서 결정하도록 virtual로 둠
    virtual void Handle_Hit(_uint iMyLayer, _uint iOtherLayer, Engine::CGameObject* pOther, const COL_HIT_INFO& tHitInfo);
public:
    virtual CGameObject* Clone(void* pArg) PURE;
    virtual void Free() override;
};

NS_END