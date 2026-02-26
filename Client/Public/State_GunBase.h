#pragma once
#include "StateBase_Player.h"

/* 하체를 어떻게 할지 키를 받아서 여기서 처리한다 */

NS_BEGIN(Client)
class CGun;

class CState_GunBase abstract : public CStateBase_Player
{
	using Super = CStateBase_Player;

public:
	// 하체 move state
	enum class MoveState { GROUND, LAND, JUMP, FALL};
	enum  Douwn_MixAnim : _uint { F =0, B, L, R, LF, LB, RF, RB, JUMP, FALL, END };

	enum KeyFlag : Flags
	{
		W = 0x00001
		,S = 0x00002
		,A = 0x00004
		, D = 0x00008
		,Space = 0x00010
	};

	enum KeyMask : Flags
	{
		MoveKeyOn = KeyFlag::W | KeyFlag::S | KeyFlag::A | KeyFlag::D,

		Mask_Jump = KeyFlag::Space,

		Mask_F = KeyFlag::W,
		Mask_B = KeyFlag::S,
		Mask_L = KeyFlag::A,
		Mask_R = KeyFlag::D,
		Mask_LF = KeyFlag::W | KeyFlag::A,
		Mask_LB = KeyFlag::S | KeyFlag::A,
		Mask_RF = KeyFlag::W | KeyFlag::A,
		Mask_RB = KeyFlag::S | KeyFlag::A,
	};

	typedef struct tagGunStateDesc : public CStateBase_Player::PLAYER_STATEBASE_DESC
	{
		Vec4 vWSAD_AnimIdx = {-1.f,-1.f ,-1.f ,-1.f };
		Vec4 vLFB_RFB_AnimIdx = { -1.f,-1.f ,-1.f ,-1.f };
	}GUN_STATEBASE_DESC;

protected:
	CState_GunBase(CActionState* pOwnerComponent, const string& strName);
	virtual ~CState_GunBase() = default;

	virtual HRESULT Initialize(void* pArg) override;
public:
	virtual HRESULT Awake(const _uint iLevelIndex) override;
	virtual HRESULT Start(void* pArg, _bool bForce = false) override;
	virtual void	Update(const _float fTimeDelta) override;
	virtual HRESULT End() override;

protected:
	MoveState	m_eMoveState = { MoveState::GROUND };
	Flags		m_FKeyFlags = {};

	array<_uint, ENUM_TO_SZET(Douwn_MixAnim::END)> m_MixAnim_Indices;

	TimeCount m_TJumpTime = { 0.f, 2.f };
	TimeCount m_TLandTime = { 0.f, 1.f };


protected:
	void Check_KeyFlag(const _float fTimeDelta);
	_bool Check_BaseKey(const _float fTimeDelta);

	void Move_Update(const _float fTimeDelta);

	void Ground_Update(const _float fTimeDelta);
	void Jump_Update(const _float fTimeDelta);
	void Fall_Update(const _float fTimeDelta);
	void Land_Update(const _float fTimeDelta);

	_bool Change_MoveState(MoveState eState);
	void Start_MoveState(MoveState eNextState);
	void End_MoveState(MoveState ePreState);

	void GunEnd();

public:
	virtual void Free() override;
};

NS_END