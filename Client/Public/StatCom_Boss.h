#pragma once
#include "MyStat.h"

NS_BEGIN(Client)

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
	_bool Is_Groggy() const { return m_vGroggy.y <= m_vGroggy.x; }
	void Sub_Groggy(_float fGroggy) { m_vGroggy.y -= fGroggy; }
	_float Get_CurrentGroggy() const { return m_vGroggy.y; }
private:
	Vec2						m_vGroggy				= {0.f, 100.f}; // min max
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