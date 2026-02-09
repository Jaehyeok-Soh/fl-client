#pragma once
#include "GameObject.h"
#include "DataStruct_Map.h"

NS_BEGIN(Engine)

class CMaterial;
NS_END


NS_BEGIN(Client)


/* Desc를 받을떄 조심할점 받는쪽에 new로 할당받아서 들어오기 떄문에 받고나서의 데이터를 확인해야한다 */

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
		_bool						isUELoaded{false};
		EMapObject_DrawType			eMapObjectDrawType{EMapObject_DrawType::END};
		wstring						wstrModelPath{};
		vector<DTO::SRT_DATA>		vecSRT{};
	}MAPOBJECT_DESC;
protected:
	CMapObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CMapObject(const CMapObject& rhs);
	virtual ~CMapObject() = default;
public:
	virtual HRESULT			Initialize_Prototype()							override;
	virtual HRESULT			Initialize(void* pArg)							override;
	HRESULT					Ready_Transform(MAPOBJECT_DESC* pDesc);
	HRESULT					Ready_Component(MAPOBJECT_DESC* pDesc);
	HRESULT					Ready_OverrideMtl(const DTO::USING_MODEL_INFO& tUsingModelInfo);

	HRESULT					Ready_PhysicsComponent(MAPOBJECT_DESC* pDesc);
	HRESULT					Ready_PhysicsCollider(MAPOBJECT_DESC* pDesc);
	HRESULT					Ready_PhysicsRigidBody(MAPOBJECT_DESC* pDesc);
public:
	HRESULT					Add_MapToolComponent(CMapObject::COMPONENT eType);
public:
	virtual HRESULT			Awake(const _uint iCurrentLevelID)				override;
	virtual void			Update_Priority(const _float fTimeDelta)		override;
	virtual void			Update(const _float fTimeDelta)					override;
	virtual void			Update_Late(const _float fTimeelta)				override;
	virtual void			Ready_Before_Render(const _float fTimeDelta)	override;
	virtual HRESULT			Render()										override;
	HRESULT					Render_Instance();
	HRESULT					Render_Default();
protected:
	_bool					m_isUELoaded{false};
	EMapObject_DrawType		m_eMapObjectDrawType{EMapObject_DrawType::Default};
	EMapObject_Type			m_eMapObjectType{ EMapObject_Type::END };
	vector<Matrix>			m_vecMatrix{};
public:
	virtual void			Free()	override;
};


NS_END