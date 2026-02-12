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

enum class TargeterState
{
	NORMAL = 0,
	TARGETSYNC,
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
	void Change_State(TargeterState eState);
private:
	void Update_Priority_State(const _float fDeltaTime);
	void Update_State(const _float fDeltaTime);
	void State_Begin(TargeterState eState);
	void State_End(TargeterState eState);

	void Normal_Begin();
	void Normal_Update_Priority(const _float fDeltaTime);
	void Normal_Update(const _float fDeltaTime);
	void Normal_End();

	void TargetSync_Begin();
	void TargetSync_Update_Priority(const _float fDeltaTime);
	void TargetSync_Update(const _float fDeltaTime);
	void TargetSync_End();
private:
	void Update_Input(const _float fTimeDelta);
	void Chase_Actor(const _float fTimeDelta);
	void Chase_Player(CContainerObject* pObject, const _float fTimeDelta);
	void OnChangeLockonTarget(CGameObject* pGo);
	Vec3 Get_CamBoneWorldPos_FromBody(CBody* pBody, CTransform* pTrnasform);
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
	
	_float m_fDistance = { 3.f };
public:
	static CCameraMan_Targeter* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END