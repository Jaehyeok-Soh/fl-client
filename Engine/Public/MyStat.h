#pragma once
#include "Component.h"

NS_BEGIN(Engine)
class CSkillBase;

class ENGINE_DLL CMyStat : public CComponent
{
	using Super = CComponent;
public:
	constexpr static EComponentType _ID = EComponentType::STAT;

	enum class STAT_TYPE { HP, DEFENSE, MENTAL, ATTACK, SHEILD, SKILL };

	enum StatFlags : Flags
	{
		HpUpdate		= 0x000001 // 자동으로 hp를 업데이트를 할거니
		,DefenseUpdtae	= 0x000002 // 자동으로 defense를	업데이트를 할거니
		,MentalUpdate	= 0x000004 // 자동으로 mental을 업데이트를 할거니

		,SheildOn		= 0x000008
	};

	typedef struct tagStatComponentDesc
	{
		_float	fMaxHp		= { 0.f };
		_float	fDefense	= { 0.f };
		_float	fMental		= { 0.f };

		_float	fAttack		= { 0.f };
		_float	fSheild		= { 0.f };

		Flags	FStatFlags	= 0;		// StatFlags 이용하시길
	}STAT_DESC;

protected:
	CMyStat();
	explicit CMyStat(const CMyStat& rhs);
	virtual ~CMyStat() = default;

	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

public:
	virtual void Add_Health(_float fHealth); // 만약 객체 마다 hp 증감을 다르게 다루고 싶을 수 있으니 가상함수로
	virtual void Add_Stat(STAT_TYPE eType, _float fValue);

	virtual void Update_Stat(const _float fTimeDelta); // stat 자동 업데이트
	
	// getter setter funcs
public:
	_float Get_HealthRatio() const { return m_vHealth.x / m_vHealth.y; }
	_bool Is_HealthZero() const { return m_vHealth.x <= 0; }

	void Set_Stat(STAT_TYPE eType, _float fValue);

	const Vec2& Get_Stat_Vec2(STAT_TYPE eType) const
	{
		//HP, DEFENSE, MENTAL
		switch (eType)
		{
		case STAT_TYPE::HP:
			return m_vHealth;

		case STAT_TYPE::DEFENSE:
			return m_vDefense;

		case STAT_TYPE::MENTAL:
			return m_vMental;
		}

		return Vec2::Zero;
	}

	_float Get_Rate(STAT_TYPE eType)
	{
		switch (eType)
		{
		case STAT_TYPE::HP:
			return m_vHealth.x / m_vHealth.y;

		case STAT_TYPE::DEFENSE:
			return m_vDefense.x / m_vDefense.y;

		case STAT_TYPE::MENTAL:
			return  m_vMental.x / m_vMental.y;
		}

		return -1.f;
	}

	_float Get_Stat_Value(STAT_TYPE eType) const
	{
		//HP, DEFENSE, MENTAL
		switch (eType)
		{
		case STAT_TYPE::ATTACK:
			return m_fAttack;

		case STAT_TYPE::SHEILD:
			return m_fSheild;
		}

		return -1.f;
	}

	void Set_Flag(_uint iFlag, _bool bOn);

protected:
	// x는 현재 값, y에는 max 값
	Vec2			m_vHealth	= { 0.f,0.f };
	Vec2			m_vDefense	= { 0.f,0.f };
	Vec2			m_vMental	= { 0.f,0.f };

	_float			m_fAttack	= { 0.f };	// 공격력 : 플레이어인 경우, 현재 state에 따른 공격량을 바꿔줄 예정.

	_float			m_fSheild	= { 0.f };	// 방어력

	_float			m_fSkillAtt	= { 0.f };  // 현재 스킬의 attack 값

	Flags			m_FStatFlags = { 0 };


	inline static Vec2		m_vDummy{0.f,0.f};

protected:
	virtual void Add_Hp(_float fHealth);		// 매게변수 값이 양수일때
	virtual void Sub_Hp(_float fHealth);		// 매게변수 값이 음수일때

	void		Add_Defense(_float fValue);
	void		Add_Mental(_float fValue);
	void		Add_SkillAtt(_float fValue);

	virtual void Update_Hp(const _float fTimeDelta);
	virtual void Update_Defense(const _float fTimeDelta);
	virtual void Update_Mental(const _float fTimeDelta);

public:
	static CMyStat* Create();
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END