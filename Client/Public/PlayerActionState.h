#pragma once
#include "ActionState.h"

NS_BEGIN(Engine)
class CNavigation;
NS_END

NS_BEGIN(Client)

class CPlayerActionState final : public CActionState
{
	using Super = CActionState;

public:
	enum AttackFlag : Flags
	{
		// hit가 되었니
		AF_OnHit	= 0x00001

		// 어떤 공격으로 들어온거니
		, AF_Addtive	= 0x00002
		, AF_Fly	= 0x00004
		, AF_Strong = 0x00008
	};

private:
	CPlayerActionState();
	CPlayerActionState(const CPlayerActionState& rhs);
	virtual ~CPlayerActionState() = default;

	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

public:
	void Set_Flag(Flags FActionFlag, _bool bOn);
	void Set_HitDesc(const HIT_DESC& tHit);

	_bool Is_OnHit();
	Flags Get_AttackFlag() const { return m_fAttackFlag; }
	Vec3 Get_HitNormal() const { return m_tPreHitDesc.vHitNormal; }

	void Reset_HitDesc() { m_tPreHitDesc = {};  m_fAttackFlag = 0; }

private:
	Flags			m_fAttackFlag = {};
	HIT_DESC		m_tPreHitDesc = {};

public:
	static CPlayerActionState* Create();
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};
NS_END