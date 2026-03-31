#include "pch.h"
#include "UINpcTextBubble_Text.h"
#include "Client_Defines.h"
#include "Client_EventDefine.h"

//=================
// Component
//=================
#include "Canvas.h"
#include "WorldUI_Component.h"
#include "Texture.h"
#include "Shader.h"
#include "VIBuffer_Rect_Tex.h"
#include "UI_Manager.h"
#include "GameInstance.h"


#define FONTSIZE_X_SLOT 0
#define FONTSIZE_Y_SLOT 1

CUINpcTextBubble_Text::CUINpcTextBubble_Text(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	:CUIText(pDevice, pDeviceContext)
{
}

CUINpcTextBubble_Text::CUINpcTextBubble_Text(const CUINpcTextBubble_Text& rhs)
	:CUIText(rhs)
{
}

HRESULT CUINpcTextBubble_Text::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUINpcTextBubble_Text::Initialize(void* pArg)
{
	NPC_TEXT_BUBBLE_TEXT_DESC* pDesc = static_cast<NPC_TEXT_BUBBLE_TEXT_DESC*>(pArg);
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;
	if (FAILED(Ready_Components(pDesc)))
		return E_FAIL;
	return S_OK;
}

HRESULT CUINpcTextBubble_Text::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;

	if (FAILED(Attach_Personal_Info()))
		return E_FAIL;

	if (m_isSpawned)
	{
		m_isSpawned = false;
		Set_Active(false);
	}
	return S_OK;
}


void CUINpcTextBubble_Text::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CUINpcTextBubble_Text::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
	Tick_By_Type(fTimeDelta);
}

void CUINpcTextBubble_Text::Update_Late(const _float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);
}

void CUINpcTextBubble_Text::Ready_Before_Render(const _float fTimeDelta)
{
	if (FAILED(Convert_Stat_To_Text()))
		return;

	Super::Ready_Before_Render(fTimeDelta);

	if (m_eTextSubClassType == DTO::EUITextSubClassType::NPC_TEXT_BUBBLE_CONTENT_TEXT)
	{
		_float fx = m_pParentCanvasCache->Get_CommonParam_float_Ref()[FONTSIZE_X_SLOT];
		_float fy = m_pParentCanvasCache->Get_CommonParam_float_Ref()[FONTSIZE_Y_SLOT];
		m_vFontPos.x -= fx * 0.5f;
		m_vFontPos.y += 5.f;
	}
}

