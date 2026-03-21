#pragma once
#include "GameObject.h"
#include "Camera.h"

NS_BEGIN(Engine)


struct CinematicCameraSequence;

enum class CameraType
{
	STATIC,
	DYNAMIC,
	END
};


class ENGINE_DLL CCameraMan abstract : public CGameObject
{
	using Super = CGameObject;
public:

protected:
	explicit CCameraMan(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, CameraType eType);
	explicit CCameraMan(const CCameraMan& rhs);
	virtual ~CCameraMan() = default;

	virtual HRESULT Initialize_Prototype() PURE;
	virtual HRESULT Initialize(void* pArg) PURE;
	HRESULT			Ready_Components(void *pArg);
public:
	virtual HRESULT Awake(const _uint iCurrentLevelID) override;
	virtual void	Update_Priority(const _float fTimeDelta) override;
	virtual void	Update(const _float fTimeDelta) override;
	virtual void	Update_Late(const _float fTimeDelta) override;
	virtual void	Ready_Before_Render(const _float fTimeDelta) override;

	CameraType		Get_Type() const { return m_eType; }
	CGameObject*	Get_Actor() { return m_pActor; }
	inline void		Change_Actor(CGameObject* pGo);

	void			Cinematic(CinematicCameraSequence* pCameraCinematicSequence);
	void			Camera_Shaking(const CAM_SHAKING_DATA& tData);

	const Vec3&		Get_CameraShakingOffsetPos() const { return m_vCamShakingOffsetPosition; }
private:
	void			Cinematic(const _float fTimeDelta);
	void			Camera_Shaking(const _float fTimeDelta);
protected:
	CameraType		m_eType = { CameraType::STATIC };
	CGameObject*	m_pActor = { nullptr };
protected:
	_bool								m_isCinematicEvent{false};
	CinematicCameraSequence*			m_pCinematicSquence{nullptr};
	/* Cinematic Sequence 관련 데이터 처리용 변수 */
	_uint								m_iCurFrameIndex{ 0 };	/* 현재 KeyFrame Index */
	_float								m_fDeltaTime{ 0.f };		/* 현재 누적된 시간 */
protected:
	list<CAM_SHAKING_DATA>				m_listCameraShakingDatas{};
	Vec3								m_vCamShakingOffsetPosition{Vec3::Zero};
public:
	virtual CGameObject*				Clone(void *pArg) PURE;
	virtual void						Free() override;
};

NS_END