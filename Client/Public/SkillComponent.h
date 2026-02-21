#pragma once
#include "MonoBehaviour.h"

/* 
플레이어의 하나의 스킬을 담당한다

하나의 컴포넌트로 뺀 이유 : 스킬 내부에서 처리할 것들이 많아져서
컴포넌트로 따로 빼서 관리 한다

*/

NS_BEGIN(Engine)
class CGameObject;
NS_END

NS_BEGIN(Client)
class CStatComponent;

class CSkillComponent : public CMonoBehaviour
{
	using Super = CMonoBehaviour;

public:
	typedef struct tagSkillComponentDesc
	{
		SKILL_DESC	tSkillDesc		= {};

		_bool		bCountTime		= { false };
		_float		fSkillTime		= { 0.f };
		_uint		iPlayerState	= { 0 };

	}SKILLCOMP_DESC;
	
protected:
	CSkillComponent();
	explicit CSkillComponent(const CSkillComponent& rhs);
	virtual ~CSkillComponent() = default;

	virtual HRESULT Initialize_Prototype() override;

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Update(const _float fTimeDelta) override;

public:
	virtual void Start_Skill(CStatComponent* pStatCom = nullptr);
	virtual void Update_Skill(const _float fTimeDelta);
	virtual void End_Skill(CStatComponent* pStatCom = nullptr);

	virtual void On_Collision_Monster(const _float fTimeDelta, CGameObject* pObj = nullptr);

public:
	const SKILL_DESC& Get_SkillDesc() const { return m_tSkillDesc; }

protected:
	_uint		m_iPlayerState = {};
	
protected:
	_bool		m_bEndSkill		= { false };
	_bool		m_bOnSkill		= { false };

	SKILL_DESC	m_tSkillDesc	= {};

	_bool		m_bCountTime	= { false };
	TimeCount	m_TSkillTimer	= { 0.f,0.f };

protected:
	void Count_SkillTime(const _float fTimeDelta);

public:
	static CSkillComponent* Create();
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};
NS_END
