#include "pch.h"
#include "GenericUI.h"
#include "Client_Defines.h"

//=================
// Component
//=================
#include "Texture.h"
#include "Shader.h"
#include "VIBuffer_Rect_Tex.h"
#include "UIAction_Registry.h"
#include "UIAction_Client.h"
#include "UITargetAction_Client.h"
#include "GameInstance.h"

CGenericUI::CGenericUI(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	:CUIObject(pDevice, pDeviceContext)
{
}

CGenericUI::CGenericUI(const CGenericUI& rhs)
	:CUIObject(rhs)
{
}

HRESULT CGenericUI::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;
	return S_OK;
}

HRESULT CGenericUI::Initialize(void* pArg)
{
	GENERIC_UI_DESC* pDesc = static_cast<GENERIC_UI_DESC*>(pArg);

	m_eRectTransformType = static_cast<ERectTransform>(pDesc->iRectTransformType);
	m_wstrTextureTag = pDesc->wstrTextureTag;
	m_iTextureIndex = pDesc->iTextureIndex;

	m_pParentCanvasCache = pDesc->pCanvasCache;
	m_pParentLayerCache = pDesc->pLayerCache;

	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components(pDesc)))
		return E_FAIL;

	m_pActionForMe = CUIAction_Client::Create(this);
	if (nullptr == m_pActionForMe)
		return E_FAIL;
	m_pActionForTarget = CUITargetAction_Client::Create(this);
	if (nullptr == m_pActionForTarget)
	{
		Safe_Release(m_pActionForMe);
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CGenericUI::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;

	m_iInteractState = static_cast<uint32_t>(DTO::EUIEvent_Flag::NONE);

	return S_OK;
}

void CGenericUI::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CGenericUI::Update(const _float fTimeDelta)
{
	m_vRenderPos = Vec3{ m_vRectPos.x + m_vMoveOffset.x + m_fX, m_vRectPos.y + m_vMoveOffset.y + m_fY, m_fZ };
	Move_Position(m_vRenderPos.x, m_vRenderPos.y, m_vRenderPos.z);

	m_tRenderRect.left		= static_cast<LONG>(m_vRenderPos.x - (m_fWidth * 0.5f));
	m_tRenderRect.right		= static_cast<LONG>(m_vRenderPos.x + (m_fWidth * 0.5f));
	m_tRenderRect.top		= static_cast<LONG>(m_vRenderPos.y - (m_fHeight * 0.5f));
	m_tRenderRect.bottom	= static_cast<LONG>(m_vRenderPos.y + (m_fHeight * 0.5f));
	Super::Update(fTimeDelta);
}

void CGenericUI::Update_Late(const _float fTimeDelta)
{
	if (m_isDisable)
		return;

	Delay_Queue(fTimeDelta);

	Lerp_Movement(fTimeDelta);
	Return_Lerp_Movement(fTimeDelta);
	Fade(fTimeDelta);
}

void CGenericUI::Ready_Before_Render(const _float fTimeDelta)
{
	Acting_By_InteractState();
	Super::Ready_Before_Render(fTimeDelta);
}

HRESULT CGenericUI::Render()
{
	if (!m_isVisible)
		return S_OK;

	if (FAILED(Super::Render()))
		return E_FAIL;

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	Get_Component<CShader>()->Apply();
	Get_Component<CVIBuffer>()->Bind_Resource();
	Get_Component<CVIBuffer>()->Render();

	return S_OK;
}

_bool CGenericUI::Calc_HitEvent()
{
	if (::PtInRect(&m_tRenderRect, m_pGameInstance->Get_MousePos()))
		return TRUE;
	return FALSE;
}

