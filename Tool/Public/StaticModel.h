#pragma once
#include "MapObject.h"
#include "UEMapdataParser.h"

class CStaticModel : public CMapObject
{
	using Super = CMapObject;
public:
	typedef struct tagStaticModel : public CMapObject::MAPOBJECT_DESC
	{
		STATICMODEL_DATA  tData{};
		CONVERTED_MAPDATA tLoadData{};
	public:
	}STATICMODEL_DESC;
protected:
	CStaticModel(EToolObjectType eType, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CStaticModel(const CStaticModel& rhs);
	virtual ~CStaticModel() = default;
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
	virtual void			Set_Dead(const wstring& wstrLayerTag)			override;
public:
	void					Set_StaticModelType(EStaticModel_Type eType) { m_eType = eType; }
	EStaticModel_Type		Get_StaticModelType() const { return m_eType; }
private:
	bool					IntsersectWithPlane(OUT Vec3& vOut);
public:
	virtual _bool			Picking(OUT Vec3& vOut)						override;
	virtual _bool			Export_Data(DTO::ECategory eCategory, CDataDocumentBase* pDocument)		override;
private:
	STATICMODEL_DATA		m_tData{};
	EStaticModel_Type		m_eType{ EStaticModel_Type::DEFUALT};
public:
	static  CStaticModel*	Create(EToolObjectType eType, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg)								override;
	virtual void			Free()											override;
};

