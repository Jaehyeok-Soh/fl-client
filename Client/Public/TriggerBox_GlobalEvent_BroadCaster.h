#pragma once
#include "TriggerBox.h"

NS_BEGIN(Client)

class CTriggerBox_GlobalEvent_BroadCaster : public CTriggerBox
{
	using Super = CTriggerBox;
public:
	typedef struct tagTriggerBox_GlobalEvent_BroadCaster_Desc : public CTriggerBox::TRIGGERBOX_DESC
	{
		vector<EGlobal_Broadcast_Type>	vecGlobalBroadcastType{ };
	}TRIGGERBOX_GLOBALEVENT_BROADCASTER_DESC;
protected:
	CTriggerBox_GlobalEvent_BroadCaster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CTriggerBox_GlobalEvent_BroadCaster(const CTriggerBox_GlobalEvent_BroadCaster& rhs);
	virtual ~CTriggerBox_GlobalEvent_BroadCaster() = default;
public:
	virtual HRESULT			Initialize_Prototype()							override;
	virtual HRESULT			Initialize(void* pArg)							override;
	HRESULT					Ready_Component(TRIGGERBOX_GLOBALEVENT_BROADCASTER_DESC* pDesc);
public:
	virtual HRESULT			Awake(const _uint iCurrentLevelID)				override;
	virtual void			Update_Priority(const _float fTimeDelta)		override;
	virtual void			Update(const _float fTimeDelta)					override;
	virtual void			Update_Late(const _float fTimeDelta)			override;
	virtual void			Ready_Before_Render(const _float fTimeDelta)	override;
	virtual HRESULT			Render()										override;

public:
	HRESULT					BroadCast();
public:
	virtual void			OnCollision(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther) override;
	virtual void			OnCollision_Enter(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther, const COL_HIT_INFO& tHitInfo) override;
	virtual void			OnCollision_Exit(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther) override;
	virtual void			OnTrigger_Enter(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther)override;
	virtual void			OnTrigger_Exit(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther) override;
public:
protected:
	vector<EGlobal_Broadcast_Type>	m_vecGlobalBroadcastType{};
public:
	static CTriggerBox_GlobalEvent_BroadCaster*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual	CGameObject*			Clone(void* pArg)override;
	virtual	void					Free()override;
};

NS_END