#pragma once
#include "TriggerBox.h"
class CTriggerBox_CinematicPlayer :  public CTriggerBox
{
	using Super = CTriggerBox;
public:
	typedef struct tagCTriggerBox_CinematicPlayer_Desc : public CTriggerBox::TRIGGERBOX_DESC
	{
		string strCinemaitcCameraSequnceName{ "" };
	}TRIGGERBOX_CINEMATICCAMERA_DESC;
protected:
	CTriggerBox_CinematicPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CTriggerBox_CinematicPlayer(const CTriggerBox_CinematicPlayer& rhs);
	virtual ~CTriggerBox_CinematicPlayer() = default;
public:
	virtual HRESULT			Initialize_Prototype()							override;
	virtual HRESULT			Initialize(void* pArg)							override;
public:
	virtual HRESULT			Awake(const _uint iCurrentLevelID)				override;
	virtual void			Update_Priority(const _float fTimeDelta)		override;
	virtual void			Update(const _float fTimeDelta)					override;
	virtual void			Update_Late(const _float fTimeDelta)			override;
	virtual void			Ready_Before_Render(const _float fTimeDelta)	override;
	virtual HRESULT			Render()										override;
public:
	virtual void			OnCollision(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther) override;
	virtual void			OnCollision_Enter(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther, const COL_HIT_INFO& tHitInfo) override;
	virtual void			OnCollision_Exit(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther) override;
	virtual void			OnTrigger_Enter(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther, const COL_HIT_INFO& tHitInfo)override;
	virtual void			OnTrigger_Exit(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther) override;
public:
	virtual void			QuestEnter() override;
	virtual void			QuestExit() override;

private:
	string					m_strCinemaitcCameraSequnceName{ "" };
public:
	static CTriggerBox_CinematicPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual	CGameObject* Clone(void* pArg)override;
	virtual	void					Free()override;
};

