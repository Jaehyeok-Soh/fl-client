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
	NORMAL = 0,
	TARGETSYNC,
	GUN,
	CINEMATIC,
	Bone
};

class CCameraMan_Targeter final : public CCameraMan
{
	using Super = CCameraMan;

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

	//void GunCam_Begin();
	//void GunCam_Update_Priority(const _float fTimeDelta);
	//void GunCam_Update(const _float fTimeDelta);
	//void GunCam_End();

private:
	void Update_Input(const _float fTimeDelta);
	void Chase_Actor(const _float fTimeDelta);
	void Chase_Player(CContainerObject* pObject, const _float fTimeDelta);
	void OnChangeLockonTarget(CGameObject* pGo);
	Vec3 Get_CamBoneWorldPos_FromBody(CBody* pBody, CTransform* pTrnasform);

	_bool Change_Distance(_float fTargetDistance, const _float fTimeDelta);

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
	
	_float m_fCurDistance		= { 3.f };

	_float m_fNormalDistance	= { 3.f };
	_float m_fGunDistance		= { 1.f };
	_float m_fDistanceSpeed		= { 5.f };
	MinMax m_MGun_RightDistance = { 0.f,1.f };

	Vec3 m_vTargetPos = Vec3::Zero;

	_bool m_bChangeFirst = { true }; // 초기 change 값


	TargeterState m_ePreState = { TargeterState::NORMAL };

public:
	static CCameraMan_Targeter* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END