#pragma once
#include "UIObject.h"
#include "Tool_Defines.h"

NS_BEGIN(Tool)

class CToolUI final : public CUIObject
{
	using Super = CUIObject;
public:
	typedef struct tagToolUIDesc : public Super::UIOBJECT_DESC
	{
		_wstring wstrTextureTag;

	}TOOLUI_DESC;

private:
	CToolUI(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CToolUI(const CToolUI& rhs);
	virtual ~CToolUI() = default;

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

public:
	static CToolUI* Create(EToolObjectType eType, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

NS_END


//#include "Client_Defines.h"
//#include "Texture.h"
//#include "TextureBase.h"
//#include "GameInstance.h"
//#include "Shader.h"
//#include "VIBuffer_Rect_Tex.h"
//#include "UI_Button.h"
//
//CUI_Button::CUI_Button(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
//    : Super(pDevice, pDeviceContext)
//{
//}
//
//CUI_Button::CUI_Button(const CUIObject& rhs)
//    : Super(rhs)
//{
//}
//
//HRESULT CUI_Button::Initialize_Prototype()
//{
//    if (FAILED(Super::Initialize_Prototype()))
//        return E_FAIL;
//
//    return S_OK;
//}
//
//HRESULT CUI_Button::Initialize(void* pArg)
//{
//    if (FAILED(Super::Initialize(pArg)))
//        return E_FAIL;
//
//    UIBUTTON_DESC* pDesc = static_cast<UIBUTTON_DESC*>(pArg);
//
//    if (FAILED(Ready_Component(pDesc)))
//        return E_FAIL;
//
//    return S_OK;
//}
//
//HRESULT CUI_Button::Awake(const _uint iCurrentLevelID)
//{
//    if (FAILED(Super::Awake(iCurrentLevelID)))
//        return E_FAIL;
//
//    Set_SizeToTextureScale();
//    return S_OK;
//}
//
//void CUI_Button::Update_Priority(const _float fTimeDelta)
//{
//    if (IsVisible())
//        Super::Update_Priority(fTimeDelta);
//}
//
//void CUI_Button::Update(const _float fTimeDelta)
//{
//    if (IsVisible())
//        Super::Update(fTimeDelta);
//}
//
//void CUI_Button::Update_Late(const _float fTimeDelta)
//{
//    if (IsVisible())
//        Super::Update_Late(fTimeDelta);
//}
//
//void CUI_Button::Ready_Before_Render(const _float fTimeDelta)
//{
//    if (IsVisible())
//        Super::Ready_Before_Render(fTimeDelta);
//}
//
//HRESULT CUI_Button::Render()
//{
//    if (FAILED(Super::Render()))
//        return E_FAIL;
//
//    if (FAILED(Bind_ShaderResources()))
//        return E_FAIL;
//
//    Get_Component<CShader>()->Apply();
//    Get_Component<CVIBuffer>()->Bind_Resource();
//    Get_Component<CVIBuffer>()->Render();
//
//    m_pGameInstance->Draw_Text(L"Font_Default", m_wstrText.c_str(), _float2{ Get_PosX(), Get_PosY() }, ::XMVectorSet(0.f, 0.f, 0.f, 1.f));
//    return S_OK;
//}
//
//_bool CUI_Button::IsPicked()
//{
//    if (Super::IsPicked())
//    {
//        if (MOUSE_LBUTTON_DOWN)
//            Invoke_Event();
//
//        return true;
//    }
//    else
//    {
//        return false;
//    }
//}
//
//void CUI_Button::OffPicked()
//{
//    Super::OffPicked();
//}
//
//void CUI_Button::Invoke_Event()
//{
//    if (m_onEvent)
//        m_onEvent();
//}
//
//void CUI_Button::On_Selected(_bool bSizeEffect)
//{
//    if (bSizeEffect)
//        Set_Size(m_fSizeX *= 1.05f, m_fSizeY *= 1.05f);
//
//    m_bSelected = true;
//}
//
//void CUI_Button::Off_Selected(_bool bSizeEffect)
//{
//    if (bSizeEffect)
//        Set_Size(m_fSizeX /= 1.05f, m_fSizeY /= 1.05f);
//
//    m_bSelected = false;
//}
//
//HRESULT CUI_Button::Ready_Component(UIBUTTON_DESC* pDesc)
//{
//    if (FAILED(Add_Component<CTexture>(0 /*static*/, pDesc->wstrTextureTag, pDesc)))
//        return E_FAIL;
//
//    if (FAILED(Add_Component<CShader>(0 /*static*/, L"Prototype_Component_Shader_VtxPosTex", pDesc)))
//        return E_FAIL;
//
//    if (FAILED(Add_Component<CVIBuffer_Rect_Tex>(0 /*static*/, L"Prototype_Component_VIBuffer_Rect_Tex", pDesc)))
//        return E_FAIL;
//
//    return S_OK;
//}
//
//HRESULT CUI_Button::Bind_ShaderResources()
//{
//    CShader* pShader = Get_Component<CShader>();
//    if (FAILED(Get_Component<CTransform>()->Bind_ShaderResource(pShader)))
//        return E_FAIL;
//
//
//    if (FAILED(Get_Component<CTexture>()->Bind_ShaderResource(pShader, m_bSelected == false ? 0 : 1)))
//        return E_FAIL;
//
//    return S_OK;
//}
//
//CUI_Button* CUI_Button::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
//{
//    CUI_Button* pInstance = new CUI_Button(pDevice, pDeviceContext);
//
//    if (FAILED(pInstance->Initialize_Prototype()))
//    {
//        MSG_BOX("CUI_Button::Create, Failed");
//        Safe_Release(pInstance);
//    }
//
//    return pInstance;
//}
//
//CGameObject* CUI_Button::Clone(void* pArg)
//{
//    CUI_Button* pInstance = new CUI_Button(*this);
//
//    if (FAILED(pInstance->Initialize(pArg)))
//    {
//        MSG_BOX("CUI_Button::Clone, Failed");
//        Safe_Release(pInstance);
//    }
//
//    return pInstance;
//}
//
//void CUI_Button::Free()
//{
//    Super::Free();
//}
