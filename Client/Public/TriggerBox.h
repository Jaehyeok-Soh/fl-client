#pragma once
#include "GameObject.h"

NS_BEGIN(Client)

class CTriggerBox : public CGameObject
{
	using Super = CGameObject;
public:
	typedef struct tagTriggerBox_Desc : public CGameObject::GAMEOBJECT_DESC
	{
	}TRIGGERBOX_DESC;
protected:
	CTriggerBox(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CTriggerBox(const CTriggerBox& rhs);
	virtual ~CTriggerBox() = default;
public:
	virtual HRESULT			Initialize_Prototype()							override;
	virtual HRESULT			Initialize(void* pArg)							override;
	HRESULT					Ready_Transform(TRIGGERBOX_DESC* pDesc);
	HRESULT					Ready_Component(TRIGGERBOX_DESC* pDesc);
public:
	virtual HRESULT			Awake(const _uint iCurrentLevelID)				override;
	virtual void			Update_Priority(const _float fTimeDelta)		override;
	virtual void			Update(const _float fTimeDelta)					override;
	virtual void			Update_Late(const _float fTimeDelta)				override;
	virtual void			Ready_Before_Render(const _float fTimeDelta)	override;
	virtual HRESULT			Render()										override;
	HRESULT					Render_Instance(_uint iPassIndex = 0);
	HRESULT					Render_Default(_uint iPassIndex = 0);

public:
	virtual					void Free()override;
};

NS_END

