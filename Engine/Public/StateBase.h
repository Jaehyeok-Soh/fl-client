#pragma once
#include "Base.h"

// Triangle 법선이 곧 내 up

NS_BEGIN(Engine)

class CActionState;
class CNavigation;
class CGameObject;
class CTransform;

#pragma region Capability
enum class StateCapability
{
	NONE = 0,
	MOVE = 1 << 0,
	ROTATE = 1 << 1,
	JUMP = 1 << 2,
	SKILL = 1 << 3,
	ATTACK = 1 << 4,
	BEATTACKED = 1 << 5,
};

inline constexpr _uint ToMask(StateCapability eFlag)
{
	return static_cast<_uint>(eFlag);
}

inline constexpr _uint operator | (StateCapability a, StateCapability b)
{
	return ToMask(a) | ToMask(b);
}

inline constexpr _uint operator | (_uint iMask, StateCapability a)
{
	return iMask | ToMask(a);
}

inline constexpr _bool Has_Capability(_uint iMask, StateCapability eFlag)
{
	return (iMask & ToMask(eFlag)) != 0;
}
#pragma endregion


class ENGINE_DLL CStateBase abstract : public CBase
{
	using Super = CBase;
public:
	// state 안에서 쓰는 animation과 관련한 불 값들
	enum STATEANI_FLAG : Flags
	{
		SA_HasPreAni		= 0x0001	// pre ani가 있니
		, SA_PreNonEvent	= 0x0002	// pre ani때 상태 변환 유무 flag.
		, SA_PreAniDone		= 0x0004	// 이전 ani가 끝났니
		, SA_WeaponAni		= 0x0008	// Weapon ani가 있니
	};

	// 먼저 실행할 animation 정보
	typedef struct tagPreAnimationDesc
	{
		_int iPrevStateIdx	= {-1};		// 이전 state가 이거이면
		_int iAnimationIdex = {-1};		// 이 animation을 재생

		// 만약 prevIdx가 -1이면 무조건 실행
	}CHECK_ANIMATION;

	// Initial desc
	typedef struct tagStateBaseDesc
	{
		Flags					FAniFlags		= { 0 };	//STATEANI_FLAG 이용할것.
		vector<CHECK_ANIMATION>	vecPreAnims;				// 프리 애니메이션들 저장
		vector<_int>			vecMainAnims;				// 메인 애니메이션들 저장

		vector<_int>			vecWeaponAnims; // weapon anims

		_bool bBlend = { false };
		_bool bLoop = { false };
	}STATE_DESC;

	// change state 할때 들어오는 desc
	typedef struct tagStateBaseStartDesc
	{
		_float	fForceAbs		= { 0.f };
		_float	fDragK			= { 0.f };
		_uint	iMainAnimIdx	= { 0 };					// main animation 중 어떤거 쓸건데?
		_bool	bCheckPre		= { true };
	}STATE_START_DESC;

protected:
	CStateBase(CActionState* pOwnerComponent, const string &strName);
	virtual ~CStateBase() = default;

	virtual HRESULT Initialize(void* pArg);
public:
	// State를 가진 Object가 Scene에서 첫 루프가 시작 될때 최초 호출되는 함수
	virtual HRESULT Awake(const _uint iLevelIndex) PURE;
	virtual HRESULT Start(void *pArg, _bool bFroce = false) PURE;
	virtual void	Update(const _float fTimeDelta) PURE;
	virtual HRESULT End() PURE;
	const _char*	Get_Name() const { return m_strName.c_str(); }
	virtual _uint	Get_Capabilities() const
	{
		return	StateCapability::MOVE
			| StateCapability::ROTATE
			| StateCapability::JUMP
			| StateCapability::SKILL
			| StateCapability::ATTACK
			| StateCapability::BEATTACKED;
	}
	_bool Can_Move() const { return Has_Capability(Get_Capabilities(), StateCapability::MOVE); }
	_bool Can_Rotate() const { return Has_Capability(Get_Capabilities(), StateCapability::ROTATE); }
	_bool Can_UseSkill() const { return Has_Capability(Get_Capabilities(), StateCapability::SKILL); }
	_bool Can_Attack() const { return Has_Capability(Get_Capabilities(), StateCapability::ATTACK); }
	_bool Can_BeAttacked() const { return Has_Capability(Get_Capabilities(), StateCapability::BEATTACKED); }
	_float Get_StateElapsedTime() const { return m_fStateElapsed; }
	virtual _bool Is_FinishedState() { return Is_AnimFinished(); }
	_uint Get_MainAnimIdx() const { return m_iMainAnimIdx; }

	/* animation funcs */
protected:
	HRESULT Request_MixAnimation(_uint iVectorIdx, _int iAnimIdx);
	HRESULT Request_ChangeAnimation(_uint iAnimationIndex, _bool bBlend, _bool bLoop, _bool bForce = false);
	HRESULT Request_Change_WeaponAnimation(_int iAnimationIndex, _bool bBlend, _bool bLoop, _bool bForce = false);
	HRESULT Request_Change_State(_uint iIndex, void *pArg = nullptr);
	HRESULT Request_Change_StateForce(_uint iIndex, void* pArg = nullptr);
	CGameObject* Get_OwnerObject();
	_float Get_AnimElpasedTimeSeconds();
	_float Get_AnimNormalizedTime();
	_bool Is_AnimFinished();
	_bool Is_MainAnimFinished();
	_bool Is_AnimTrackPositionAt(_float fRatio);
	_bool Is_AnimTrackPositionBetween(_float fStartRatio, _float EndRatio);
	_bool Is_AnimTrackPositionHalf();

