#pragma once
#include "MapObject.h"
#include "DataStruct_Map.h"

NS_BEGIN(Client)

class CStaticObject final  : public CMapObject
{
	using Super = CMapObject;
public:
	typedef struct tagStaticObject_Desc : public CMapObject::MAPOBJECT_DESC
	{
		Vec3					 vScale_Isolated = {};
	}STATICOBJECT_DESC;
public:
	CStaticObject(ID3D11Device* pDevice , ID3D11DeviceContext* pContext);
	CStaticObject(const CStaticObject& rhs);
	virtual ~CStaticObject() = default;
private:
	virtual HRESULT			Initialize_Prototype()							override;
	virtual HRESULT			Initialize(void* pArg)							override;
	HRESULT					Ready_Component(STATICOBJECT_DESC* pDesc);
public:
	virtual HRESULT			Awake(const _uint iCurrentLevelID)				override;
	virtual void			Update_Priority(const _float fTimeDelta)		override;
	virtual void			Update(const _float fTimeDelta)					override;
	virtual void			Update_Late(const _float fTimeDelta)			override;
	virtual void			Ready_Before_Render(const _float fTimeDelta)	override;
	virtual HRESULT			Render()										override;
private:

public:
	static CStaticObject* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg)								override;
	virtual void Free() override;
};

NS_END

