#pragma once
#include "StatComponent.h"

NS_BEGIN(Client)
class CStatCom_Player final : public CStatComponent
{
	using Super = CStatComponent;

public:


private:
	CStatCom_Player();
	explicit CStatCom_Player(const CStatCom_Player& rhs);
	virtual ~CStatCom_Player() = default;

	virtual HRESULT Initialize_Prototype()			override;

public:
	virtual HRESULT Awake(_uint iLevelIndex)		override;
	virtual HRESULT Initialize(void* pArg)			override;
	virtual void	Update(const _float fTimeDelta) override;

	// stats
private:
	_float		m_fAttackEnergy		= { 0.f };
	_float		m_fPlusHp			= { 0.f };

private:
	_uint		m_iDashCount = { 2 };
	_uint		m_fQSkill;
	_uint		m_fESkill;

	// timers
private:
	TimeCount	m_TDashTimer = { 0.f,1.f };
	TimeCount	m_TQSkill_Timer = { 0.f,2.f };
	TimeCount	m_TESkill_Timer = { 0.f,2.f };

public:
	static CStatCom_Player* Create();
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END