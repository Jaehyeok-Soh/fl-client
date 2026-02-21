#pragma once
#include "Component.h"

NS_BEGIN(Engine)

class ENGINE_DLL CMyStat : public CComponent
{
	using Super = CComponent;
public:
	typedef struct tagStatComponentDesc
	{
		_float	fMaxHp		= { 0 };
		_float	fAttack		= { 0 };
		_float	fSheild		= { 0 };
	}STAT_DESC;
protected:
	CMyStat();
	explicit CMyStat(const CMyStat& rhs);
	virtual ~CMyStat() = default;

	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

public:
	virtual void Add_Health(_float fHealth); // 만약 객체 마다 hp 증감을 다르게 다루고 싶을 수 있으니 가상함수로
	
	// getter setter funcs
public:
	_float Get_HealthRatio() const { return m_fHealth / m_fMaxHealth; }
	_bool Is_HealthZero() const { return m_fHealth <= 0; }

	_float Get_Attack() const { return m_fAttack; }
	_float Get_Sheild() const { return m_fSheild; }

	void Set_Attack(_float fAttack) { m_fAttack = fAttack; }
	void Set_SheildFlag(_bool bSheildOn) { m_bSheildOn = bSheildOn; }

protected:
	_float			m_fMaxHealth		= { -1.f }; // Max가 0이면 안되어서 음수로 해둠
	_float			m_fHealth			= { 0 };

	_float			m_fAttack			= { 0 };	// 공격력 : 플레이어인 경우, 현재 state에 따른 공격량을 바꿔줄 예정.

	_float			m_fSheild			= { 0 };	// 방어력
	_bool			m_bSheildOn			= { false };

protected:
	virtual void Add_Hp(_float fHealth);		// 매게변수 값이 양수일때
	virtual void Sub_Hp(_float fHealth);		// 매게변수 값이 음수일때

public:
	static CMyStat* Create();
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END