	void			Additive_MixOn(_bool bOn);
	void			Additive_DataSetting(_bool bAdditive, _int iRefIdx, _int iPosIdx, _float fMixOffset = 1.f);
	void			Additive_DataSetting(_bool bAdditive, _int iPosIdx, _float fMixOffset = 1.f);

	/* move funcs */
protected:
	_bool Align_Movement(const _float fTimeDelta);
	_bool Align_Movement_MoveDir(const _float fTimeDelta);
	_bool Align_Move(_uint iRunState, _bool bForce = false, void* pArg = nullptr);
	void Move(Vec3 vAccelation);
	void SetCCTInputDirection(Vec3 vInputDir);
	void SetCCTImpuls(Vec3 vImpuls);
	void Set_ZeroVelocity();
	void Set_ZeroHorizontalVelocity();
	void Set_ZeroVerticalVelocity();
	void Set_ZeroDeAccelRate();
	void Set_DeAccelRate(_float fRate);
	void Reset_DeAccelRate();
	void Follow_CameraLook(const _float fTimeDelta);
	void Apply_Gravity(const _float fTimeDelta);
	void SetupLook_CameraLook();
	void SetupLook_CameraSameLook(); // camera look == owenr look
	void SetupLook_CameraLookLerp(const _float fTimeDelta, _float fLerpSpeed);
	void SetupLookAt(const Vec3 &vPoint);
	void SetupLook_Target_XZ();
	void SetupLook_PointLerp(const _float fTimeDelta, Vec3 vPoint, _float fLerpSpeed);
	_bool Align_Attack(_uint iState);

	_bool Is_Grounded() const;
	_bool Is_ApplyGravity() const;
	_bool Is_AttackPressed() const;
	void Chase_Target(Vec3 vTargetPosition, const _float fTimedelta, const _float fSpeedRatio = 1.f);
	void Move_Up(const _float fTimeDelta, const _float fSpeedRatio);
	void Move_Left(const _float fTimeDelta, const _float fSpeedRatio = 1.f);
	void Move_Right(const _float fTimeDelta, const _float fSpeedRatio = 1.f);
	void Move_Front(const _float fTimeDelta, const _float fSpeedRatio = 1.f);
	void Move_Backward(const _float fTimeDelta, const _float fSpeedRatio = 1.f);
	void Move_Down(const _float fTimeDelta, const _float fSpeedRatio = 1.f);
	void StartForce_Front_ForAnimation(_float fForceAbs, _float fDragK);
	void StartForce_Backward_ForAnimation(_float fForceAbs, _float fDragK);
	void StartForce_Left_ForAnimation(_float fForceAbs, _float fDragK);
	void StartForce_Right_ForAnimation(_float fForceAbs, _float fDragK);
	void Set_AttackCollider(_uint iPartIndex, _bool bActive, ATTACK_DESC* pDesc);
	void Apply_ForceMove(const _float fTimeDelta);
	void Clear_ForceMove();
	_int Get_PrevState() const;
	CGameObject* Get_Target();
	void Set_AnimationPlayRate(_float fSpeed);
	void Set_JumpCount(_uint iCount);

	void Turn_byCam(const _float fTimeDelta);

	/* key funcs */
protected:
	_bool Key_Input(_uint iKey);
	
	/* getter setter funcs*/
protected:
	_bool			IsOn_CCTFlag(PxControllerCollisionFlag::Enum eFlag);
	void			Set_ApplyGravity(_bool bApply);
	void			Set_ApplyYLerp(_bool bApply);
	void			Set_GravityOffset(_float fOffset);
	void			Set_YLerp(_bool bLerp);

protected:
	_bool IsBlend() { return m_bBlend; }
	_bool IsLoop() { return m_bLoop; }

protected:
	CTransform* Get_CamTransform();
	Vec3 Get_MoveDir();


protected:
	class CGameInstance* m_pGameInstance	= { nullptr };
	CActionState*		m_pOwnerStateComp	= { nullptr };
protected:
	_float	m_fStateElapsed = { 0.f };
	_bool	m_bBlend		= { false };
	_bool	m_bLoop			= { false };
	_bool	m_bMainForce	= { false };

	string	m_strName		= { "" };

	Flags					m_FAniFlags		= { 0 };
	vector<CHECK_ANIMATION>	m_vecPreAnims;
	vector<_int>			m_vecMainAnims;
	vector<_int>			m_vecWeaponAnims; // pre + main 개수만큼 1대 1로 들고 있는다. 재생 안 할거면 -1로 넣기
	_uint					m_iMainAnimIdx = { 0 }; // mainAnimIdx

	_int m_iMixAni			= { -1 }; // todo : animation 섞는거 어떻게 할지...

public:
	virtual void Free() override;
};
NS_END
