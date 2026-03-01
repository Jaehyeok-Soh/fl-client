#pragma once
#include "Base.h"

/*
스킬마다 사용하는 방식이 다양하므로
가상함수로 많이 빼둠.

필요시 오버라이딩을 이용해서 사용할 것
*/

NS_BEGIN(Engine)
class CMyStat;

class ENGINE_DLL CSkillBase : public CBase
{
	using Super = CBase;

public:
	enum SkillFlags : Flags
	{
		// start 할때 stat 값에 어케 해줄래
		// add는 기존 값에 더하기
		// set은 아예 새로 셋팅
		Attack_Add		= 0x0000001
		, Sheild_Add	= 0x0000002
		, SkillAtt_Add	= 0x0000004
	
		,Attack_Set = 0x0000001
		, Sheild_Set = 0x0000002
		, SkillAtt_Set = 0x0000004

		, Mental_Sub = 0x0000008 // 멘탈 처음에 삭제 할건지ㅋㅇ

		// end 할때 stat 값에 어케 해줄래
		, Attack_Sub	= 0x0000010
		, Sheild_Sub	= 0x0000020
		, SkillAtt_Sub	= 0x0000040
	};


	typedef struct tagSkillDesc
	{
		SKILL_TYPE		eSkillType = { SKILL_TYPE::END };
		TIME_COUNTER	tCoolTimer = { };	// 다음 공격까지 cooltime

		_float		fNeedMental = { 0.f };		// 공격하기 위한 정신력 정도 (양수로 넣어주세요)

		/* skill 자체 att, sheild */
		_float		fSkillAttack = { 0.f };
		_float		fSkillSheild = { 0.f };

		/* CMyStat에 값의 변동을 줄 att, sheild */
		_float		fStatAttack = { 0.f };
		_float		fStatSheild = { 0.f };

	}SKILL_INFO;

	typedef struct tagSkillComponentDesc
	{
		SKILL_INFO	tSkillInfo = {};

		_bool		bCountTime = { false }; // time을 카운트 할거니?
		_float		fSkillTime = { 0.f };   // 그래서 skill 지속 time

		Flags		FSkillFlags = { 0 };	//SkillFlags 이거를 이용할 것
	}SKILL_DESC;

protected:
	CSkillBase();
	virtual ~CSkillBase() = default;

	virtual HRESULT Initialize(void* pArg);

public:
	virtual void Update(const _float fTimeDelta);

	// start와 end는 외부에서 호출해준다
public:
	virtual _bool Start_Skill(CMyStat* pStatCom = nullptr); // stat에 있는 값을 건들일 수 있어서 값 던져줌 : aciton skill에서 던져주고 있는다
	virtual void End_Skill(CMyStat* pStatCom = nullptr);
	 
	virtual _bool On_Collision(const _float fTimeDelta, CGameObject* pObj = nullptr); // 충돌 했을 때

	// getter setter fucs
public:
	const SKILL_INFO& Get_SkillDesc() const { return m_tSkillInfo; }

	_bool Is_OnSkill() const { return m_bOnSkill; }
	_bool Is_EndSkill() const { return m_bEndSkill; }

	void Set_CountTime(_bool bCount) { m_bCountTime = bCount; }

	_uint Get_SkillCount() const { return m_iOnSkillCount; }

protected:
	_bool		m_bEndSkill = { false };
	_bool		m_bOnSkill = { false };

	SKILL_INFO	m_tSkillInfo = {};

	/* skill 지속 타이머 관련 멤버 변수 */
	_bool		m_bCountTime = { false };		// skill 지속 시간을 카운팅 할거니
	TimeCount	m_TSkillTimer = { 0.f,0.f };	// TimeCount : x는 acc 값, y는 max 값

	_uint		m_iOnSkillCount = { 0 };		// 이미 skill이 실행되고 또 skill을 적용가능 할 시를 위해

	Flags		m_FSkillFlags = { 0 };

protected:
	// 시작 전에 start 할수 있는지 없는지 검사 하는 함수
	virtual _bool Can_StartSkill(CMyStat* pStatCom = nullptr);

	virtual void Update_Skill(const _float fTimeDelta); // skill on시 계속적으로 처리해아하는 로직
	virtual void Count_SkillTime(const _float fTimeDelta);   // skill cool time을 체크
	virtual void Count_NextCoolTime(const _float fTimeDelta);

	// skill 시작과 끝에 flag값을 확인하고
	// flag에 맞게 stat com의 값을 제어한다
	virtual void Check_StartFlag(CMyStat* pStatCom);
	virtual void Check_EndFlag(CMyStat* pStatCom);

public:
	static CSkillBase* Create(void* pArg);
	virtual void Free() override;
};
NS_END

