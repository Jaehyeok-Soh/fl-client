#pragma once
#include "MapObject.h"


NS_BEGIN(Engine)

class CInstanceMesh;

NS_END

NS_BEGIN(Tool)

class CInstanceModel final : public Tool::CMapObject
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
	HRESULT					Ready_SRTData();
public:
	virtual HRESULT			 Awake(const _uint iCurrentLevelID)				override;
	virtual void			 Update_Priority(const _float fTimeDelta)		override;
	virtual void			 Update(const _float fTimeDelta)					override;
	virtual void			 Update_Late(const _float fTimeDelta)			override;
	virtual void			 Ready_Before_Render(const _float fTimeDelta)	override;
	virtual HRESULT			 Render()										override;
	virtual void			 Draw_ImGui()									override;

	virtual USING_MODEL_INFO Get_UsingModelInfo() const { return m_tData.tUsingModelInfo; }
	virtual vector<SRT_DATA> Get_SRTDatas()override{ return m_tData.vecSRT; }
	virtual Matrix			 Get_WorldMatrix() override;
	virtual void			 Set_WorldMatrix(const Matrix& WorldMatrix)		override;
public:
	virtual void			 Reset_OriginTransform(_uint iIndex)override;

	void					 Update_InstanceWorldMatrix(_uint iIndex);
	void					 Update_InstanceWorldMatirx( const SimpleMath::Matrix& WorldMatrix ,_uint iIndex);
public:
	bool					 IntsersectWithPlane(OUT Vec3& vOut);
	void					 Set_SelctInstanceID(_int iIndex) { m_iSelectInstanceID = iIndex; }
public:
	INSTANCEMODEL_DATA&		Get_Data()			{ return m_tData; }
	_uint					Get_InstanceCount() { return  (_uint)m_tData.vecMatirx.size(); }
	_int					Get_iSelectInstanceID() const { return m_iSelectInstanceID; }
	// CMapObject을(를) 통해 상속됨
	_bool Picking(OUT Vec3& vOut) override;

	virtual _bool			Export_Data(DTO::ECategory eCategory, CDataDocumentBase* pDocument)		override;

private:

	_int					m_iSelectInstanceID{0};
	INSTANCEMODEL_DATA		m_tData{};
public:
	static  CInstanceModel* Create(EToolObjectType eType, ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg)	override;
	virtual void			Free()				override;
};

NS_END