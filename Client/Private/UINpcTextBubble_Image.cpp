#include "pch.h"
#include "UINpcTextBubble_Image.h"
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

CUINpcTextBubble_Image::CUINpcTextBubble_Image(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	:CUIDynamic_Image(pDevice, pDeviceContext)
{
}

CUINpcTextBubble_Image::CUINpcTextBubble_Image(const CUINpcTextBubble_Image& rhs)
	:CUIDynamic_Image(rhs)
{
}

HRESULT CUINpcTextBubble_Image::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUINpcTextBubble_Image::Initialize(void* pArg)
{
	NPC_TEXT_BUBBLE_IMAGE_DESC* pDesc = static_cast<NPC_TEXT_BUBBLE_IMAGE_DESC*>(pArg);
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;
	if (FAILED(Ready_Components(pDesc)))
		return E_FAIL;
	return S_OK;
}


HRESULT CUINpcTextBubble_Image::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;
	if (FAILED(Attach_Personal_Info()))
		return E_FAIL;
	return S_OK;
}

void CUINpcTextBubble_Image::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CUINpcTextBubble_Image::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
	Tick_By_Type(fTimeDelta);
}

void CUINpcTextBubble_Image::Update_Late(const _float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);
}

void CUINpcTextBubble_Image::Ready_Before_Render(const _float fTimeDelta)
{
	Move_Rotate(m_fRotate);
	Super::Ready_Before_Render(fTimeDelta);
}

HRESULT CUINpcTextBubble_Image::Render()
{
	if (!m_isVisible)
		return S_OK;
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;
	if (FAILED(Super::Render()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUINpcTextBubble_Image::Ready_Components(NPC_TEXT_BUBBLE_IMAGE_DESC* pDesc)
{
	if (FAILED(Super::Ready_Components(pDesc)))
		return E_FAIL;
	return S_OK;
}

HRESULT CUINpcTextBubble_Image::Bind_ShaderResources()
{
	CShader* pShader = Get_Component<CShader>();
	if (FAILED(Get_Component<CTransform>()->Bind_ShaderResource(pShader)))
		return E_FAIL;
	if (FAILED(Super::Bind_ShaderResources()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUINpcTextBubble_Image::Attach_Personal_Info()
{
	if (m_isSpawned)
	{
		m_isSpawned = false;
		Set_Active(false);
	}
	return S_OK;
}

void CUINpcTextBubble_Image::Bind_Events()
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

	// ´ëÈ­ Event
	m_vecEventHandles.push_back(
		m_pGameInstance->Subscribe<DIALOGUE_BEGIN>([this](_int iId)
			{
				this->Set_Invisible();
			}));

	m_vecEventHandles.push_back(
		m_pGameInstance->Subscribe<INTERACT_DETECT>([this](CGameObject* pObj)
			{
				Set_Active(true);
				this->Set_Visible();
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

void CUINpcTextBubble_Image::Tick_By_Type(const _float fTimeDelta)
{
	switch (m_eDImageSubClass)
	{
	case DTO::EUIDImageSubClassType::NPC_TEXT_BUBBLE_BG:
		m_fWidth = m_pParentCanvasCache->Get_CommonParam_float()[FONTSIZE_X_SLOT];
		break;
	case DTO::EUIDImageSubClassType::NPC_TEXT_BUBBLE_BG_DOWN:
		
		break;
	}
}

void CUINpcTextBubble_Image::Initialize_Visible_Event()
{
	Ready_Fade(0.3f, 0.f, 1.f, m_fDelay);
}

_bool CUINpcTextBubble_Image::Tick_Visible_Event(const _float fTimeDelta)
{
	_bool isFade = Tick_Fade(fTimeDelta);
	if (isFade)
	{
		return true;
	}
	return false;
}

void CUINpcTextBubble_Image::Initialize_InVisible_Event()
{
}

_bool CUINpcTextBubble_Image::Tick_InVisible_Event(const _float fTimeDelta)
{
	return true;
}

HRESULT CUINpcTextBubble_Image::Spawn_FromPool(void* pArg)
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
		m_pTargetNPC = pTextBubble->pTarget;
		m_strSoundTag = pTextBubble->strSoundTag;

		m_isDeadRequest = false;
	}
	return S_OK;
}

HRESULT CUINpcTextBubble_Image::Despawn_FromPool()
{
	if (FAILED(Super::Despawn_FromPool()))
		return E_FAIL;

	m_isVisible = false;
	m_isVisibleTrigger = false;
	m_isPreVisible = false;
	return S_OK;
}

CUINpcTextBubble_Image* CUINpcTextBubble_Image::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CUINpcTextBubble_Image* pInstance = new CUINpcTextBubble_Image(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CUINpcTextBubble_Image::Create, Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUINpcTextBubble_Image::Clone(void* pArg)
{
	CUINpcTextBubble_Image* pInstance = new CUINpcTextBubble_Image(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CUINpcTextBubble_Image::Clone, Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUINpcTextBubble_Image::Free()
{
	Super::Free();
}
