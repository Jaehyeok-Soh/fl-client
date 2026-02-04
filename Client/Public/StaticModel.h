#pragma once
#include "MapObject.h"
#include "DataStruct_Map.h"

NS_BEGIN(Client)

class CStaticModel final  : public CMapObject
{
	using Super = CMapObject;
public:
	typedef struct tagStaticModel_Desc : public CMapObject::MAPOBJECT_DESC
	{
		DTO::USING_MODEL_INFO	 tUsingModelInfo{};
	}STATICMODEL_DESC;
public:
	CStaticModel(ID3D11Device* pDevice , ID3D11DeviceContext* pContext);
	CStaticModel(const CStaticModel& rhs);
	virtual ~CStaticModel() = default;
private:
	virtual HRESULT			Initialize_Prototype()							override;
	virtual HRESULT			Initialize(void* pArg)							override;
	HRESULT					Ready_Component(STATICMODEL_DESC* pDesc);
	HRESULT					Change_OverrideMtl(STATICMODEL_DESC* pDesc);
	
	HRESULT					Ready_PhysicsComponent(STATICMODEL_DESC* pDesc);
	HRESULT					Ready_PhysicsCollider(STATICMODEL_DESC* pDesc);
	HRESULT					Ready_PhysicsRigidBody(STATICMODEL_DESC* pDesc);
public:
	virtual HRESULT			Awake(const _uint iCurrentLevelID)				override;
	virtual void			Update_Priority(const _float fTimeDelta)		override;
	virtual void			Update(const _float fTimeDelta)					override;
	virtual void			Update_Late(const _float fTimeDelta)			override;
	virtual void			Ready_Before_Render(const _float fTimeDelta)	override;
	virtual HRESULT			Render()										override;
private:

public:
	static CStaticModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg)								override;
	virtual void Free() override;
};

NS_END

