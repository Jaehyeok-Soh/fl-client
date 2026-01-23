#pragma once
#include "MapObject.h"

class CStaticModel : public CMapObject
{
	using Super = CMapObject;
public:
	typedef struct tagStaticMesh : public CMapObject::MAPOBJECT_DESC
	{
	}STATICMODEL_DESC;
protected:
	CStaticModel(EToolObjectType eType, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CStaticModel(const CStaticModel& rhs);
	virtual ~CStaticModel() = default;
public:
	virtual HRESULT			Initialize_Prototype()							override;
	virtual HRESULT			Initialize(void* pArg)							override;
public:
	virtual HRESULT			Awake(const _uint iCurrentLevelID)				override;
	virtual void			Update_Priority(const _float fTimeDelta)		override;
	virtual void			Update(const _float fTimeDelta)					override;
	virtual void			Update_Late(const _float fTimeDelta)			override;
	virtual void			Ready_Before_Render(const _float fTimeDelta)	override;
	virtual HRESULT			Render()										override;
	virtual void			Draw_ImGui()									override;
	virtual void			Set_Dead(const wstring& wstrLayerTag)			override;
public:
	virtual _bool			Picking(OUT Vec3& vOut)						override;
	virtual HRESULT			Export_Data(OUT MAPOBJECT_SAVEDATA& data)		override;
public:
	static  CStaticModel*	Create(EToolObjectType eType, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg)								override;
	virtual void			Free()											override;
};

