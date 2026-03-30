#pragma once
#include "ICameraAnchorHost.h"
#include "Client_Defines.h"
#include "ContainerObject.h"

NS_BEGIN(Engine)
class CNavigation;
class CStateBase;
NS_END

NS_BEGIN(Client)
class CStatComponent;
class CSkillComponent;
class CCameraMan_Targeter;
class CBody;
class CGun;

class CPlayer abstract : public CContainerObject
					   , public ICameraAnchorHost
{
	using Super = CContainerObject;
public:
	enum class PLAYER_TYPE { MOON, END };

	enum class EWEAPON { MELEE, RANGE, SKILL,CONDEMN, END }; // array 접근용 enum  값

	enum class MELEE { SWORD, DUAL, END };
	enum class RANGE { MACHINE, END };
	enum class SKILL { MOON, END };
	enum class CONDEMN { NORMAL, END };
	
	// 각 무기가 가지고 있어야 하는 정보들
	typedef struct tagWeaponInfo
	{
		_uint iPartStartIdx = {};			// player part중 몇번째 인지	: 초기 셋팅 값
		_uint iPartSize		= { 1 };		// part중 몇개를 쓰는지			: 초기 셋팅 값
		_bool bHave		= { false };		// 해당 무기를 얻었는지

		_uint iWeaponState	= {};			// 무기 state // weapon 안에 있는 스테이트
	}WEAPON_INFO;

	typedef struct tagPlayerDesc : public Super::GAMEOBJECT_DESC
	{
		wstring wstrBodyModelTag = { L"" };
		wstring wstrNavigationPrototypeTag = { L"" };
		_int iNavigationCellIndex = { -1 };
		Vec3 vSpawnPosition = {};

		PLAYER_TYPE ePlayerType = { PLAYER_TYPE::END };

	}PLAYER_DESC;

	enum Part : _uint
	{
		BODY = 0,

		// weapon
		SWORD,
		SKILL,
		GUN,
		
		// 추가 wepaon
		Dual_R,
		Dual_L,

		Condemn,

		EFFECT,
		DETECTCOLLIDER_UI, // 몬스터 감지용 collider
		DETECTCOLLIDER,

		CLOAK,

		DETECTCOLLIDER_INTERACT,

		END
	};

	enum Skill : _uint
	{
		MoonE = 0,
		MoonQ
	};

	enum class State : _uint
	{
		IDLE
		,WALK
		,CROUCH
		,CROUCHWALK
		,SLIDE

		,DASHBACK
		,DASHSKY

		,RUNSHORT
		,RUNLOOP

		,JUMP
		,JUMPDOUBLE
		,JUMPBULLET
		,JUMPBACK

		,FALL
		,LAND

		/* 근거리 공격 */
		, COMBO
		, COMBO_DUAL
		, JUMPATTSTART
		, JUMPATTEND
		, CHARGE

		/* 원거리 공격 */
		,GUNIDLE
		,GUNWALK
		,GUNATTACK
		,GUNRELOAD

		,SKILL1
		,SKILL2

		,JUMPWALL

		,HITADDTIVE
		,HITFLYSTART
		,HITFLYEND
		,HITSTRONG

		,CONDEMN
		,SPECIALDASH

		,NPCTALK

		,STUN_START
		,SPHIT_START

		,END
	};
protected:
	CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CPlayer(const CPlayer& rhs);
	virtual ~CPlayer() = default;

	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
public:
	virtual HRESULT		Awake(const _uint iCurrentLevelID) override;
	virtual void		Update_Priority(const _float fTimeDelta) override;
	virtual void		Update(const _float fTimeDelta) override;
	virtual void		Update_Late(const _float fTimeDelta) override;
	virtual void		Ready_Before_Render(const _float fTimeDelta) override;
	virtual HRESULT		Render() override;
public:
	virtual _int		Get_AnimationIndex(const wstring& wstrName) override;
	virtual _wstring	Get_AnimationName(_uint iAniIndex);

public:
	PLAYER_TYPE Get_PlayerType() const { return m_ePlayerType; }

public:
	HRESULT Change_IdleForce();

	// weapon 관련 함수
public:
	void	Set_WepaponOn(_uint iWeaponType, _uint iIdx, _bool bOn);						// weapon on off만 관리
	void	SetWepaponOn_SetState(_uint iWeaponType, _uint iIdx, _bool bOn, _uint iState);	// weapon on off & state 까지 setting
	
	// UI 주석 // iWeaponType = 근거린지 원거린지 
	_bool	Change_MainWeapon(_uint iWeaponType, _uint iIdx);								// 예시 : 근접무기를 sword -> dual로 바꾸고 싶을때
	
	// UI 주석 // iWeaponType = 근거린지 원거린지 
	_bool	Change_MainWeaponNext(_uint iWeaponType);										// 특정 무기 타입을 다음 인덱스의 weapon으로 바꾸고 싶을때. 안에서 자동으로 인덱스 관리중
	
	// UI 주석 // iWeaponType = iState
	void	Change_WeaponState(_uint iWeaponType, _uint iState);							// 어떤 weapon을 어떤 state로. 단 활성화 되었을때만 조절 가능
	
	_int	Get_CurWeaponIdx(_uint iWeaponType);											// 이 무기타입이 어떤 무기로 설정하고 있는가
	_bool	Can_UseWeapon(_uint iWeaponType);												// 이 무기타입을 지금 사용할 수 있는가

	_bool	Get_CanQuickSlotOpen() const { return m_bQuickSlotOpen; }						// 지금 quick slot open 할 수 있는지 -> player state쪽에서 정보 넘겨줌
	void	Set_CanQuickSlopOpen(_bool bOpen) { m_bQuickSlotOpen = bOpen; }

	_bool	Can_AttackWeapon(_uint iWeaponType);

	// state funcs
public:
	_bool	Check_OnGround(_float fMaxDist = 0.72f);
	_bool	Check_ColliWithMonster();
	void	Count_Combo();
	void	Count_Dash();
	void	Set_RootMotion_Apply(_bool bApply);

	_bool	Check_DoubleJump();
	void	Set_DoubleJumpCount(_bool bCount) { m_tDoubleJumpCount.bCountTime = bCount; if (!bCount) m_tDoubleJumpCount.fTimeAcc = 0.f; }

	void	Change_CamState(_uint iCamState);
	_float	Get_CamPitch()const;

	void				Set_FKeyEvent(_uint iEvenet, _bool bOn); // f event 활성화 onoff
	const Vec3&			Get_CollidedMonster_Position();

	HRESULT				Bind_PlayerInfo(class CShader* pShader = nullptr);
	SHADER_PLAYER_INFO* Get_PlayerInfo() { return &m_tCBPlayerInfo; }		/* 임시용 */
public:
	_bool				Start_Attack(State iState);
	void				End_Attack(State iState);

	State				Get_CurState();

	// Weapon Quick Slot
public:
	const array<WEAPON_INFO, ENUM_TO_SZET(MELEE::END)>& Get_MeleeInfo() const { return m_arrMeleeInfo; }
	array<WEAPON_INFO, ENUM_TO_SZET(MELEE::END)>& Get_MeleeInfo_Ref() { return m_arrMeleeInfo; }
	const array<WEAPON_INFO, ENUM_TO_SZET(RANGE::END)>& Get_RangeInfo() const { return m_arrRangeInfo; }
	array<WEAPON_INFO, ENUM_TO_SZET(RANGE::END)>& Get_RangeInfo_Ref() { return m_arrRangeInfo; }

public:
	// Camera Interface
	virtual ICameraAnchorProvider* Get_CameraAnchorProvider(_int iPartIndex = 0) override;
	virtual CTransform* Get_CameraAnchorOwnerTransform() override;
protected:
	CPhysics_QueryFilterCallback* m_pPhysic_QueryFilter = { nullptr };

protected:
	TIME_COUNTER		m_tDoubleJumpCount = {};
	PLAYER_TYPE			m_ePlayerType = { PLAYER_TYPE::END };
	_bool				m_bMainPlayer = { false };
	SHADER_PLAYER_INFO	m_tCBPlayerInfo{};

	_bool				m_bQuickSlotOpen = { false };

protected:
	array<_int, ENUM_TO_SZET(EWEAPON::END)>			m_arrWeaponEnum; // 각 무기 종류에서 어떤거를 들고 있는가. 만약 없다면 -1

	array<WEAPON_INFO, ENUM_TO_SZET(MELEE::END)>	m_arrMeleeInfo;
	array<WEAPON_INFO, ENUM_TO_SZET(RANGE::END)>	m_arrRangeInfo;
	array<WEAPON_INFO, ENUM_TO_SZET(RANGE::END)>	m_arrSkillInfo;
	array<WEAPON_INFO, ENUM_TO_SZET(CONDEMN::END)>	m_arrCondemnInfo;

private:
	HRESULT		Ready_WeaponInfo();
	
	HRESULT		Ready_PartObjects(PLAYER_DESC* pDesc);
	HRESULT		Ready_PartWeapon(PLAYER_DESC* pDesc);
	HRESULT		Ready_Components(PLAYER_DESC* pDesc);

	HRESULT		Ready_BaseStates();
	HRESULT		Ready_HitStates();

	HRESULT		Ready_PartCollider();
	HRESULT		Ready_Interact_PartCollider();

private:
	void		Set_CurPartWeapon_State(EWEAPON eWeaponType, _uint iState);

private:
	void		Count_DoubleJump(const _float fTimeDelta);

public:
	virtual CGameObject* Clone(void* pArg) PURE;
	virtual void Free() override;
};

NS_END