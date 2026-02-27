#pragma once
#include "TriggerBox.h"

class CTriggerBox_LevelChange : public CTriggerBox
{
	using Super = CTriggerBox;
public:
	typedef struct tagTriggerBox_LevelChange_Desc : public CTriggerBox::TRIGGERBOX_DESC
	{
		ELevelType				eChangeLevelType{ ELevelType::END };
	}TRIGGERBOX_LEVELCHANGE_DESC;
protected:
	CTriggerBox_LevelChange(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CTriggerBox_LevelChange(const CTriggerBox_LevelChange& rhs);
	virtual ~CTriggerBox_LevelChange() = default;
public:
	virtual HRESULT			Initialize_Prototype()							override;
	virtual HRESULT			Initialize(void* pArg)							override;
	HRESULT					Ready_Component(TRIGGERBOX_LEVELCHANGE_DESC* pDesc);
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
	virtual void			OnTrigger_Enter(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther)override;
	virtual void			OnTrigger_Exit(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther) override;
public:
protected:
	ELevelType				m_eChangeLevelType{ ELevelType ::END };
public:
	static CTriggerBox_LevelChange*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual	CGameObject*			Clone(void* pArg)override;
	virtual	void					Free()override;
};

