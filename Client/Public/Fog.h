#pragma once
#include "EnvObject.h"


NS_BEGIN(Client)

class CFog final: public CEnvObject
{
	using Super = CEnvObject;
public:
	typedef struct tagFog_Desc : public CEnvObject::ENVOBJECT_DESC
	{

	}FOG_DESC;
protected:
	CFog(ID3D11Device* pDeivce,ID3D11DeviceContext* pContext);
	CFog(const CFog& rhs);
	virtual ~CFog() {}
protected:
	virtual HRESULT			Initialize_Prototype()override;
	virtual HRESULT			Initialize(void* pArg)override;
public:
	virtual HRESULT			Awake(const _uint iCurrentLevelID)				override;
	virtual void			Update_Priority(const _float fTimeDelta)		override;
	virtual void			Update(const _float fTimeDelta)					override;
	virtual void			Update_Late(const _float fTimeelta)				override;
	virtual void			Ready_Before_Render(const _float fTimeDelta)	override;
	virtual HRESULT			Render()										override;
public:
	static CFog*			Create(ID3D11Device* pDeivce, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

NS_END

