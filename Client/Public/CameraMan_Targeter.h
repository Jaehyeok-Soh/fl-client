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
	END
};

class CCameraMan_Targeter final : public CCameraMan
{
	using Super = CCameraMan;

public:
	enum class DISTANCE_DATA { RIGHT, UP, LOOK , END };

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

public:
	_float Get_Pitch() const { return m_fPitch; }

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

private:
	void Update_Input(const _float fTimeDelta);
	void Chase_Actor(const _float fTimeDelta);
	void Chase_Player(CContainerObject* pObject, const _float fTimeDelta);
	void OnChangeLockonTarget(CGameObject* pGo);
	Vec3 Get_CamBoneWorldPos_FromBody(CBody* pBody, CTransform* pTrnasform);

	_bool Change_Distance(_float fTargetDistance,_float fPreDistance, _float& fCurDistanxe, const _float fTimeDelta);

	void Change_DistancesAll(const _float fTimeDelta);

	Vec3 CheckCameraCollision(Vec3 vCameraPos, Vec3 vTargetPos);

private:
	TargeterState m_eCurrentState = { TargeterState::NORMAL };
	CGameObject* m_pLockonTarget = { nullptr };
	_float m_fStateTime = { 0.f };
	_float m_fTargetSpeed = { 0.f };

	_bool m_bChaseInit = { false };
	Vec3 m_vChaseFiltered = { 0.f, 0.f, 0.f };

	_bool m_bImpactInit = { false };
	const _float m_fImpactDuration = { 0.14f };

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

	array<_float, ENUM_TO_SZET(DISTANCE_DATA::END)> m_arrNormalDistances; // r : 0  , l : 3
	array<_float, ENUM_TO_SZET(DISTANCE_DATA::END)> m_arrGunDistances; // r : 0.6, l : 1

	_float m_fMoveDistanceTime	= { 0.28f };


	_float m_fDistanceSpeed		= { 5.f };

	Vec3 m_vTargetPos = Vec3::Zero;

	_bool m_bChangeFirst = { true }; // 초기 change 값


	TargeterState m_ePreState = { TargeterState::NORMAL };

public:
	static CCameraMan_Targeter* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END