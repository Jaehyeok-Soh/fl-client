#pragma once
#include "MapObject.h"
#include "DataStruct_Map.h"

NS_BEGIN(Client)

class CInstanceModel : public CMapObject
{
	using Super = CMapObject;
public:
	typedef struct tagInstanceModel_Desc : public CMapObject::MAPOBJECT_DESC
	{
		DTO::TMap_InstanceModelData	tData{};
	}INSTANCEMODEL_DESC;
public:
	CInstanceModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CInstanceModel(const CInstanceModel& rhs);
	virtual ~CInstanceModel() = default;
private:
	virtual HRESULT Initialize_Prototype()		override;
	virtual HRESULT Initialize(void* pArg)		override;
	HRESULT Ready_Component(INSTANCEMODEL_DESC* pDesc);

	HRESULT					Ready_PhysicsComponent(INSTANCEMODEL_DESC* pDesc);
	HRESULT					Ready_PhysicsCollider(INSTANCEMODEL_DESC* pDesc);
	HRESULT					Ready_PhysicsRigidBody(INSTANCEMODEL_DESC* pDesc);
public:
	virtual HRESULT			Awake(const _uint iCurrentLevelID)				override;
	virtual void			Update_Priority(const _float fTimeDelta)		override;
	virtual void			Update(const _float fTimeDelta)					override;
	virtual void			Update_Late(const _float fTimeelta)				override;
	virtual void			Ready_Before_Render(const _float fTimeDelta)	override;
	virtual HRESULT			Render()										override;
private:
	vector<Matrix>			m_vecMatrix{};
public:
	static  CInstanceModel* Create(ID3D11Device* pDevice , ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg)	override;
	virtual void			Free()				override;
};

NS_END
