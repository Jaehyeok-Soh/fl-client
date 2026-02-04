#pragma once
#include "UIObject.h"

NS_BEGIN(Client)
class CCanvas;
class CGenericUI;
class CUILayer final : public CUIObject
{
	using Super = CUIObject;

public:
	typedef struct tagUILayerDesc : public UIOBJECT_DESC
	{
		CCanvas* pCanvasCache = { nullptr };
	}UILAYER_DESC;

private:
	CUILayer(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CUILayer(const CUILayer& rhs);
	virtual ~CUILayer() = default;

public:
	HRESULT Initialize_Prototype() override;
	HRESULT Initialize(void* pArg) override;

public:
	virtual HRESULT Awake(const _uint iCurrentLevelID) override;
	void Transmit_for_UI();
	virtual void Update_Priority(const _float fTimeDelta) override;
	virtual void Update(const _float fTimeDelta) override;
	virtual void Update_Late(const _float fTimeDelta) override;
	virtual void Ready_Before_Render(const _float fTimeDelta) override;
	virtual HRESULT Render() override;

	vector<CGenericUI*>* Get_UIVector() { return &m_vecUIs; }
	
private:
	HRESULT Ready_Components(UILAYER_DESC* pDesc);
	HRESULT Bind_ShaderResources();

private:
	vector<CGenericUI*> m_vecUIs;
	CCanvas* m_pParentCanvasCache = { nullptr };

public:
	static CUILayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CGameObject* Clone(void* pArg);
	virtual void Free()override;

};

NS_END