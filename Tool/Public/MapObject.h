#pragma once
#include "ToolObject.h"


NS_BEGIN(Tool)

class CMapObject : public CToolObject
{


	using Super = CToolObject;
public:
	typedef struct tagMapObjectDesc : public CToolObject::TOOLOBJECT_DESC
	{
		wstring wstrModelTag{L""};
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
private:
	/* 내가 해당하는 MapObject Type 에 따른 Desc을 작성을 다르게할예정 */
	wstring					m_wstrModelName{L""};
	EMapObject_Type			m_eMapObjectType{ EMapObject_Type::END };
	void*					m_pDesc{nullptr};
public:
	virtual void			Free()									override;
};

NS_END

