#pragma once
#include "GameObject.h"
#include "DataStruct_Map.h"

NS_BEGIN(Engine)

class CMaterial;
NS_END


NS_BEGIN(Client)

class CMapObject abstract : public CGameObject
{
	using Super = CGameObject;
public:
	enum class COMPONENT
	{
		END
	};
public:
	typedef struct tagMapObjectDesc : public CGameObject::GAMEOBJECT_DESC
	{
		Client::EMapObject_Type		eType{ Client::EMapObject_Type::END };
	}MAPOBJECT_DESC;
protected:
	CMapObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CMapObject(const CMapObject& rhs);
	virtual ~CMapObject() = default;
public:
	virtual HRESULT			Initialize_Prototype()							override;
	virtual HRESULT			Initialize(void* pArg)							override;
	HRESULT					Ready_Component();
	HRESULT					Ready_OverrideMtl(const DTO::USING_MODEL_INFO& tUsingModelInfo);
public:
	HRESULT					Add_MapToolComponent(CMapObject::COMPONENT eType);
public:
	virtual HRESULT			Awake(const _uint iCurrentLevelID)				override;
	virtual void			Update_Priority(const _float fTimeDelta)		override;
	virtual void			Update(const _float fTimeDelta)					override;
	virtual void			Update_Late(const _float fTimeelta)				override;
	virtual void			Ready_Before_Render(const _float fTimeDelta)	override;
	virtual HRESULT			Render()										override;
protected:
	Client::EMapObject_Type			m_eMapObjectType{ Client::EMapObject_Type::END };
	array< CMonoBehaviour*, ENUM_TO_UINT(Client::CMapObject::COMPONENT::END)> m_arrayMapToolComponent{};


	bool					m_iUseOverrideMaterials{ false };
	vector<CMaterial*>		m_vecOverrideMaterials{};

public:
	virtual void			Free()	override;
};


NS_END