#include "pch.h"
#include "Canvas.h"
#include "Client_Defines.h"
#include <iostream>
//=================
// Component
//=================
#include "Texture.h"
#include "Shader.h"
#include "VIBuffer_Rect_Tex.h"

#include "GenericUI.h"

#include "UI_Manager.h"
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
	m_strName = pDesc->strName;
	m_vecChildPrefabTag = (pDesc->vecPrefabs);

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
	for (auto* pUI : m_vecUI)
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
	if (Check_Dead())
		All_Dead();

	Super::Ready_Before_Render(fTimeDelta);
}

HRESULT CCanvas::Render()
{
	return S_OK;
}

void CCanvas::OnCanvasEvent(ETriggerEventType eEvent, CGenericUI* pSender)
{
	for (auto* pUI : m_vecUI)
	{
		if (nullptr == pUI)
			continue;
		pUI->OnUIEvent(eEvent, pSender);
	}
}

_bool CCanvas::Check_FinEvent()
{
	for (auto* pUI : m_vecUI)
	{
		if (!pUI->Get_FinEvent())
			return false;
	}
	return true;
}

HRESULT CCanvas::Ready_Prefab(_uint iPoolLevel, _uint iSpawnLevel)
{
	for (const auto& wstr : m_vecChildPrefabTag)
	{
		m_pGameInstance->Request_AddObject(iPoolLevel, wstr, iSpawnLevel, &m_pPrefabData,
			[this, iSpawnLevel](CGameObject* p)
			{
				auto* pUI = static_cast<CGenericUI*>(p);
				if (nullptr == pUI)
					return;
				(this->Get_UIVector())->push_back(pUI);
				CUI_Manager::GetInstance()->Add_VecGenericUICache(iSpawnLevel,pUI);
				CUI_Manager::GetInstance()->Request_SortUI();
			});
	}
	m_isAllDead = false;
	return S_OK;
}

_bool CCanvas::Check_Dead()
{
	if (m_vecUI.empty())
		return false;

	for (auto* pUI : m_vecUI)
	{
		if (!pUI->IsDead())
			return false;
	}
	 return m_isAllDead = true;
}

