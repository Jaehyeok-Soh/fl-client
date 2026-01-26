#pragma once
#include "UIObject.h"
#include "Tool_Defines.h"

NS_BEGIN(Tool)

class CToolUI;

class CToolLayer final : public CUIObject
{
	using Super = CUIObject;
public:
	typedef struct tagToolUIDesc : public Super::UIOBJECT_DESC
	{
		_wstring wstrTextureTag;

	}TOOLUI_DESC;

private:
	CToolLayer(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CToolLayer(const CToolLayer& rhs);
	virtual ~CToolLayer() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg)override;

	virtual HRESULT Awake(const _uint iCurrentLevelID) override;
	virtual void Update_Priority(const _float fTimeDelta) override;
	virtual void Update(const _float fTimeDelta) override;
	virtual void Update_Late(const _float fTimeDelta) override;
	virtual void Ready_Before_Render(const _float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_Components(TOOLUI_DESC* pDesc);
	HRESULT Bind_ShaderResources();

private:
	vector<CToolUI*> m_vecToolUIs;

	/* 아마 있어야될 변수 */
	/* 얘의 m_iPass -> vecToolUIs 가 동일하게 가짐 */

public:
	static CToolLayer* Create(EToolObjectType eType, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

NS_END

