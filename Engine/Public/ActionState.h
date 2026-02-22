#pragma once
#include "Component.h"
#include "StateBase.h"
#include "ControlContext.h"

NS_BEGIN(Engine)

class CModel;
class CTransform;
class CNavigation;
class CCameraMan;
class CComputeShader;

class ENGINE_DLL CActionState abstract : public CComponent
{
	using Super = CComponent;
public:
	constexpr static EComponentType _ID = EComponentType::ACTIONSTATE;
	typedef struct tagActionStateDesc
	{
		_uint			iStateCount = { 0 };
		CModel*			pOwnerModel		= { nullptr };

		// state가 달린 객체들은 animation change를 state 내부에서 하기 때문에 
		// 꼭 값 넣어줘야 함
		CComputeShader* pOwnerAnimECS		= { nullptr }; 
	}ACTIONSTATE_DESC;

protected:
	CActionState();
	CActionState(const CActionState& rhs);
	virtual ~CActionState() = default;

	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
public:
	HRESULT			Awake(_uint iLvelIndex);
	void			Update(const _float fTimeDelta);

	/* state funcs */
public:
	HRESULT			Change_State(_uint iIndex, _bool bForce = false, void* pArg = nullptr);
	HRESULT			Add_State(_uint iIndex, CStateBase *pState);
	HRESULT			Remove_State(_uint iIndex);
	HRESULT			Swap_State(_uint iIndex, CStateBase *pState, CStateBase** ppSwappedState);
	CStateBase*		Get_State(_uint iIndex);

	/* getter setter funcs */
public:
	_bool			IsInState(_uint iIndex) const { return m_iCurrentState == iIndex; }
	_bool			IsValidStateIndex(_uint iIndex) const { return iIndex < m_vecStates.size() && m_vecStates[iIndex] != nullptr; }
	_bool			IsOn_CCTFlag(PxControllerCollisionFlag::Enum eFlag);

	_int			Get_PrevState() const	{ return m_iPrevState;  }
	_int			Get_CurrentStateIndex() { return m_iCurrentState; }
	CStateBase*		Get_CurrentState();
	_uint			Get_CurrentCapabilities() { return Get_CurrentState()->Get_Capabilities(); }
	const char*		Get_CurrentStateName() const;

	void			Set_Navigation(CNavigation* pNavigation);
	void			Set_ApplyGravity(_bool bApply) { m_bApplyGravity = bApply; }
	void			Set_ApplyYLerp(_bool bApply) { m_bApplyYLerp = bApply; }

	void			Set_GravityOffset(_float fOffset) { if (fOffset > m_fGravity)return; m_fGravityOffset = fOffset; }

	/* animation funcs*/
protected:
	HRESULT			Request_ChangeAnimation(_uint iAnimationIndex, _bool bBlend, _bool bLoop, _bool bForce = false);
	_float			Get_AnimElpasedTimeSeconds();
	_float			Get_AnimNormalizedTime();
	_bool			Is_AnimFinished();
	_bool			Is_AnimTrackPositionAt(_float fRatio);
	_bool			Is_AnimTrackPositionBetween(_float fStartRatio, _float EndRatio);
	_bool			Is_AnimTrackPositionHalf();
	void			Set_AnimationPlayRate(_uint iIndex, _float fSpeed);

	/* moves funcs */
protected:
	_bool			Align_Movement(const _float fTimeDelta);
	void			Follow_CameraLook(const _float fTimeDelta);
	_bool			Is_Grounded() const;
	_bool			Is_ApplyGravity() const;
	_bool			Is_AttackPressed() const;
	void			Chase_Target(const Vec3& vTargetPosition, const _float fTimedelta, const _float fSpeedRatio = 1.f);
	void			Move_Up(const _float fTimeDelta, const _float fSpeedRatio);
	void			Move_Left(const _float fTimeDelta, const _float fSpeedRatio = 1.f);
	void			Move_Right(const _float fTimeDelta, const _float fSpeedRatio = 1.f);
	void			Move_Front(const _float fTimeDelta, const _float fSpeedRatio = 1.f);
	void			Move_Backward(const _float fTimeDelta, const _float fSpeedRatio = 1.f);
	void			Move_Down(const _float fTimeDelta, const _float fSpeedRatio = 1.f);
	void			StartForce_Front_ForAnimation(_float fForceAbs, _float fDragK);
	void			StartForce_Backward_ForAnimation(_float fForceAbs, _float fDragK);
	void			StartForce_Left_ForAnimation(_float fForceAbs, _float fDragK);
	void			StartForce_Right_ForAnimation(_float fForceAbs, _float fDragK);
	void			Set_AttackCollider(_uint iPartIndex, _bool bActive, ATTACK_DESC* pDesc);
	void			Apply_Gravity(const _float fTimeDelta);
	void			Apply_Gravity_CCT(const _float fTimeDelta);
	void			Apply_ForceMove(const _float fTimeDelta);
	void			Clear_ForceMove();
	void			SetupLook_CameraLook();
	void			SetupLookAt(const Vec3& vPoint);
	void			SetupLook_Target_XZ();
	CGameObject*	Get_Target();
	_bool			Align_Move(_uint iState, _bool bForce = false, void* pArg = nullptr);
	_bool			Align_Attack(_uint iState);
	void			Turn_byCam(const _float fTimeDelta);

	void			Set_JumpCount(_uint iCount);

	/* key funcs */
protected:
	_bool			Key_Input(CControlContext::CONTROL_KEY eKey);

protected:
	HRESULT			Set_OwnerComponents();

protected:
	CControlContext*	m_pOwnerControlContext	= { nullptr };
	CTransform*			m_pOwnerTransform		= { nullptr };
	CNavigation*		m_pOwnerNavigation		= { nullptr };
	CModel*				m_pOwnerModel			= { nullptr };
	CCameraMan*			m_pOwnerTargetCamera	= { nullptr };
	CComputeShader*		m_pOwnerAnimECS			= { nullptr };

	_uint				m_iStateCount			= { 0 };
	_int				m_iCurrentState			= { -1 };
	_int				m_iPrevState			= { -1 };

	const _float		m_fGravity				= { -9.8f };
	const _float		m_fMaxVerticalSpeed		= { 30.f };
	const _float		m_fMaxFallSpeed			= { -30.f };
	_float				m_fVerticalSpeed		= { 0.f };

	_bool				m_bApplyGravity			= { true };
	_bool				m_bApplyYLerp			= { false };

	_float				m_fGravityOffset		= { 0.f };

	vector<CStateBase*> m_vecStates;

	// Physics
	PxControllerCollisionFlags CCTFlags{};
public:
	virtual CComponent* Clone(void* pArg) PURE;
	virtual void Free() override;
	friend CStateBase;
};

NS_END