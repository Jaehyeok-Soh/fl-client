#pragma once
#include "Component.h"

NS_BEGIN(Engine)
class CMyStat;

class ENGINE_DLL CSkill : public CComponent
{
	using Super = CComponent;

public:
	enum class SKILL_TYPE { DAMAGE, BUFF, SUMMON, CURE, DEFENSE, END }; // skill의 타입

	typedef struct tagSkillDesc
	{
		SKILL_TYPE	eSkillType = { SKILL_TYPE::END };
		TimeCount	TCoolTime = { 0.f,0.f };	// 다음 공격까지 cooltime

		_float		fNeedMental = { 0.f };		// 공격하기 위한 정신력 정도

		/* skill 자체 att, sheild */
		_float		fSkillAttack = { 0.f };
		_float		fSkillSheild = { 0.f };

		/* CMyStat에 값의 변동을 줄 att, sheild */
		_float		fStatAttack = { 0.f };
		_float		fStatSheild = { 0.f };
	}SKILL_DESC;

	typedef struct tagSkillComponentDesc
	{
		SKILL_DESC	tSkillDesc = {};

		_bool		bCountTime = { false }; // time을 카운트 할거니?
		_float		fSkillTime = { 0.f };   // 그래서 skill 지속 time
	}SKILLCOMP_DESC;

protected:
	CSkill();
	explicit CSkill(const CSkill& rhs);
	virtual ~CSkill() = default;

	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

public:
	virtual void Update_Component(const _float fTimeDelta);

	// start와 end는 외부에서 호출해준다
public:
	virtual void Start_Skill(CMyStat* pStatCom = nullptr); // stat에 있는 값을 건들일 수 있어서 값 던져줌
	virtual void End_Skill(CMyStat* pStatCom = nullptr);
	 
	virtual void On_Collision(const _float fTimeDelta, CGameObject* pObj = nullptr); // 충돌 했을 때


	// getter setter fucs
public:
	const SKILL_DESC& Get_SkillDesc() const { return m_tSkillDesc; }

	_bool Is_OnSkill() const { return m_bOnSkill; }
	_bool Is_EndSkill() const { return m_bEndSkill; }

	void Set_EndSkill(_bool bEnd) { m_bEndSkill = bEnd; }
	void Set_CountTime(_bool bCount) { m_bCountTime = bCount; }

protected:
	_bool		m_bEndSkill = { false };
	_bool		m_bOnSkill = { false };

	SKILL_DESC	m_tSkillDesc = {};

	_bool		m_bCountTime = { false };
	TimeCount	m_TSkillTimer = { 0.f,0.f };

protected:
	virtual void Update_Skill(const _float fTimeDelta);
	virtual void Count_SkillTime(const _float fTimeDelta);

public:
	static CSkill* Create();
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};
NS_END

