#include "pch.h"
#include "Canvas.h"
#include "Client_Defines.h"

//=================
// Component
//=================
#include "Texture.h"
#include "Shader.h"
#include "VIBuffer_Rect_Tex.h"

#include "UILayer.h"
#include "GenericUI.h"

#include "GameInstance.h"

CCanvas::CCanvas(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	:CUIObject(pDevice, pDeviceContext)
{
}

CCanvas::CCanvas(const CCanvas& rhs)
	:CUIObject(rhs)
{
}

HRESULT CCanvas::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;
	return S_OK;
}

HRESULT CCanvas::Initialize(void* pArg)
{
	CANVAS_DESC* pDesc = static_cast<CANVAS_DESC*>(pArg);

	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components(pDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CCanvas::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;
	return S_OK;
}

void CCanvas::Transmit_for_UI()
{
	for (auto* pLayer : m_vecUILayers)
	{
		if (!pLayer->IsVisible())
			continue;

		for (auto* pUI : *pLayer->Get_UIVector())
		{
			if (nullptr != pUI)
			{
				switch (pUI->Get_RectTransformType())
				{
				case Client::ERectTransform::LT:	pUI->Set_RectPos(Get_LT());
					break;
				case Client::ERectTransform::CT:	pUI->Set_RectPos(Get_CT());
					break;
				case Client::ERectTransform::RT:	pUI->Set_RectPos(Get_RT());
					break;
				case Client::ERectTransform::LC:	pUI->Set_RectPos(Get_LC());
					break;
				case Client::ERectTransform::C:		pUI->Set_RectPos(Get_C());
					break;
				case Client::ERectTransform::RC:	pUI->Set_RectPos(Get_RC());
					break;
				case Client::ERectTransform::LB:	pUI->Set_RectPos(Get_LB());
					break;
				case Client::ERectTransform::CB:	pUI->Set_RectPos(Get_CB());
					break;
				case Client::ERectTransform::RB:	pUI->Set_RectPos(Get_RB());
					break;
				default:
					break;
				}
			}
		}
	}
}

void CCanvas::Update_Priority(const _float fTimeDelta)
{
	Transmit_for_UI();
	Super::Update_Priority(fTimeDelta);
}

void CCanvas::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
}

void CCanvas::Update_Late(const _float fTimeDelta)
{
	Calc_HitUpdate();
	Super::Update_Late(fTimeDelta);
}

void CCanvas::Ready_Before_Render(const _float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);
}

HRESULT CCanvas::Render()
{
	return S_OK;
}

HRESULT CCanvas::Ready_Components(CANVAS_DESC* pDesc)
{
	return S_OK;
}

HRESULT CCanvas::Bind_ShaderResources()
{
	return S_OK;
}

void CCanvas::Calc_HitUpdate()
{
	/* 누른 순간 */
	if (MOUSE_LBUTTON_DOWN)
	{
		m_pCaptureUI = Calc_TopUI();
		if (nullptr != m_pCaptureUI)
			Engine_Utils::Add_Flag(m_pCaptureUI->Get_InteractState_Ref(), CUIObject::EInteractState::PRESS_ENTER);
	}
	/* 누르고 있을 때 */
	else if (MOUSE_LBUTTON_HOLD)
	{
		if (nullptr != m_pCaptureUI)
		{
			if (m_pCaptureUI->Calc_HitEvent())
			{
				if (!m_isPreUIPressing)
				{
					Engine_Utils::Add_Flag(m_pCaptureUI->Get_InteractState_Ref(), CUIObject::EInteractState::PRESS_ENTER);
					m_isPreUIPressing = TRUE;
				}
				else
				{
					Engine_Utils::Add_Flag(m_pCaptureUI->Get_InteractState_Ref(), CUIObject::EInteractState::PRESSING);
				}
			}
			else
			{
				if (m_isPreUIPressing)
				{
					Engine_Utils::Add_Flag(m_pCaptureUI->Get_InteractState_Ref(), CUIObject::EInteractState::PRESS_EXIT);
					m_isPreUIPressing = FALSE;
				}
			}
		}
	}
	/* 땐 순간 */
	else if (MOUSE_LBUTTON_UP)
	{
		if (nullptr != m_pCaptureUI)
		{
			if (m_pCaptureUI->Calc_HitEvent())
			{
				Engine_Utils::Add_Flag(m_pCaptureUI->Get_InteractState_Ref(), CUIObject::EInteractState::CLICKED);
			}
		}
		m_pCaptureUI = nullptr;
	}
	/* 안 누르고 있을 때*/
	else
	{
		CGenericUI* pUI = Calc_TopUI();
		/* 마우스랑 겹치는 UI가 없다 */
		if (nullptr == pUI)
		{
			/* 호버링중인 UI가 있다 */
			if (nullptr != m_pHoveringUI)
			{
				Engine_Utils::Add_Flag(m_pHoveringUI->Get_InteractState_Ref(), CUIObject::EInteractState::HOVERING_EXIT);
				m_pHoveringUI = nullptr;
			}
		}
		/* 마우스랑 겹치는 UI가 있다 */
		else
		{
			if (nullptr == m_pHoveringUI)
			{
				m_pHoveringUI = pUI;
				Engine_Utils::Add_Flag(m_pHoveringUI->Get_InteractState_Ref(), CUIObject::EInteractState::HOVERING_ENTER);
			}
			else
			{
				if (m_pHoveringUI != pUI)
				{
					Engine_Utils::Add_Flag(m_pHoveringUI->Get_InteractState_Ref(), CUIObject::EInteractState::HOVERING_EXIT);
					Engine_Utils::Add_Flag(pUI->Get_InteractState_Ref(), CUIObject::EInteractState::HOVERING_ENTER);
					m_pHoveringUI = pUI;
				}
			}
		}
	}
}

CGenericUI* CCanvas::Calc_TopUI()
{
	CGenericUI* pTopUI = { nullptr };

	for (CUILayer* pLayer : m_vecUILayers)
	{
		if (!pLayer->IsVisible())
			continue;

		auto* vecUI = pLayer->Get_UIVector();
		for (CGenericUI* pUI : *vecUI)
		{
			/* UI안에 마우스가 있으면 TRUE */
			if (pUI->Calc_HitEvent())
			{
				if (nullptr == pTopUI)
					pTopUI = pUI;
				else
				{
					if (pTopUI->Get_PosZ() < pUI->Get_PosZ())
						pTopUI = pUI;
				}
			}
		}
	}
	return pTopUI;
}

CCanvas* CCanvas::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CCanvas* pInstance = new CCanvas(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CCanvas::Create, Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CCanvas::Clone(void* pArg)
{
	CCanvas* pInstance = new CCanvas(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CCanvas::Clone, Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCanvas::Free()
{
	Super::Free();
}
