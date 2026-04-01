#include "pch.h"
#include "UICommunity_Text.h"
#include "Client_Defines.h"
#include "Client_EventDefine.h"

//=================
// Component
//=================
#include "IInteractable.h"
#include "WorldUI_Component.h"
#include "MyStat.h"
#include "Texture.h"
#include "Shader.h"
#include "VIBuffer_Rect_Tex.h"
#include "GameInstance.h"
#include "QuestManager.h"
#include <UI_Manager.h>

CUICommunity_Text::CUICommunity_Text(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	:CUIText(pDevice, pDeviceContext)
{
}

CUICommunity_Text::CUICommunity_Text(const CUICommunity_Text& rhs)
	:CUIText(rhs)
{
}

HRESULT CUICommunity_Text::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUICommunity_Text::Initialize(void* pArg)
{
	COMMUNITY_TEXT_DESC* pDesc = static_cast<COMMUNITY_TEXT_DESC*>(pArg);
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;
	if (FAILED(Ready_Components(pDesc)))
		return E_FAIL;
	return S_OK;
}

HRESULT CUICommunity_Text::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;

	if (FAILED(Attach_Personal_Info()))
		return E_FAIL;
	return S_OK;
}


void CUICommunity_Text::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CUICommunity_Text::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);

	Tick_By_Type(fTimeDelta);
}

void CUICommunity_Text::Update_Late(const _float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);
}

void CUICommunity_Text::Ready_Before_Render(const _float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);
	if (FAILED(Convert_Stat_To_Text()))
		return;
}

HRESULT CUICommunity_Text::Render()
{
	if (!m_isVisible)
		return S_OK;
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;
	if (FAILED(Super::Render()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUICommunity_Text::Ready_Components(COMMUNITY_TEXT_DESC* pDesc)
{
	if (FAILED(Super::Ready_Components(pDesc)))
		return E_FAIL;
	return S_OK;
}

HRESULT CUICommunity_Text::Bind_ShaderResources()
{
	CShader* pShader = Get_Component<CShader>();
	if (FAILED(Get_Component<CTransform>()->Bind_ShaderResource(pShader)))
		return E_FAIL;
	if (FAILED(Super::Bind_ShaderResources()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUICommunity_Text::Attach_Personal_Info()
{
	return S_OK;
}

HRESULT CUICommunity_Text::Convert_Stat_To_Text()
{
	return S_OK;
}

void CUICommunity_Text::Bind_Events()
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
		m_pGameInstance->Subscribe<CINEMATIC_START>(
			[this]()
			{
				this->Set_Invisible();
			}));

	m_vecEventHandles.push_back(
		m_pGameInstance->Subscribe<INTERACT_DETECT>([this](CGameObject* pObj)
			{			
				if (pObj->Get_ID() == ENUM_TO_UINT(EObjectEnumTag::Enum::NPC_CITIZEN))
					return;

				Set_Active(true);
				this->Set_Visible();

				this->m_wstrText = Engine_Utils::ToWString(pObj->Get_Name());
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

void CUICommunity_Text::Initialize_Visible_Event()
{
	Ready_Fade_Text(0.3f, 0.f, 1.f, m_fDelay);
}

void CUICommunity_Text::Initialize_InVisible_Event()
{
	Ready_Fade_Text(0.3f, 1.f, 0.f, m_fDelay);
}

_bool CUICommunity_Text::Tick_Visible_Event(const _float fTimeDelta)
{
	_bool isFade = Tick_Fade_Text(fTimeDelta);
	if (isFade)
	{
		return true;
	}
	return false;
}

_bool CUICommunity_Text::Tick_InVisible_Event(const _float fTimeDelta)
{
	_bool isFade = Tick_Fade_Text(fTimeDelta);

	if (isFade)
	{
		Set_Active(false);
		return true;
	}
	return false;
}

void CUICommunity_Text::Tick_By_Type(const _float fTimeDelta)
{
}

CUICommunity_Text* CUICommunity_Text::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CUICommunity_Text* pInstance = new CUICommunity_Text(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CUICommunity_Text::Create, Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUICommunity_Text::Clone(void* pArg)
{
	CUICommunity_Text* pInstance = new CUICommunity_Text(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CUICommunity_Text::Clone, Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUICommunity_Text::Free()
{
	Super::Free();
}
