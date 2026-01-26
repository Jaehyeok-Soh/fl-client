#pragma once
#include "Base.h"

// Triangle 법선이 곧 내 up

NS_BEGIN(Engine)

class CActionState;
class CNavigation;
class CGameObject;

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
		, SA_PreNonEvent	= 0x0002	// pre ani때 상태 변환 유무
		, SA_PreAniDone		= 0x0004	// 이전 ani가 끝났니
	};

	// 먼저 실행할 animation 정보
	typedef struct tagPreAnimationDesc
	{
		_int iPrevStateIdx	= {-1};
		_int iAnimationIdex = {-1};
	}PRE_ANIMATION;

	// Initial desc
	typedef struct tagStateBaseDesc
	{
		Flags			FAniFlags		= { 0 };
		vector<PRE_ANIMATION>	vecPreAnims;

		_bool bBlend = { false };
		_bool bLoop = { false };
		_int					iAnimIndex		= -1;
	}STATE_DESC;

	// change state 할때 들어오는 desc
	typedef struct tagStateBaseStartDesc
	{
		_float	fForceAbs		= { 0.f };
		_float	fDragK			= { 0.f };
	}STATE_START_DESC;

protected:
	CStateBase(CActionState* pOwnerComponent, const string &strName);
	virtual ~CStateBase() = default;

	virtual HRESULT Initialize(void* pArg);
public:
	// State를 가진 Object가 Scene에서 첫 루프가 시작 될때 최초 호출되는 함수
	virtual HRESULT Awake(const _uint iLevelIndex) PURE;
	virtual HRESULT Start(void *pArg, _bool bFroce = false) PURE;
	virtual void Update(const _float fTimeDelta) PURE;
	virtual HRESULT End() PURE;
	const _char* Get_Name() const { return m_strName.c_str(); }
	virtual _uint Get_Capabilities() const
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

protected:
	HRESULT Request_ChangeAnimation(_uint iAnimationIndex, _bool bBlend, _bool bLoop, _bool bForce = false);
	HRESULT Request_Change_State(_uint iIndex, void *pArg = nullptr);
	CGameObject* Get_OwnerObject();
	_float Get_AnimElpasedTimeSeconds();
	_float Get_AnimNormalizedTime();
	_bool Is_AnimFinished();
	_bool Is_MainAnimFinished();
	_bool Is_AnimTrackPositionAt(_float fRatio);
	_bool Is_AnimTrackPositionBetween(_float fStartRatio, _float EndRatio);
	_bool Is_AnimTrackPositionHalf();

protected:
	_bool Align_Movement(const _float fTimeDelta);
	_bool Align_Move(_uint iRunState);

	void Follow_CameraLook(const _float fTimeDelta);
	void Apply_Gravity(const _float fTimeDelta);
	void SetupLook_CameraLook();
	void SetupLookAt(const Vec3 &vPoint);
	void SetupLook_Target_XZ();
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

	_bool Key_Input(_uint iKey);
private:
	
private:
	_bool IsBlend() { return m_bBlend; }
	_bool IsLoop() { return m_bLoop; }
private:
	class CGameInstance* m_pGameInstance	= { nullptr };
	CActionState*		m_pOwnerStateComp	= { nullptr };
protected:
	_float	m_fStateElapsed = { 0.f };
	_bool	m_bBlend		= { false };
	_bool	m_bLoop			= { false };
	_bool	m_bMainForce	= { false };
	_int	m_iAnimIndex	= { -1 };
	string	m_strName		= { "" };

	Flags					m_FAniFlags		= { 0 };
	vector<PRE_ANIMATION>	m_vecPreAnims;

	_int m_iMixAni			= { -1 }; // todo : animation 섞는거 어떻게 할지...

public:
	virtual void Free() override;
};
NS_END
