#pragma once
#include "MapObject.h"



NS_BEGIN(Client)
class Effect;

class CEnvObject : public CMapObject
{
	using Super = CMapObject;
protected:
	typedef struct tagEnvObject_Desc : public CMapObject::MAPOBJECT_DESC
	{

	}ENVOBJECT_DESC;
protected:
	CEnvObject(ID3D11Device* pDevice , ID3D11DeviceContext* pContext);
	CEnvObject(const CEnvObject& rhs);
	virtual ~CEnvObject() {}
protected:
	virtual HRESULT			Initialize_Prototype()override;
	virtual HRESULT			Initialize(void* pArg) override;
public:
	virtual HRESULT			Ready_Effect();
public:
	virtual HRESULT			Awake(const _uint iCurrentLevelID)				override;
	virtual void			Update_Priority(const _float fTimeDelta)		override;
	virtual void			Update(const _float fTimeDelta)					override;
	virtual void			Update_Late(const _float fTimeelta)				override;
	virtual void			Ready_Before_Render(const _float fTimeDelta)	override;
	virtual HRESULT			Render()										override;
public:
	vector<Effect*>			m_vecEffect;
public:
	virtual void Free() override;
};


NS_END