void CGenericUI::Acting_By_InteractState()
{
	if (Engine_Utils::Has_Flag(m_iInteractState, DTO::EUIEvent_Flag::INVOKED))
	{
		Excute_Action(DTO::EUIEvent::INVOKED);
		Engine_Utils::RemoveSoft_Flag(m_iInteractState, DTO::EUIEvent_Flag::INVOKED);
		return;
	}

	if (m_iInteractState == DTO::EUIEvent_Flag::NONE)
		Excute_Action(DTO::EUIEvent::NONE);
	else
	{
		if (Engine_Utils::Has_Flag(m_iInteractState, DTO::EUIEvent_Flag::PRESS_ENTER))
		{
			Excute_Action(DTO::EUIEvent::PRESS_ENTER);
		}
		else if (Engine_Utils::Has_Flag(m_iInteractState, DTO::EUIEvent_Flag::PRESS_EXIT))
		{
			Excute_Action(DTO::EUIEvent::PRESS_EXIT);
		}
		else if (Engine_Utils::Has_Flag(m_iInteractState, DTO::EUIEvent_Flag::HOVER_ENTER))
		{
			Excute_Action(DTO::EUIEvent::HOVER_ENTER);
		}
		else if (Engine_Utils::Has_Flag(m_iInteractState, DTO::EUIEvent_Flag::HOVER_EXIT))
		{
			Excute_Action(DTO::EUIEvent::HOVER_EXIT);
		}

		if (Engine_Utils::Has_Flag(m_iInteractState, DTO::EUIEvent_Flag::PRESSING))
		{
			Excute_Action(DTO::EUIEvent::PRESSING);
		}
		else if (Engine_Utils::Has_Flag(m_iInteractState, DTO::EUIEvent_Flag::HOVERING))
		{
			Excute_Action(DTO::EUIEvent::HOVERING);
		}
	}
}

HRESULT CGenericUI::Bind_Action(const DTO::TUI_EventBindData& data)
{
	const size_t index = ENUM_TO_SZET(data.eEvent);
	if (index >= m_vecBindingActions.size())
		return E_FAIL;

	DTO::TUI_EventBindData Desc = {};
	Desc.strOwnerTag			= m_strName;
	Desc.eAction				= data.eAction;
	Desc.eEvent					= data.eEvent;
	Desc.Params					= data.Params;
	Desc.strTargetTag			= data.strTargetTag;
	m_vecBindingActionData[index].push_back(Desc);
	auto Func = m_pGameInstance->Get_UIAction_Registry()->Build_Action(data.eAction, data.Params);
	if (!Func)
		return E_FAIL;
	m_vecBindingActions[index].push_back(std::move(Func));
	return S_OK;
}

HRESULT CGenericUI::Bind_Action(DTO::EUIEvent EventType, DTO::EUIAction ActType, const json& params)
{
	const size_t index = ENUM_TO_SZET(EventType);
	if (index >= m_vecBindingActions.size())
		return E_FAIL;

	DTO::TUI_EventBindData Desc = {};
	Desc.strOwnerTag = m_strName;
	Desc.eAction = ActType;
	Desc.eEvent = EventType;
	Desc.Params = params;
	Desc.strTargetTag = "";
	m_vecBindingActionData[index].push_back(Desc);
	auto Func = m_pGameInstance->Get_UIAction_Registry()->Build_Action(ActType, params);
	if (!Func)
		return E_FAIL;
	m_vecBindingActions[index].push_back(std::move(Func));
	return S_OK;
}

HRESULT CGenericUI::Remove_Action(DTO::EUIEvent EventType, DTO::EUIAction ActType)
{
	const size_t EventIndex = ENUM_TO_SZET(EventType);
	if (EventIndex >= m_vecBindingActionData.size())
		return E_FAIL;

	_bool isRemoved = { FALSE };
	for (auto iter = m_vecBindingActionData[EventIndex].begin(); iter != m_vecBindingActionData[EventIndex].end(); iter++)
	{
		if (iter->eAction == ActType)
		{
			m_vecBindingActionData[EventIndex].erase(iter);
			isRemoved = TRUE;
			break;
		}
	}
	if (!isRemoved)
	{
		MSG_BOX("CToolUI::Remove_Action, No Action with Match strActionKey");
		return E_FAIL;
	}
	m_vecBindingActions[EventIndex].clear();
	if (FAILED(ReBind_Action()))
		return E_FAIL;
	return S_OK;
}

HRESULT CGenericUI::Excute_Action(DTO::EUIEvent EventType)
{
	if (nullptr == m_pActionForMe)
		return E_FAIL;

	size_t index = ENUM_TO_SZET(EventType);
	if (index >= m_vecBindingActions.size())
		return E_FAIL;

	for (auto& fn : m_vecBindingActions[index])
		fn(m_pActionForMe, m_pActionForTarget);
	return S_OK;
}

