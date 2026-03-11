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

	typedef struct tagFKeyData
	{
		_uint iKeyEvent = { 0 };				// 어떤 이벤트인지
		_bool bEventCheckOn = { false };		// 이벤트 활성화 됐니?
	}FKEY_DATA;

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
	Vec3 Get_VicPosition() const;

	void Reset_HitDesc() { m_tPreHitDesc = {};  m_fAttackFlag = 0; }

	_bool Can_FKeyEvent();
	_uint Get_CurFKeyEvent() const { return m_tFKeyData.iKeyEvent; }

public:
	void		Set_FKeyEvent(_uint iEvnet, _bool bOn);

private:
	Flags			m_fAttackFlag = {};
	HIT_DESC		m_tPreHitDesc = {};

	FKEY_DATA		m_tFKeyData = {};

public:
	static CPlayerActionState* Create();
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};
NS_END