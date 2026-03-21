#pragma once
#include "StateBase.h"

// player의 state들이 공통적으로 처리해야하는 것들을 다루기 위해
// 부모 statebase를 하나 생성

#define ANIMTIC (24.f * 1.2f)

NS_BEGIN(Client)
class CGun;

class CStateBase_Player abstract : public CStateBase
{
	using Super = CStateBase;

public:
	// wasd가 눌렸을때 어떻게 할건지
	enum MOVEFLAGS : Flags
	{
			NORMAL			= 0x0001 // 8방향 움직임
		,	PRESS_CHANGE	= 0x0002 // state change : press
		,	UP_CHANGE		= 0x0004 // state change : up
		,	OWN				= 0x0008 // 자신만의 움직임
		,	LOOP_DONE		= 0x0010 // loop done일때 만약 키가 눌렸다면
	};

	enum COLLISIONFLAGS : Flags
	{
		C_DOWN		= 0x00001 // 바닥 충돌 시간 누적후 -> falll
		, C_WALL_NO = 0x00002

		// hit state change 할래 말래
		,C_Addtive	= 0x00008
		,C_Fly		= 0x00010
		,C_Strong	= 0x00020

		,C_CheckF = 0x00040
	};

	enum WEAPONCHANGEFLAGS : Flags
	{
		None = 0

		// change check를 할거니
		,	Change_Check		= 0x00001

		//  change timming
		, Change_End			= 0x00010	// state 끝나고 change
		, Change_NextFrame		= 0x00020	// 다음 프레임에 change

		// 어떤걸 change 할거니 (melee -> gun x, melee 1 -> melee 2)
		,	Change_Melee		= 0x00002
		,	Change_Gun			= 0x00004
		,	Change_Skill		= 0x00008

		// masks
		,	Mask_ChangeWeapons = Change_Melee | Change_Gun | Change_Skill
	};

	typedef struct tagHitStartDesc : public CStateBase::STATE_START_DESC
	{
		Vec3 vHitDir = {};
		Vec3 vVicPos = {};
	}HITSTATE_START_DESC;

	enum class STATEKEY : _uint {MOVE, SPACE, SHIFT, LCRTL_PRESS, LCRTL_UP, E,Q, LM, RM, CHARGE, LOOPDONE ,LOOPDONEMOVEKEY, END}; //END에는 키가 없을떄 바꿀 state를 넣자

	// 최소 desc
	typedef struct tagBaseDesc : public CStateBase::STATE_DESC
	{
		CGun* pOwnerGun = { nullptr };
		Flags FCollis = { 0 };
	}PLAYER_STATE_SPECIFICDESC;

	typedef struct tagPlayerStateDesc : public CStateBase::STATE_DESC
	{
		Flags					FMoves		= { 0 }; // MOVEFLAGS 이용
		Flags					FCollis		= { 0 }; // COLLISIONFLAGS 이용
		Flags					FWeaponChanges = { 0 }; // WEAPONCHANGEFLAGS 이용
		vector<_uint>			vecChangeState_ByKey;			// 키 입력에 따라 어떻게 바꿀지 담는 벡터

		TIME_COUNTER			tKeyTimer = {};

		CGun* pOwnerGun = { nullptr };
	}PLAYER_STATEBASE_DESC;

protected:
	CStateBase_Player(CActionState* pOwnerComponent, const string& strName);
	virtual ~CStateBase_Player() = default;

	virtual HRESULT Initialize(void* pArg) override;
public:
	virtual HRESULT Awake(const _uint iLevelIndex) override;
	virtual HRESULT Start(void* pArg, _bool bForce = false) override;
	virtual void	Update(const _float fTimeDelta) override;
	virtual HRESULT End() override;

