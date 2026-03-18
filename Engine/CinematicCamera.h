#pragma once
#include "CameraMan.h"

class CCinematicCamera final: public CCameraMan
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
	HRESULT								Play_Cinematic(Camera_Cinematic_Sequence* pCamCinematicSequence);
protected:
	Camera_Cinematic_Sequence*			m_pCinematicSquence;	/* 현재 CinematicSquence */
	_uint								m_iCurFrameIndex;		/* 현재 KeyFrame Index */
	_float								m_fDeltaTime;			/* 현재 누적된 시간 */
public:
	static	CCinematicCamera*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*				Clone(void* pArg) override;
	virtual void						Free() override;
};

