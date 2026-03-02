#pragma once
#include "Component.h"
#include "SkillBase.h"

NS_BEGIN(Engine)
class CMyStat;

class ENGINE_DLL CActionSkill : public CComponent
{
	using Super = CComponent;

public:
	constexpr static EComponentType _ID = EComponentType::ACTIONSKILL;

	typedef struct tagActionSkillDesc
	{
		_uint iSkillCount = { 0 };

		CMyStat* pOwnerStat = { nullptr };
	}ACTIONSKILL_DESC;

protected:
	CActionSkill();
	explicit CActionSkill(const CActionSkill& rhs);
	virtual ~CActionSkill() = default;

	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

public:
	void Awake(const _uint iCurLevelIndex);
	HRESULT			Add_Skill(_uint iIndex, CSkillBase* pSkill);
	CSkillBase*		Get_Skill(_uint iIndex);

public:
	// vector를 순회하면서 skill 내부업데이트 함수를 호출 해준다
	virtual void	Update_Skills(const _float fTimeDelta);

	// 외부에서 원하는 skill을 끄고 킨다
	virtual _bool	Start_Skill(_uint iIndex);
	virtual void	End_Skill(_uint iIndex);


	// getter setter func
public:
	_bool							Is_EndSkill(_uint iIndex);
	_bool							Is_OnSkill(_uint iIndex);
	const CSkillBase::SKILL_INFO&	Get_SkillDesc(_uint iIndex);

	void							Set_CountTime(_uint iIndex, _bool bCount);

protected:
	CMyStat*				m_pOwnerStat = { nullptr };

	vector<CSkillBase*>		m_vecSkills;
	_uint					m_iSkillCount = {};

protected:
	_bool Check_Index(_uint iIndex);

public:
	static CActionSkill* Create();
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
	friend CSkillBase; // 일단 friend로 해둠
};

NS_END