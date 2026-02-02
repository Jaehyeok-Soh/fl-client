#include "pch.h"
#include "Button.h"
#include "UIObject.h"
#include "ToolCanvas.h"
#include "ToolUI.h"
#include "ImGui_ToolManager.h"
#include "GameInstance.h"

CButton::CButton()
{
}

CButton::CButton(const CButton& rhs)
{
}

HRESULT CButton::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CButton::Initialize_Prototype(const BUTTON_DESC& Desc)
{
	m_pOwner = Desc.pOwner;
	return S_OK;
}

HRESULT CButton::Initialize(void* pArg)
{
	BUTTON_DESC* pDesc = static_cast<BUTTON_DESC*>(pArg);
	return S_OK;
}

void CButton::Update(const _float fTimeDelta)
{
	if(nullptr != m_pOwner)
		OnClick();
}

void CButton::OnClick()
{
	if (Engine_Utils::Has_Flag(m_pOwner->Get_InteractState(), CUIObject::EInteractState::NONE))
		m_pOwner->Set_TextureIndex(0);
	if (Engine_Utils::Has_Flag(m_pOwner->Get_InteractState(), CUIObject::EInteractState::HOVERING_ENTER))
		m_pOwner->Set_TextureIndex(1);
	if (Engine_Utils::Has_Flag(m_pOwner->Get_InteractState(), CUIObject::EInteractState::HOVERING_EXIT))
		m_pOwner->Set_TextureIndex(2);
	if (Engine_Utils::Has_Flag(m_pOwner->Get_InteractState(), CUIObject::EInteractState::PRESS_ENTER))
		m_pOwner->Set_TextureIndex(3);
	if (Engine_Utils::Has_Flag(m_pOwner->Get_InteractState(), CUIObject::EInteractState::PRESSING))
		m_pOwner->Set_TextureIndex(4);
	if (Engine_Utils::Has_Flag(m_pOwner->Get_InteractState(), CUIObject::EInteractState::PRESS_EXIT))
		m_pOwner->Set_TextureIndex(5);
	if (Engine_Utils::Has_Flag(m_pOwner->Get_InteractState(), CUIObject::EInteractState::CLICKED))
		m_pOwner->Set_TextureIndex(6);
}

CButton* CButton::Create(const BUTTON_DESC& Desc)
{
	CButton* pInstance = new CButton();
	if (FAILED(pInstance->Initialize_Prototype(Desc)))
	{
		MSG_BOX("CButton::Create, Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CButton::Clone(void* pArg)
{
	CButton* pInstance = new CButton(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CButton::Clone, Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CButton::Free()
{
	Super::Free();
}


