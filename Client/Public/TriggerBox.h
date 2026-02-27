#pragma once
#include "GameObject.h"


NS_BEGIN(DTO)

struct SRT_DATA;

NS_END

NS_BEGIN(Client)

class CTriggerBox : public CGameObject
{
public:
	enum class Type
	{
		CHANGE_LEVEL,
		MONSTER_SPAWNER,
		END,
	};

	using Super = CGameObject;
protected:
	typedef struct tagTriggerBox_Desc : public CGameObject::GAMEOBJECT_DESC
	{
		/* Trigger Box */
		Vec3					vTriggerBox_Extents{1.f,1.f,1.f};
		/* ÇÊ¼ö */
		const DTO::SRT_DATA*	pSRTData{nullptr};
	}TRIGGERBOX_DESC;
protected:
	CTriggerBox(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CTriggerBox(const CTriggerBox& rhs);
	virtual ~CTriggerBox() = default;
public:
	virtual HRESULT			Initialize_Prototype()							override;
	virtual HRESULT			Initialize(void* pArg)							override;
	HRESULT					Ready_Component(TRIGGERBOX_DESC* pDesc);
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
	CTriggerBox::Type		Get_TriggerBoxType() const { return m_eTriggerBoxType; }

protected:
	CTriggerBox::Type		m_eTriggerBoxType{ CTriggerBox::Type::END};

public:
	virtual					void Free()override;
};

NS_END

