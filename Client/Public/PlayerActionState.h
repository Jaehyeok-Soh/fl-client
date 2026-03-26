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

	enum class BONE_STATE
	{
		NORMAL, HITSTART, HITEND, END
	};

	enum BoneHitType : Flags
	{
		/* hit 방향 : main player -> on hit 함수 내부에서 전해줌*/
		BHT_Front = 0x00001,
		BHT_BACK = 0x00002,

		/* hit 세기 : cur state 권한 */
		BHT_FORCE_WEAK = 0x00004,
		BHT_FORCE_STRONG = 0x00008,
	};

	typedef struct tagFKeyData
	{
		_uint iKeyEvent = { 0 };				// 어떤 이벤트인지
		_bool bEventCheckOn = { false };		// 이벤트 활성화 됐니?
	}FKEY_DATA;

	typedef struct tagBoneRatio
	{
		_float fTimeAcc = { 0.f };
		_uint iHitType = { 0 }; //BoneHitType 설정

		_float fLerpHalfTime	= 0.18f;
		Matrix matFrontHit_Weak	= Matrix::CreateFromYawPitchRoll(XMConvertToRadians(0.f), XMConvertToRadians(20.f), XMConvertToRadians(50.f));
		Matrix matFrontHit_Strong = Matrix::CreateFromYawPitchRoll(XMConvertToRadians(0.f), XMConvertToRadians(20.f), XMConvertToRadians(80.f));

		Matrix matBackHit_Weak		= Matrix::CreateFromYawPitchRoll(XMConvertToRadians(0.f), XMConvertToRadians(-25.f), XMConvertToRadians(-50.f));
		Matrix matBackHit_Strong = Matrix::CreateFromYawPitchRoll(XMConvertToRadians(0.f), XMConvertToRadians(-20.f), XMConvertToRadians(-80.f));

	}BONEHIT_DATA;

	typedef struct tagBoneChangeArgs
	{
		_uint iHitType = { 0 }; //BoneHitType 설정
	}BONESTATE_CHANGE_ARGS;
	
private:
	CPlayerActionState();
	CPlayerActionState(const CPlayerActionState& rhs);
	virtual ~CPlayerActionState() = default;

	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

public:
	virtual void Update(const _float fTImeDelta) override;

public:
	_bool Get_KeyFlag(_uint iKeyFlag);

	// hit 관련 funcs
public:
	void Set_Flag(Flags FActionFlag, _bool bOn);
	void Set_HitDesc(const HIT_DESC& tHit);

	_bool	Is_OnHit();
	Flags	Get_AttackFlag() const { return m_fAttackFlag; }
	Vec3	Get_HitNormal() const { return m_tPreHitDesc.vHitNormal; }
	Vec3	Get_VicPosition() const;

	void	Reset_HitDesc() { m_tPreHitDesc = {};  m_fAttackFlag = 0; }

	_bool	Is_AttackLanded();

	// Fkey 관련 funcs
public:
	void	Set_FKeyEvent(_uint iEvnet, _bool bOn);
	_bool	Can_FKeyEvent();
	_uint	Get_CurFKeyEvent() const { return m_tFKeyData.iKeyEvent; }

	// timer 관련 funcs
public:
	_bool Can_ChangeGunState();
	void  Set_GunTimerOn() { m_tGunCoolTimer.bCountTime = true; m_tGunCoolTimer.fTimeAcc = 0.f; }

	// pivot 관련 funcs
public:
	void		Set_PivotPos(Vec3 vPivotPos) { m_vPivotPos = vPivotPos; }
	const Vec3& Get_PivotPos() const { return m_vPivotPos; }

	void		Set_SpecialDashOn(_bool bOn) { m_bCanSpecialDash = bOn; }
	_bool		Get_SpecialDashOn() const { return m_bCanSpecialDash; }

public:
	void			Change_ActionBoneState(BONE_STATE eState, BONESTATE_CHANGE_ARGS* pArgs = nullptr);

private:
	BONE_STATE		m_eBoneState	= { BONE_STATE::END };

	Flags			m_fAttackFlag	= {};
	HIT_DESC		m_tPreHitDesc	= {};

	FKEY_DATA		m_tFKeyData		= {};

private:
	TIME_COUNTER	m_tGunCoolTimer = {};

	Vec3			m_vPivotPos = {};
	_bool			m_bCanSpecialDash = { false };

	BONEHIT_DATA	m_tBoneHit = {};

private:
	void Start_BoneState(BONE_STATE ePreState, BONESTATE_CHANGE_ARGS* tArgs);
	_bool End_BoneState(BONE_STATE eNextState);

	void Update_BoneState(const _float fTimeDelta);

	void Update_Normal(const _float fTimeDelta);
	void Update_HitStart(const _float fTimeDelta);
	void Update_HitEnd(const _float fTimeDelta);

	_uint Get_CurState_BoneHitFlag() const;

public:
	static CPlayerActionState* Create();
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};
NS_END