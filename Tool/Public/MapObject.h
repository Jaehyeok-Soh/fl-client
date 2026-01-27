#pragma once
#include "ToolObject.h"


NS_BEGIN(Tool)





class CMapObject : public CToolObject
{
	using Super = CToolObject;
public:

	typedef struct tagMapObjectDesc : public CToolObject::TOOLOBJECT_DESC
	{
		bool			isLoaded{ false };
		wstring			wstrModelName{L""};
		wstring			wstrModelPath{L""};
	}MAPOBJECT_DESC;

protected:
	CMapObject(EToolObjectType eType, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CMapObject(const CMapObject& rhs);
	virtual ~CMapObject() = default;
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
	virtual void			Draw_ImGui()override;
protected:
	EMapObject_Type			m_eMapObjectType{ EMapObject_Type::END };
	bool					m_isLoaded{false};
	void*					m_pDesc{nullptr};
public:
	virtual void			Free()									override;
};

NS_END

