#pragma once
#include "MonoBehaviour.h"
#include "UIAction_Registry.h"

NS_BEGIN(Tool)

class CToolUI;
class CUIAction_Player final : public CMonoBehaviour
{
	using Super = CMonoBehaviour;
public:
	typedef struct tagActionPlayerDesc : public MONO_DESC
	{
		CToolUI* pOwner;
	}ACTION_PLAYER_DESC;

	typedef struct tagMoveStateDesc
	{
		Vec3	vStartPos;
		Vec3	vTargetPos;
		_float	fDuration;
		_float	fAlpha;
	}MOVE_DESC;

	typedef struct tagProgressStateDesc
	{
		_float fStartUV;
		_float fTargetUV;
		_float fDuration;
		_float fAlpha;
	}PROGRESS_DESC;

private:
	CUIAction_Player();
	CUIAction_Player(const CUIAction_Player& rhs);
	virtual ~CUIAction_Player() = default;

	HRESULT Initialize_Prototype() override;
	HRESULT Initialize_Prototype(ACTION_PLAYER_DESC* pDesc);
	HRESULT Initialize(void* pArg) override;

public:
	void Update(const _float fTimeDelta) override;

#pragma region /////Lerp Movement///////////////////////////////////////////////////////////
public:
	void Start_Lerp_Movement(MOVE_DESC* pDesc);
	void Lerp_Movement(const _float fTimeDelta);
	void Apply_Lerp_Movement();
private:
	_bool	m_isPlaying_Lerp_Movement = { false };
	Vec3	m_vLerpMovement_StartPos = {};
	Vec3	m_vLerpMovement_TargetPos = {};
	_float	m_fLerpMovement_TargetAlpha = {};
	_float	m_fLerpMovement_Duration = {};
	_float	m_fLerpMovement_TimeAcc = {};
	_bool	m_isMoved = { false };
	Vec3	m_vMoveOffset = {};
#pragma endregion

#pragma region /////Progress Bar/////////////////////////////////////////////////////////
public:
	void Start_Progress(PROGRESS_DESC* pDesc);

#pragma endregion


private:
	CToolUI* m_pOwner = { nullptr };

public:
	static CUIAction_Player* Create(ACTION_PLAYER_DESC* pDesc);
	CComponent* Clone(void* pArg);
	virtual void Free() override;
};

NS_END