void CCanvas::All_Dead()
{
	if (!m_isAllDead)
		return;

	for (auto* pUI : m_vecUI)
	{
		pUI->Set_Dead();
	}

	CUI_Manager::GetInstance()->Request_Clear_DeadUI();
	m_vecUI.clear();
	Set_Dead();
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
	if (!m_ArrReleasedUI.empty())
	{
		for (auto*& pUI : m_ArrReleasedUI)
		{
			if (nullptr != pUI)
			{
				pUI->Get_InteractState_Ref() = EUIEvent_Flag::NONE;
				pUI = nullptr;
			}
		}
	}

	/* Trigger 이벤트 소비 */
	if (nullptr != m_pCaptureUI)
	{
		Engine_Utils::RemoveSoft_Flag(m_pCaptureUI->Get_InteractState_Ref(), EUIEvent_Flag::PRESS_ENTER);
		Engine_Utils::RemoveSoft_Flag(m_pCaptureUI->Get_InteractState_Ref(), EUIEvent_Flag::PRESS_EXIT);
		m_pCaptureUI->Get_InteractState_Ref() = EUIEvent_Flag::NONE;
	}
	if (nullptr != m_pHoveringUI)
	{
		Engine_Utils::RemoveSoft_Flag(m_pHoveringUI->Get_InteractState_Ref(), EUIEvent_Flag::HOVER_ENTER);
		Engine_Utils::RemoveSoft_Flag(m_pHoveringUI->Get_InteractState_Ref(), EUIEvent_Flag::HOVER_EXIT);
		m_pHoveringUI->Get_InteractState_Ref() = EUIEvent_Flag::NONE;
	}

	/* 누른 순간 */
	if (MOUSE_LBUTTON_DOWN)
	{
		/* 눌린곳에 있는 UI중 가장 위에 있는 애 */
		m_pCaptureUI = Calc_TopUI();
		if (nullptr != m_pCaptureUI)
		{
			Engine_Utils::Add_Flag(m_pCaptureUI->Get_InteractState_Ref(), EUIEvent_Flag::PRESS_ENTER);
			m_isPreUIPressing = TRUE;
		}
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
					Engine_Utils::Add_Flag(m_pCaptureUI->Get_InteractState_Ref(), EUIEvent_Flag::PRESS_ENTER);
					m_isPreUIPressing = TRUE;
				}
				else
				{
					Engine_Utils::Add_Flag(m_pCaptureUI->Get_InteractState_Ref(), EUIEvent_Flag::PRESSING);
				}
			}
			else
			{
				if (m_isPreUIPressing)
				{
					m_pCaptureUI->Get_InteractState_Ref() = EUIEvent_Flag::NONE;
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
			/* 땠을 때 동일한 UI면 */
			if (m_pCaptureUI->Calc_HitEvent())
			{
				Engine_Utils::Add_Flag(m_pCaptureUI->Get_InteractState_Ref(), EUIEvent_Flag::PRESS_EXIT);
				const uint32_t CaptureUI = 0u;
				m_ArrReleasedUI[CaptureUI] = m_pCaptureUI;
				m_pCaptureUI = nullptr;
			}
			else
			{
				m_pCaptureUI->Get_InteractState_Ref() = EUIEvent_Flag::NONE;
				m_pCaptureUI = nullptr;
			}
			m_isPreUIPressing = FALSE;
		}
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
				Engine_Utils::Add_Flag(m_pHoveringUI->Get_InteractState_Ref(), EUIEvent_Flag::HOVER_EXIT);
				const uint32_t HoverUI = 1u;
				m_ArrReleasedUI[HoverUI] = m_pHoveringUI;
				m_pHoveringUI = nullptr;
			}
		}
		/* 마우스랑 겹치는 UI가 있다 */
		else
		{
			if (nullptr == m_pHoveringUI)
			{
				m_pHoveringUI = pUI;
				Engine_Utils::Add_Flag(m_pHoveringUI->Get_InteractState_Ref(), EUIEvent_Flag::HOVER_ENTER);
			}
			else
			{
				if (m_pHoveringUI != pUI)
				{
					Engine_Utils::Add_Flag(m_pHoveringUI->Get_InteractState_Ref(), EUIEvent_Flag::HOVER_EXIT);
					Engine_Utils::Add_Flag(pUI->Get_InteractState_Ref(), EUIEvent_Flag::HOVER_ENTER);
					const uint32_t HoverUI = 1u;
					m_ArrReleasedUI[HoverUI] = m_pHoveringUI;
					m_pHoveringUI = pUI;
				}
				else
				{
					Engine_Utils::Add_Flag(m_pHoveringUI->Get_InteractState_Ref(), EUIEvent_Flag::HOVERING);
				}
			}
		}
	}
}

CGenericUI* CCanvas::Calc_TopUI()
{
	CGenericUI* pTopUI = { nullptr };

	for (CGenericUI* pUI : m_vecUI)
	{
		if (pUI->Calc_HitEvent())
		{
			if (nullptr == pTopUI)
			{
				pTopUI = pUI;
			}
			else
			{
				if (pTopUI->Get_PosZ() > pUI->Get_PosZ())
					pTopUI = pUI;
			}
		}
	}
	return pTopUI;
}

HRESULT CCanvas::Spawn_FromPool(void* pArg)
{
	if (pArg)
		return E_FAIL;

	if (FAILED(Super::Spawn_FromPool(pArg)))
		return E_FAIL;

	m_pPrefabData = *static_cast<UI_PREFAB_DATA*>(pArg);


	return S_OK;
}

HRESULT CCanvas::Despawn_FromPool()
{
	if (FAILED(Super::Despawn_FromPool()))
		return E_FAIL;

	return S_OK;
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