HRESULT CUINpcTextBubble_Text::Render()
{
	if (!m_isVisible)
		return S_OK;
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;
	if (FAILED(Super::Render()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUINpcTextBubble_Text::Ready_Components(NPC_TEXT_BUBBLE_TEXT_DESC* pDesc)
{
	if (FAILED(Super::Ready_Components(pDesc)))
		return E_FAIL;
	return S_OK;
}

HRESULT CUINpcTextBubble_Text::Bind_ShaderResources()
{
	CShader* pShader = Get_Component<CShader>();
	if (FAILED(Get_Component<CTransform>()->Bind_ShaderResource(pShader)))
		return E_FAIL;
	if (FAILED(Super::Bind_ShaderResources()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUINpcTextBubble_Text::Attach_Personal_Info()
{
	switch (m_eTextSubClassType)
	{
	case DTO::EUITextSubClassType::NPC_TEXT_BUBBLE_NAME_TEXT:
		break;
	case DTO::EUITextSubClassType::NPC_TEXT_BUBBLE_CONTENT_TEXT:
		break;
	}

	return S_OK;
}

HRESULT CUINpcTextBubble_Text::Convert_Stat_To_Text()
{
	switch (m_eTextSubClassType)
	{
	case DTO::EUITextSubClassType::NPC_TEXT_BUBBLE_NAME_TEXT:
		break;
	case DTO::EUITextSubClassType::NPC_TEXT_BUBBLE_CONTENT_TEXT:
		break;
	}
	return S_OK;
}

void CUINpcTextBubble_Text::Bind_Events()
{
	Super::Bind_Events();

	m_vecEventHandles.push_back(
		m_pUIManager->Get_UIEvents().Subscribe([this](const UIEVENT_DESC& Desc)
			{
				if (EUIEventID::MENU_OPEN == Desc.eEventID)
				{
					this->Set_Invisible();
				}
			}));

	m_vecEventHandles.push_back(
		m_pGameInstance->Subscribe<INTERACT_DETECT>([this](CGameObject* pObj)
			{
				if (m_pTargetNPC == pObj)
				{
					Set_Active(true);
					this->Set_Visible();
				}
			}));

	// ´ëÈ­ Event
	m_vecEventHandles.push_back(
		m_pGameInstance->Subscribe<DIALOGUE_BEGIN>([this](_int iId)
			{
				this->Set_Invisible();
			}));

	m_vecEventHandles.push_back(
		m_pGameInstance->Subscribe<INTERACT_LOST>([this](CGameObject* pObj)
			{
				this->Set_Invisible();
			}));

	m_vecEventHandles.push_back(
		m_pGameInstance->Subscribe<INTERACT_ENTER>([this](CGameObject* pObj)
			{
				this->Set_Invisible();
			}));
}

void CUINpcTextBubble_Text::Tick_By_Type(const _float fTimeDelta)
{
	switch (m_eTextSubClassType)
	{
	case DTO::EUITextSubClassType::NPC_TEXT_BUBBLE_NAME_TEXT:
		break;
	case DTO::EUITextSubClassType::NPC_TEXT_BUBBLE_CONTENT_TEXT:
	{
		m_wstrText = m_wstrNpcText;
		m_vFontSize = m_pGameInstance->Measure_Font(m_wstrFontTag, m_wstrNpcText.c_str());
		m_pParentCanvasCache->Get_CommonParam_float_Ref()[FONTSIZE_X_SLOT] = m_vFontSize.x;
		m_pParentCanvasCache->Get_CommonParam_float_Ref()[FONTSIZE_Y_SLOT] = m_vFontSize.y;
	}
		break;
	}
}

void CUINpcTextBubble_Text::Initialize_Visible_Event()
{
	Ready_Fade_Text(0.3f, 0.f, 0.7f, m_fDelay);
}

void CUINpcTextBubble_Text::Initialize_InVisible_Event()
{
}

_bool CUINpcTextBubble_Text::Tick_Visible_Event(const _float fTimeDelta)
{
	_bool isFade = Tick_Fade_Text(fTimeDelta);
	if (isFade)
	{
		return true;
	}
	return false;
}

_bool CUINpcTextBubble_Text::Tick_InVisible_Event(const _float fTimeDelta)
{
	Request_SetDead();
	return true;
}

HRESULT CUINpcTextBubble_Text::Spawn_FromPool(void* pArg)
{
	if (FAILED(Super::Spawn_FromPool(pArg)))
		return E_FAIL;

	UI_PREFAB_DATA* pDesc = static_cast<UI_PREFAB_DATA*>(pArg);
	if (auto* pTextBubble = std::get_if<UI_NPC_TEXT_BUBBLE_PREFAB_DATA>(&pDesc->Data))
	{
		auto* pComp = Get_Script_Component(L"WorldUIComponent");
		if (nullptr == pComp)
			return E_FAIL;

		m_pWorldUIComp = static_cast<CWorldUI_Component*>(pComp);
		if (nullptr == m_pWorldUIComp)
			return E_FAIL;

		m_pWorldUIComp->Set_Target(pTextBubble->pTarget);
		m_pWorldUIComp->Set_TargetWorldOffset(pTextBubble->vOffset);
		m_pTargetNPC	= pTextBubble->pTarget;
		m_wstrNpcText	= pTextBubble->wstrContents;

		switch (m_eTextSubClassType)
		{
		case DTO::EUITextSubClassType::NPC_TEXT_BUBBLE_NAME_TEXT:
			m_wstrText = Engine_Utils::ToWString(pTextBubble->pTarget->Get_Name());
			break;
		case DTO::EUITextSubClassType::NPC_TEXT_BUBBLE_CONTENT_TEXT:
			m_wstrText = m_wstrNpcText;
			m_vFontSize = m_pGameInstance->Measure_Font(m_wstrFontTag, m_wstrNpcText.c_str());
			break;
		}

		m_isDeadRequest = false;
	}

	m_isSpawned = true;
	return S_OK;
}

HRESULT CUINpcTextBubble_Text::Despawn_FromPool()
{
	if (FAILED(Super::Despawn_FromPool()))
		return E_FAIL;

	m_isVisible = false;
	m_isVisibleTrigger = false;
	m_isPreVisible = false;
	return S_OK;
}

CUINpcTextBubble_Text* CUINpcTextBubble_Text::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CUINpcTextBubble_Text* pInstance = new CUINpcTextBubble_Text(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CUINpcTextBubble_Text::Create, Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUINpcTextBubble_Text::Clone(void* pArg)
{
	CUINpcTextBubble_Text* pInstance = new CUINpcTextBubble_Text(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CUINpcTextBubble_Text::Clone, Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUINpcTextBubble_Text::Free()
{
	Super::Free();
}