HRESULT CGenericUI::Excute_Specific_Action(DTO::EUIEvent EventType, DTO::EUIAction eAction)
{
	uint32_t index = {};
	for (const auto& pActData : m_vecBindingActionData[ENUM_TO_UINT(EventType)])
	{
		if (pActData.eAction == eAction)
		{
			m_vecBindingActions[ENUM_TO_UINT(EventType)][index](m_pActionForMe, m_pActionForTarget);
			return S_OK;
		}
		index++;
	}
	return E_FAIL;
}

HRESULT CGenericUI::ReBind_Action()
{
	for (uint32_t i = 0; i < m_vecBindingActionData.size(); ++i)
	{
		m_vecBindingActions[i].clear();
		for (auto& data : m_vecBindingActionData[i])
		{
			auto Func = m_pGameInstance->Get_UIAction_Registry()->Build_Action(data.eAction, data.Params);
			if (!Func)
				return E_FAIL;
			m_vecBindingActions[i].push_back(std::move(Func));
		}
	}
	return S_OK;
}

void CGenericUI::Delay_Queue(const _float fTimeDelta)
{
	for (size_t i = 0; i < m_vecActionQueue.size(); )
	{
		m_vecActionQueue[i].fRemain -= fTimeDelta;
		if (m_vecActionQueue[i].fRemain <= 0.f)
		{
			m_vecActionQueue[i].Func();
			m_vecActionQueue[i] = std::move(m_vecActionQueue.back());
			m_vecActionQueue.pop_back();

			continue;
		}
		i++;
	}
}

void CGenericUI::Push_DelayAction(const _float fDelay, std::function<void()>&& Func)
{
	if (0.f >= fDelay)
	{
		Func();
		return;
	}
	else
	{
		m_vecActionQueue.push_back(SCHEDULE_DESC{ fDelay, std::move(Func) });
		return;
	}

}

HRESULT CGenericUI::Ready_Components(GENERIC_UI_DESC* pDesc)
{
	if (FAILED(Add_Component<CTexture>(ENUM_TO_UINT(ELevelType::STATIC), m_wstrTextureTag, pDesc)))
		return E_FAIL;
	if (FAILED(Add_Component<CShader>(0, L"Prototype_Component_Shader_VtxPosTex", pDesc)))
		return E_FAIL;
	if (FAILED(Add_Component<CVIBuffer_Rect_Tex>(0, L"Prototype_Component_VIBuffer_Rect_Tex", pDesc)))
		return E_FAIL;
	return S_OK;
}

HRESULT CGenericUI::Bind_ShaderResources()
{
	CShader* pShader = Get_Component<CShader>();
	pShader->Set_Pass(1);

	if (FAILED(Get_Component<CTransform>()->Bind_ShaderResource(pShader)))
		return E_FAIL;
	if (FAILED(Get_Component<CTexture>()->Bind_ShaderResource(pShader, m_iTextureIndex)))
		return E_FAIL;
	if (FAILED(pShader->Get_Variable("g_fHpBarRatio")->SetRawValue(&m_fFade_ResultAlpha, 0, sizeof(_float))))
		return E_FAIL;
	return S_OK;
}

void CGenericUI::Start_Lerp_Movement(const Vec3& vTargetPos, const _float fTargetAlpha, const _float& fDuration, _bool isPin)
{
	m_vLerpMovement_StartPos = m_vRenderPos; /* vRenderPos -> Local */
	m_vLerpMovement_TargetPos = vTargetPos; /* vTargetPos -> Local */

	m_fLerpMovement_TargetAlpha = fTargetAlpha;
	m_fLerpMovement_Duration = fDuration;
	m_fLerpMovement_TimeAcc = 0.f;
	m_isPlaying_Lerp_Movement = true;
	m_isLerpMovement_Pin = isPin;
}

void CGenericUI::Start_Return_Lerp_Movement()
{
	m_vLerpMovement_StartPos = m_vMoveOffset;
	m_vLerpMovement_TargetPos = Vec3{ 0.f, 0.f, 0.f };

	m_fLerpMovement_TimeAcc = 0.f;
	m_isPlaying_Return_Lerp_Movement = true;
}

