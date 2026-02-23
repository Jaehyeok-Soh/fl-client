#pragma once
#include "GameObject.h"

NS_BEGIN(Client)

class CWorldUI final : public CGameObject
{
	typedef struct tagWorldUIDesc
	{

	}WORLDUI_DESC;
	using Super = CGameObject;
private:
	CWorldUI(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CWorldUI(const CWorldUI& rhs);
	virtual ~CWorldUI() = default;
public:
	HRESULT Initialize_Prototype() override;
	HRESULT Initialize(void* pArg) override;
public:
	virtual HRESULT Awake(const _uint iCurrentLevelID) override;
	virtual void Update_Priority(const _float fTimeDelta) override;
	virtual void Update(const _float fTimeDelta) override;
	virtual void Update_Late(const _float fTimeDelta) override;
	virtual void Ready_Before_Render(const _float fTimeDelta) override;
	virtual HRESULT Render() override;
private:
	HRESULT Ready_Components(WORLDUI_DESC* pDesc);
	HRESULT Bind_ShaderResources();
public:
	static CWorldUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CGameObject* Clone(void* pArg);
	virtual void Free()override;
};

NS_END