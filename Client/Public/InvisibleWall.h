#pragma once
#include "MapObject.h"

NS_BEGIN(Client)

class CInvisibleWall :  public CMapObject
{
	using Super = Client::CMapObject;
public:
	typedef struct tagInvisibleWall_Desc : public CMapObject::MAPOBJECT_DESC
	{

	}INVISIBLEWALL_DESC;
protected:
	CInvisibleWall(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CInvisibleWall(const CInvisibleWall& rhs);
	virtual ~CInvisibleWall() {}
protected:
	virtual HRESULT			Initialize_Prototype()							override;
	virtual HRESULT			Initialize(void* pArg)							override;
	HRESULT					Ready_Component(INVISIBLEWALL_DESC* pDesc);
public:
	virtual HRESULT			Awake(const _uint iCurrentLevelID)				override;
	virtual void			Update_Priority(const _float fTimeDelta)		override;
	virtual void			Update(const _float fTimeDelta)					override;
	virtual void			Update_Late(const _float fTimeelta)				override;
	virtual void			Ready_Before_Render(const _float fTimeDelta)	override;
	virtual HRESULT			Render()										override;
public:
	static	CInvisibleWall* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg)override;
	virtual					void Free() override;
};

NS_END