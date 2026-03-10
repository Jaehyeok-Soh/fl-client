#pragma once
#include "SkillObjectBase.h"

NS_BEGIN(Client)

class CEffect_WarningCircle;

class CSkillWarningSpace : public CSkillObjectBase
{
	using Super = CSkillObjectBase;
public:
	enum class EState : _uint
	{
		WARNING = 0,
		STRIKE,
		DISAPEAR,
		END
	};
protected:
	CSkillWarningSpace(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSkillWarningSpace(const CSkillWarningSpace& rhs);
	virtual ~CSkillWarningSpace() = default;
	
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
public:
	virtual HRESULT Awake(const _uint iCurrentLevelID) override;

	virtual void Update(const _float fTimeDelta) override;
	virtual void OnTrigger_Enter(_uint iMyLayer, _uint iOtherLayer, CGameObject* pOther, const COL_HIT_INFO& tHitInfo) override;
protected:
	virtual void Update_HybridState(const _float fTimeDelta) override;

	virtual void On_StateEnter(_uint iState) override;
	virtual void On_StateExit(_uint iState) override;
	virtual void On_EffectModuleEnter(CGameObject* pModule) override;
protected:
	void Build_WarningDesc();
protected:
	EFFECT_WARNING_DESC m_tWarnDesc{};
public:
	static CSkillWarningSpace* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

NS_END