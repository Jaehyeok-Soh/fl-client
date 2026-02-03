#pragma once
#include "MapObject.h"

NS_BEGIN(Tool)

class CInstanceModel  final : public CMapObject
{
	using Super = CMapObject;
public:
	typedef struct tagInstanceModel_Desc : public CMapObject::MAPOBJECT_DESC
	{
		INSTANCEMODEL_DATA tData{};
	}INSTANCEMODEL_DESC;
public:
	CInstanceModel(EToolObjectType eType, ID3D11Device* pDevice , ID3D11DeviceContext* pContext);
	explicit CInstanceModel(const CInstanceModel& rhs);
	virtual ~CInstanceModel() = default;
public:
	virtual HRESULT			Initialize_Prototype()							override;
	virtual HRESULT			Initialize(void* pArg)							override;
	HRESULT					Ready_Component();
public:
	virtual HRESULT			Awake(const _uint iCurrentLevelID)				override;
	virtual void			Update_Priority(const _float fTimeDelta)		override;
	virtual void			Update(const _float fTimeDelta)					override;
	virtual void			Update_Late(const _float fTimeDelta)			override;
	virtual void			Ready_Before_Render(const _float fTimeDelta)	override;
	virtual HRESULT			Render()										override;
	virtual void			Draw_ImGui()									override;
private:
	INSTANCEMODEL_DATA		m_tData{};
public:
	static  CInstanceModel* Create(EToolObjectType eType, ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg)	override;
	virtual void			Free()				override;

	// CMapObject을(를) 통해 상속됨
	_bool Picking(OUT Vec3& vOut) override;
};

NS_END