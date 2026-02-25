#pragma once
#include "MapObject.h"

class CWater : public CMapObject
{
	using Super = Client::CMapObject;
public:
	typedef struct Water_Desc : public CMapObject::MAPOBJECT_DESC
	{

	}WATER_DESC;
protected:
	CWater(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CWater(const CWater& rhs);
	virtual ~CWater() {}
protected:
	virtual HRESULT			Initialize_Prototype()							override;
	virtual HRESULT			Initialize(void* pArg)							override;
	HRESULT					Ready_Component(WATER_DESC* pDesc);
public:
	virtual HRESULT			Awake(const _uint iCurrentLevelID)				override;
	virtual void			Update_Priority(const _float fTimeDelta)		override;
	virtual void			Update(const _float fTimeDelta)					override;
	virtual void			Update_Late(const _float fTimeelta)				override;
	virtual void			Ready_Before_Render(const _float fTimeDelta)	override;
	virtual HRESULT			Render()										override;
public:
	static CWater*					Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg)override;
	virtual					void Free() override;
};

