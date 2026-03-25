#pragma once
#include "CameraMan.h"


NS_BEGIN(Engine)
class CBone;
class CGameObject;
class CTransform;
class CContainerObject;
NS_END

NS_BEGIN(Client)
class CPlayer;
class CBody;

enum class TargeterState : _uint
{
	NORMAL = 0,		// player tarcking cam
	TARGETSYNC,
	GUN,			// tps cam
	SKILL_SEQUENCE,
	CINEMATIC,
	TURN,
	SCRIPTED_SHOT,
	END
};

class CCameraMan_Targeter final : public CCameraMan
{
	using Super = CCameraMan;

public:
	enum class DISTANCE_DATA { RIGHT, UP, LOOK , END };

	typedef struct tagTurnData
	{
		_float	fTurnHalfTime	= {};	// 처음 돌아갈 시간
		_float	fTurnHoldTime	= {};	// 다시 제자리로 돌아갈 시간

		_float	fDistance		= {};	// pivot 과의 거리

		Vec3	vPivot			= {};	// 회전 시킬 중심
		Vec3	vFirstLookDir	= {};   // begin에서 셋팅할 look 방향. pivot을 

		_float	fSpeed			= {};	// 움직임 속도

	}TURNCAM_DATA;

private:
	CCameraMan_Targeter(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CCameraMan_Targeter(const CCameraMan_Targeter& rhs);
	virtual ~CCameraMan_Targeter() = default;

	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
public:
	virtual HRESULT Awake(const _uint iCurrentLevelID) override;
	virtual void Update_Priority(const _float fTimeDelta) override;
	virtual void Update(const _float fTimeDelta) override;
	virtual void Update_Late(const _float fTimeDelta) override;
	virtual void Ready_Before_Render(const _float fTimeDelta) override;
	void Initialize_WhenChangeTarget(CGameObject* pTarget);
	void Change_CamState(TargeterState eState);
	void Change_CamState(_uint iState);

	HRESULT	Ready_GlobalEvent();
	HRESULT Request_PlayScriptedShot(const SCRIPTED_CAMERA_SHOT_DESC& shotDesc, const SCRIPTED_CAMERA_SHOT_BINDING_DESC& bindingDesc) override;
	HRESULT Request_StopScriptedShot() override;
	// getter setter
public:
	_float	Get_Pitch() const { return m_fPitch; }
	void	Set_TurnData(TURNCAM_DATA& tData) { m_tTurnData = tData; }
private:
	void Update_Priority_State(const _float fTimeDelta);
	void Update_State(const _float fTimeDelta);
	void State_Begin(TargeterState eState);
	void State_End(TargeterState eState);

	void Normal_Begin();
	void Normal_Update_Priority(const _float fTimeDelta);
	void Normal_Update(const _float fTimeDelta);
	void Normal_End();

	void TargetSync_Begin();
	void TargetSync_Update_Priority(const _float fTimeDelta);
	void TargetSync_Update(const _float fTimeDelta);
	void TargetSync_End();

	void GunCam_Begin();
	void GunCam_Update_Priority(const _float fTimeDelta);
	void GunCam_Update(const _float fTimeDelta);
	void GunCam_End();

	void Skill_SequeneCam_Begin();
	void Skill_SequeneCam_Update_Priority(const _float fTimeDelta);
	void Skill_SequeneCam_Update(const _float fTimeDelta);
	void Skill_SequeneCam_End();

	void TurnCam_Begin();
	void TurnCam_Update_Priority(const _float fTimeDelta);
	void TurnCam_Update(const _float fTimeDelta);
	void TurnCam_End();

	void ScriptedShot_Begin();
	void ScriptedShot_Update_Priority(const _float fTimeDelta);
	void ScriptedShot_Update(const _float fTimeDelta);
	void ScriptedShot_End();
private:
	void Update_Input(const _float fTimeDelta);
	void Chase_Actor(const _float fTimeDelta);
	void Chase_Player(CContainerObject* pObject, const _float fTimeDelta);
	void OnChangeLockonTarget(CGameObject* pGo);
	Vec3 Get_CamBoneWorldPos_FromBody(CBody* pBody, CTransform* pTrnasform);

	_bool Change_Distance(_float fTargetDistance,_float fPreDistance, _float& fCurDistanxe, const _float fTimeDelta);

	void Change_DistancesAll(const _float fTimeDelta);

	Vec3 CheckCameraCollision(Vec3 vCameraPos, Vec3 vTargetPos);

	// For. TurnState
	void Update_TurnOn(const _float fTimeDelta);
	void Update_TurnOff(const _float fTimeDelta);
	_float Eval_TurnYawDegree() const;

	// For. ScriptShot State
	// pivot과 lookat 캡쳐
	void Capture_ScriptedShotSnapshot();
	// camera interface로 등록해놨던 Object들 정보 세팅
	_bool Resolve_ScriptedShotAnchors(OUT CAMERA_ANCHOR_RESULT& outPivot, OUT CAMERA_ANCHOR_RESULT& outLookAt);
	// pivot basis를 기준으로 전체 샷의 오프셋을 계산
	void Resolve_ShotBasis(const Engine::CAMERA_ANCHOR_RESULT& pivotAnchor, OUT Vec3& outRight, OUT Vec3& outUp, OUT Vec3& outLook);
	// resolve된 pivot/lookat anchor를 받아서 anchor offset 채널에 반영
	// camera local xyz, orbit 반영하여 base pose를 생성하는 함수
	void Evaluate_ScriptedShotBasePose(_float fTime,
		const CAMERA_ANCHOR_RESULT& pivotAnchor,
		const CAMERA_ANCHOR_RESULT& lookAtAnchor,
		OUT CAMERA_POSE& outBasePose,
		OUT Vec3& outPivotWS);
	// controller additive
	void Evaluate_ScriptedControllerResult(_float fTime, CAMERA_MODIFIER_RESULT& outResult) const;
	// pose 적용 및 normal sync
	void Apply_CameraPose(const CAMERA_POSE& tPose);
	void Sync_NormalStateFromCurrentPose();
	// Pivot 및 LookAt 오브젝트 레퍼런스 관리
	void Retain_ScriptedShotBindingObjects();
	void Release_ScriptedShotBindingObjects();
private:
	TargeterState m_eCurrentState = { TargeterState::NORMAL };
	CGameObject* m_pLockonTarget = { nullptr };
	_float m_fStateTime = { 0.f };
	_float m_fTargetSpeed = { 0.f };

	_bool m_bChaseInit = { false };
	Vec3 m_vChaseFiltered = { 0.f, 0.f, 0.f };

	_bool m_bImpactInit = { false };
	const _float m_fImpactDuration = { 0.2f };

	_float m_fTau_Chase = { 0.12f };
	_float m_fTau_Pos = { 0.07f };
	_float m_fTau_Rotate = { 0.07f };

	_float m_fSens = { 0.2f };
	
	// 회전 보간용
	_float m_fYaw			= { 0.f };
	_float m_fYaw_Target	= { 0.f }; 
	_float m_fPitch			= { 0.f };
	_float m_fPitch_Target	= { 0.f };
	
	// 초기 값은 normal이랑 같도록 한다
	_float m_fCurRightDistance		= { 0.f }; // 이거 하고 있었슨.
	_float m_fCurLookDistance		= { 3.f };

	array<_float, ENUM_TO_SZET(DISTANCE_DATA::END)> m_arrCurDistances;
	array<_float, ENUM_TO_SZET(DISTANCE_DATA::END)> m_arrPreDistances;

	array<_float, ENUM_TO_SZET(DISTANCE_DATA::END)> m_arrNormalDistances;	// r : 0  , l : 3
	array<_float, ENUM_TO_SZET(DISTANCE_DATA::END)> m_arrGunDistances;		// r : 0.6, l : 1

	_float m_fMoveDistanceTime	= { 0.28f };


	_float m_fDistanceSpeed		= { 5.f };

	Vec3 m_vTargetPos = Vec3::Zero;

	_bool m_bChangeFirst = { true }; // 초기 change 값

	// For. Turn State
	TURNCAM_DATA	m_tTurnData = {};
	Vec3 m_vTurnBaseLookDir = Vec3::Backward;
	_float m_fTurnBaseDistance = 0.f;

	TargeterState	m_ePreState = { TargeterState::NORMAL };
	
private:
	// For. ScriptShot State
	SCRIPTED_CAMERA_SHOT_DESC         m_tScriptedShotDesc = {};
	SCRIPTED_CAMERA_SHOT_BINDING_DESC m_tScriptedShotBinding = {};
	SCRIPTED_CAMERA_SHOT_RUNTIME      m_tScriptedShotRuntime = {};

	// Update에서 계산해서 Ready_Before_Render에서 소비
	CAMERA_MODIFIER_RESULT m_tScriptedOverlayResult = {};
	_bool                  m_bUseScriptedOverlay = false;
	Vec3                   m_vLastScriptedPivotWS = Vec3::Zero;
#ifdef _DEBUG
public:
	void Debug_PlayScriptedShot(const SCRIPTED_CAMERA_SHOT_DESC& tDesc, const SCRIPTED_CAMERA_SHOT_BINDING_DESC& tBinding);
	void Debug_StopScriptedShot();
	void Debug_SetScriptedShotTime(_float fTime);
	void Debug_SetScriptedShotPause(_bool bPause);
	_bool Debug_IsScriptedShotPlaying() const { return m_tScriptedShotRuntime.bPlaying; }
	_float Debug_GetScriptedShotTime() const { return m_tScriptedShotRuntime.fElapsed; }
#endif
public:
	static CCameraMan_Targeter* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END