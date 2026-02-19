#pragma once
#include "MonoBehaviour.h"

NS_BEGIN(Client)

class CStatComponent : public CMonoBehaviour
{
	using Super = CMonoBehaviour;
public:
	typedef struct tagStatComponentDesc
	{
		_uint	iMaxHp		= { 0 };
		_uint	iAttack		= { 0 };
		_uint	iSheild		= { 0 };
	}STATCOMP_DESC;
protected:
	CStatComponent();
	explicit CStatComponent(const CStatComponent& rhs);
	virtual ~CStatComponent() = default;

	virtual HRESULT Initialize_Prototype() override;

public:
	virtual HRESULT Awake(_uint iLevelIndex) override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Update(const _float fTimeDelta) override;

public:
	virtual void Add_Health(_int iHealth); // 만약 객체 마다 hp 증감을 다르게 다루고 싶을 수 있으니 가상함수로
	
	// getter setter funcs
public:
	_float Get_HealthRatio() const { return (_float)m_iHealth / (_float)m_iMaxHealth; }
	_bool Is_HealthZero() const { return m_iHealth <= 0; }

	_uint Get_Attack() const { return m_iAttack; }
	_uint Get_Sheild() const { return m_iSheild; }

	void Set_Attack(_uint iAttack) { m_iAttack = iAttack; }
	void Set_SheildFlag(_bool bSheildOn) { m_bSheildOn = bSheildOn; }

protected:
	CGameInstance*	m_pGameInstance		= { nullptr };
	_uint			m_iMaxHealth		= { 0 };
	_int			m_iHealth			= { 0 };

	_uint			m_iAttack			= { 0 };	// 공격력 : 플레이어인 경우, 현재 state에 따른 공격량을 바꿔줄 예정.

	_uint			m_iSheild			= { 0 };	// 방어력
	_bool			m_bSheildOn			= { false };

protected:
	virtual void Add_Hp(_int iHealth);		// 매게변수 값이 양수일때
	virtual void Sub_Hp(_int iHealth);		// 매게변수 값이 음수일때

public:
	static CStatComponent* Create();
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END