void CGenericUI::Lerp_Movement(const _float fTimeDelta)
{
	if (!m_isPlaying_Lerp_Movement || m_isPlaying_Return_Lerp_Movement)
		return;

	m_isAction = true; /* 咀记 吝 */

	if (m_fLerpMovement_Duration <= 0.f)
	{
		m_vMoveOffset = m_vLerpMovement_TargetPos - m_vLerpMovement_StartPos;
		m_isPlaying_Lerp_Movement = false;
		m_isAction = false; /* 咀记 场 */
		return;
	}

	m_fLerpMovement_TimeAcc += fTimeDelta;

	_float t = m_fLerpMovement_TimeAcc / m_fLerpMovement_Duration;
	if (t >= 1.f) t = 1.f;
	else if (t <= 0.f) t = 0.f;

	_float s = t;
	if (m_fLerpMovement_TargetAlpha > 0.f)
		s = 1.f - powf(1.f - t, m_fLerpMovement_TargetAlpha);

	m_vMoveOffset = (m_vLerpMovement_TargetPos - m_vLerpMovement_StartPos) * s;

	if (t >= 1.f)
	{
		if (m_isLerpMovement_Pin)
		{
			m_vMoveOffset = m_vLerpMovement_TargetPos - m_vLerpMovement_StartPos;
			m_isMoved = TRUE;
		}
		else
		{
			Start_Return_Lerp_Movement();
		}

		m_isAction = false; /* 咀记 场 */
		m_isPlaying_Lerp_Movement = false;
	}
}

void CGenericUI::Return_Lerp_Movement(const _float fTimeDelta)
{
	if (!m_isPlaying_Return_Lerp_Movement || m_isPlaying_Lerp_Movement)
		return;

	m_isAction = true; /* 咀记 吝 */

	if (m_fLerpMovement_Duration <= 0.f)
	{
		m_vMoveOffset = Vec3{ 0.f, 0.f, 0.f };
		m_isPlaying_Return_Lerp_Movement = false;
		m_isAction = false; /* 咀记 场 */
		return;
	}

	m_fLerpMovement_TimeAcc += fTimeDelta;

	_float t = m_fLerpMovement_TimeAcc / m_fLerpMovement_Duration;
	if (t >= 1.f) t = 1.f;
	else if (t <= 0.f) t = 0.f;

	_float s = t;
	if (m_fLerpMovement_TargetAlpha > 0.f)
		s = 1.f - powf(1.f - t, m_fLerpMovement_TargetAlpha);

	m_vMoveOffset = m_vLerpMovement_StartPos + (m_vLerpMovement_TargetPos - m_vLerpMovement_StartPos) * s;

	if (t >= 1.f)
	{
		m_vMoveOffset = Vec3{ 0.f, 0.f, 0.f };
		m_isPlaying_Return_Lerp_Movement = false;
		m_isAction = false; /* 咀记 场 */
	}
}

void CGenericUI::Start_Fade(const _float fStartAlpha, const _float fTargetAlpha, const _float fDuration)
{
	m_fFade_StartAlpha = fStartAlpha;
	m_fFade_ResultAlpha = fStartAlpha;
	m_fFade_TargetAlpha = fTargetAlpha;
	m_fFade_Duration = fDuration;
	m_fFade_TimeAcc = 0.f;
	m_isPlaying_Fade = true;

	Get_Component<CShader>()->Set_Pass(2);
}

void CGenericUI::Fade(const _float fTimeDelta)
{
	if (!m_isPlaying_Fade)
		return;

	m_isAction = true;

	if (m_fFade_Duration <= 0.f)
	{
		m_fFade_ResultAlpha = m_fFade_TargetAlpha;
		m_isPlaying_Fade = false;
		m_isAction = false;

		return;
	}

	m_fFade_TimeAcc += fTimeDelta;
	_float t = m_fFade_TimeAcc / m_fFade_Duration;

	if (t >= 1.f)
	{
		t = 1.f;
		m_isAction = false;
		m_isPlaying_Fade = false;
	}

	m_fFade_ResultAlpha = m_fFade_StartAlpha + (m_fFade_TargetAlpha - m_fFade_StartAlpha) * t;
}

CGenericUI* CGenericUI::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CGenericUI* pInstance = new CGenericUI(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CGenericUI::Create, Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CGenericUI::Clone(void* pArg)
{
	CGenericUI* pInstance = new CGenericUI(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CGenericUI::Clone, Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CGenericUI::Free()
{
	Safe_Release(m_pActionForMe);
	Safe_Release(m_pActionForTarget);
	Super::Free();
}
