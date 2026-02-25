#pragma once
#include "MapObject.h"

NS_BEGIN(Client)

class CPlants : public CMapObject
{
	using Super = Client::CMapObject;
protected:
	typedef struct Plants_Desc : public CMapObject::MAPOBJECT_DESC
	{

	}PLANTS_DESC;
protected:
	CPlants(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CPlants(const CPlants& rhs);
	virtual ~CPlants() {}
protected:
	virtual HRESULT			Initialize_Prototype()							override;
	virtual HRESULT			Initialize(void* pArg)							override;
	HRESULT					Ready_Component(PLANTS_DESC* pDesc);
public:
	virtual HRESULT			Awake(const _uint iCurrentLevelID)				override;
	virtual void			Update_Priority(const _float fTimeDelta)		override;
	virtual void			Update(const _float fTimeDelta)					override;
	virtual void			Update_Late(const _float fTimeelta)				override;
	virtual void			Ready_Before_Render(const _float fTimeDelta)	override;
	virtual HRESULT			Render()										override;
public:
	virtual					void Free() override;
};


NS_END
