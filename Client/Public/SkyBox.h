#pragma once
#include "GameObject.h"

NS_BEGIN(Engine)
class CTexture;
NS_END


NS_BEGIN(Client)

class CSkyBox : public CGameObject
{
	using Super = CGameObject;
public:
	typedef struct tagSkyBox_Desc : public CGameObject::GAMEOBJECT_DESC
	{
		wstring wstrModelTag{L""};
		wstring wstrTextureTag{L""};
	}SKYBOX_DESC;
protected:
	CSkyBox(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CSkyBox(const CSkyBox& rhs);
	virtual ~CSkyBox() {}
public:
	HRESULT					Initialize_Prototype() override;
	HRESULT					Initialize(void* pArg) override;
protected:
	HRESULT					Ready_Component(SKYBOX_DESC* pDesc);
public:
	virtual HRESULT			Awake(const _uint iCurrentLevelID)override;
	virtual void			Update_Priority(const _float fTimeDelta)override;
	virtual void			Update(const _float fTimeDelta)override;
	virtual void			Update_Late(const _float fTimeDelta)override;
	virtual void			Ready_Before_Render(const _float fTimeDelta)override;
	virtual HRESULT			Render()override;
public:
	float					m_fAccDT;
public:
	static CSkyBox*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg)override;
	virtual void			Free()override;
};

NS_END