	// 기본으로는 BEATTACKED 가능. BEATTACKED 불가능한 state에서는 override 필요
public:
	virtual _uint	Get_Capabilities() const override
	{
		return	ENUM_TO_UINT(Engine::StateCapability::BEATTACKED);
	}

public:
	virtual void Change_PlayerState(STATEKEY eKey, _bool bForce = false);	// change 랩핑 함수 : 필요시 오버라이드
	virtual void Change_PlayerState(_uint iState, _bool bForce = false);	// change 함수2 : 키 없이 state 기준으로
	virtual void Change_PlayerHitState(_uint iState, void* pArg = nullptr);
	
protected:
	_uint					m_iEndStateIdx		= { 0 };			// CPlayer::State::END 캐싱 해둠 : 만약 END면 state change x

	Flags					m_FMoves			= { 0 };
	Flags					m_FCollisions		= { 0 };
	Flags					m_FWeaponChanges	= { 0 };

	vector<_uint>			m_vecChangeState_ByKey;

	TIME_COUNTER			m_tKeyTimer			= {};

	STATE_START_DESC		m_tNextStateDesc	= {};

	TimeCount				m_TFallingCount		= { 0.f,0.38f }; // 예전 값  : 0.4f
	TimeCount				m_TChargeCount		= { 0.f,0.18f }; // 예전 값  : 0.15f

	_float					m_fMoveTimeOffset = { 1.f };

	_bool					m_bLookAtMonster = { false };
	Vec3					m_vMonsterPos = { Vec3::Zero };

	// state가 변환 했다면 true
protected:
	_bool	Check_Keys(const _float fTimeDelta);

	_bool	Check_MoveKey(const _float fTimeDelta);
	_bool	Check_JumpKey(const _float fTimeDelta);
	_bool	Check_DashKey(const _float fTimeDelta);
	_bool	Check_CtrlPressKey(const _float fTimeDelta);
	_bool	Check_CtrlUpKey(const _float fTimeDelta);
	_bool	Check_MeleeKey(const _float fTimeDelta);
	_bool	Check_RangeKey(const _float fTimeDelta);
	_bool	Check_SkillKey(const _float fTimeDelta);

	_bool	Check_FKey(const _float fTimeDelta);

	_bool	Check_WeaponChnage(const _float fTimeDelta);
	_bool	Check_Hit(const _float fTimeDelta);
	_bool	Check_Collis(const _float fTimeDelta);

	// 움직임 관련 함수
protected:
	void	Jump_Impuls(_float fOffset = 1.f);
	void	Look_Impuls(_float fOffset = 1.f);

	void	LookAt_Monser();

	// player 객체 연결 함수들
protected:
	_bool	Check_OnGround(_float fMaxDist = 0.6f); // 땅에 있는지 검사 0.8f

	_bool	Start_Att(_uint iPlayerState);
	void	End_Att(_uint iPlayerState);

	void	Set_RootMotion_Apply(_bool bApply);

	void	Set_DoubleJumpCount(_bool bCount);
	_bool	Check_Double();

	// gun util funcs
protected:
	_bool	Can_Fire();
	_bool	Can_Reload();
	void	Set_GunTimer(_bool bOn);
	void	Reset_GunTimer();
	void	Reload_Gun();

	// player weapon util funcs
protected:
	void	Change_WeaponState(_uint iPartWeapon, _uint iState);
	_bool	Change_Weapon();

	_int	Get_WeaponIdx(_uint iWeaponType);
	_bool	Can_UseWeapon(_uint iWeaponType);

protected:
	virtual _bool Change_State_WhenLoopDone(const _float fTimeDelta);

	virtual void OwnMove(const _float fTimeDelta) {};		// state 내부에서 알아서 움직일때
	virtual void Set_NextStateDesc(_uint iNextState);		// 다음 state에 따라 desc을 작성한다 : 각 state 내부에서
	virtual void CheckAni_WhenStart() {};					// 만약 자체에서 로직을 통해 바꾸고 싶다면

	virtual _bool Can_CheckKey(const _float fTimeDelta);

	virtual void Reset_WhenStart();

protected:
	HRESULT Start_AttackState(void* pArg);

private:
	CGun*					m_pOwnerGun = { nullptr };

private:
	_bool	Has_ChangeState(STATEKEY eKey);
	_bool	Can_ChangeNextWeapon(_uint iWeaponType);

	void	Count_Combo();

public:
	virtual void Free() override;
};

NS_END