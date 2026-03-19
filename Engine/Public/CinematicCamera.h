#pragma once
#include "CameraMan.h"
#include "CinematicCameraSequence.h"

NS_BEGIN(Engine)

struct CinematicCameraSequence;

class ENGINE_DLL CCinematicCamera final: public CCameraMan
{
	using Super = CCameraMan;
public:
	typedef struct tagCinematicCamer_Desc : public CCameraMan::GAMEOBJECT_DESC
	{

	}CINEMATICCAMER_DESC;
protected:
	explicit CCinematicCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CCinematicCamera(const CCinematicCamera& rhs);
	virtual ~CCinematicCamera() = default;
protected:
	HRESULT								Initialize_Prototype()override;
	HRESULT								Initialize(void* pArg)override;
public:
	virtual HRESULT						Awake(const _uint iCurrentLevelID) override;
	virtual void						Update_Priority(const _float fTimeDelta) override;
	virtual void						Update(const _float fTimeDelta) override;
	virtual void						Update_Late(const _float fTimeDelta) override;
	virtual void						Ready_Before_Render(const _float fTimeDelta) override;
	virtual HRESULT						Render()override;
public:
	void								Update_PreMove(const _float fTimeDelta);
public:
	HRESULT								Play_Cinematic(CinematicCameraSequence* pCamCinematicSequence);
	HRESULT								End_Cinematic();
protected:
	_bool								m_isPreCamData_AddKeyFrame;
	_bool								m_isActionStart;
	_bool								m_isDepratEvent;
	_bool								m_isOnReachEvent;
	CinematicCameraSequence*			m_pCinematicSquence;	/* 현재 CinematicSquence */
	_int								m_iCurFrameIndex;		/* 현재 KeyFrame Index */
	_int								m_iPreFrameIndex;		/* 이전 KeyFrame Index */
	_float								m_fDeltaTime;			/* 현재 누적된 시간 */
private:
	vector<Camera_Keyframe_Data>		m_vecCamer_KeyFrame_Data;
	Camera_Keyframe_Data				m_tEntryPointData;		/* 카메라 시네마틱 연출 시작 지점  */
public:
	static	CCinematicCamera*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*				Clone(void* pArg) override;
	virtual void						Free() override;
};

NS_END