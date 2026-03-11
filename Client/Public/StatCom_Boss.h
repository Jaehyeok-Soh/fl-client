#pragma once
#include "MyStat.h"

NS_BEGIN(Client)

#define GROGGY_MIN 0.0f
#define GROGGY_MAX 40.0f

class CStatCom_Boss final : public CMyStat
{
	using Super = CMyStat;
public:
	typedef struct tagBossStatDesc : public Super::STAT_DESC
	{
		vector<_uint> vecExtraComputeOrder;
		_float fCriticalRate = { 0.f }; // 0~1
		_float fCriticalAttack = { 0.f }; // 크리티컬 발생시 더해줄 값
	}BOSS_STAT_DESC;
private:
	CStatCom_Boss();
	explicit CStatCom_Boss(const CStatCom_Boss& rhs);
	virtual ~CStatCom_Boss() = default;

	virtual HRESULT Initialize_Prototype()			override;
	virtual HRESULT Initialize(void* pArg)			override;
public:
	EGroggyState Sub_Groggy(_float fValue);
	_float Get_CurrentGroggy() const { return m_vGroggy.x; }
	_float Get_CurrentGRoggyRatio() const { return m_vGroggy.x / m_vGroggy.y; }
private:
	void Reset_GroggyStats();
private:
	_uint						m_iGroggyFlag			= { ENUM_TO_UINT(EGroggyState::None) };
	Vec2						m_vGroggy				= { GROGGY_MAX, GROGGY_MAX }; // current / max
	_float						m_fCriticalRate			= { 0.f }; // 0 ~ 1
	_float						m_fCirticalRate_Add		= { 0.f }; // 0 ~ 1
	_float						m_fCirticalAttack		= { 0.f };
	Engine::EXTRA_ATTACK_DESC	m_tExtra_AttackDesc		= {};
public:
	virtual CComponent* Clone(void* pArg) override;
	static CStatCom_Boss* Create();
	virtual void Free() override;
};

NS_END