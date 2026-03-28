#include "pch.h"
#include "UIConversation_Image.h"
#include "Client_Defines.h"
#include "Client_EventDefine.h"
//=================
// Component
//=================
#include "WorldUI_Component.h"
#include "Texture.h"
#include "Shader.h"
#include "VIBuffer_Rect_Tex.h"
#include "UI_Manager.h"
#include "UITutorial_Manager.h"
#include "GameInstance.h"

CUIConversation_Image::CUIConversation_Image(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	:CUIDynamic_Image(pDevice, pDeviceContext)
{
}

CUIConversation_Image::CUIConversation_Image(const CUIConversation_Image& rhs)
	:CUIDynamic_Image(rhs)
{
}

HRESULT CUIConversation_Image::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIConversation_Image::Initialize(void* pArg)
{
	CONVERSATION_IMAGE_DESC* pDesc = static_cast<CONVERSATION_IMAGE_DESC*>(pArg);
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;
	if (FAILED(Ready_Components(pDesc)))
		return E_FAIL;

	return S_OK;
}


HRESULT CUIConversation_Image::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;
	if (FAILED(Attach_Personal_Info()))
		return E_FAIL;

	return S_OK;
}

void CUIConversation_Image::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CUIConversation_Image::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
	Tick_By_Type(fTimeDelta);
}

void CUIConversation_Image::Update_Late(const _float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);
}

void CUIConversation_Image::Ready_Before_Render(const _float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);
}

HRESULT CUIConversation_Image::Render()
{
	if (!m_isVisible)
		return S_OK;
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;
	if (FAILED(Super::Render()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIConversation_Image::Ready_Components(CONVERSATION_IMAGE_DESC* pDesc)
{
	if (FAILED(Super::Ready_Components(pDesc)))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIConversation_Image::Bind_ShaderResources()
{
	CShader* pShader = Get_Component<CShader>();
	if (FAILED(Get_Component<CTransform>()->Bind_ShaderResource(pShader)))
		return E_FAIL;
	if (FAILED(Super::Bind_ShaderResources()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIConversation_Image::Attach_Personal_Info()
{
	m_vOriginColor = m_vColorTint;
	m_vOriginGradColor = m_vGradiantColorTint;

	return S_OK;
}

void CUIConversation_Image::Bind_Events()
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
		m_pGameInstance->Subscribe<DIALOGUE_BEGIN>([this](_int iId)
			{
				this->Set_Visible();
				switch (m_eDImageSubClass)
				{
				case DTO::EUIDImageSubClassType::CONVERSATION_DOWN:
					break;
				case DTO::EUIDImageSubClassType::CONVERSATION_BG:
				CUITutorial_Manager::GetInstance()->PlayerState_All_Lock();
					break;
				}
			}));
	m_vecEventHandles.push_back(
		m_pGameInstance->Subscribe<DIALOGUE_END>([this]()
			{
				this->Set_Invisible();

				switch (m_eDImageSubClass)
				{
				case DTO::EUIDImageSubClassType::CONVERSATION_DOWN:
					break;
				case DTO::EUIDImageSubClassType::CONVERSATION_BG:
  				CUITutorial_Manager::GetInstance()->Return_Locked_PlayerState();
					break;
				}
			}));
}

void CUIConversation_Image::Tick_By_Type(const _float fTimeDelta)
{
	switch (m_eDImageSubClass)
	{
	case DTO::EUIDImageSubClassType::CONVERSATION_DOWN:
		break;
	case DTO::EUIDImageSubClassType::CONVERSATION_BG:
		break;
	}
}

void CUIConversation_Image::Initialize_Visible_Event()
{
	switch (m_eDImageSubClass)
	{
	case DTO::EUIDImageSubClassType::CONVERSATION_DOWN:
		break;
	case DTO::EUIDImageSubClassType::CONVERSATION_BG:
		Ready_LerpChange(1.5f, 1.f, 0.f, 3.f, m_fDelay, true);
		break;
	}
}

_bool CUIConversation_Image::Tick_Visible_Event(const _float fTimeDelta)
{
	switch (m_eDImageSubClass)
	{
	case DTO::EUIDImageSubClassType::CONVERSATION_DOWN:
		return true;
		break;
	case DTO::EUIDImageSubClassType::CONVERSATION_BG:
	{
		_bool isChange = Tick_LerpChange(&m_fProgress_Ratio, fTimeDelta);

		if (isChange)
		{
			return true;
		}
	}
		break;
	}
	return false;
}

void CUIConversation_Image::Initialize_InVisible_Event()
{
	switch (m_eDImageSubClass)
	{
	case DTO::EUIDImageSubClassType::CONVERSATION_DOWN:
		break;
	case DTO::EUIDImageSubClassType::CONVERSATION_BG:
		Ready_LerpChange(1.5f, 0.f, 1.f, 3.f, m_fDelay, true);
		break;
	}
}

_bool CUIConversation_Image::Tick_InVisible_Event(const _float fTimeDelta)
{
	switch (m_eDImageSubClass)
	{
	case DTO::EUIDImageSubClassType::CONVERSATION_DOWN:
		return true;
		break;
	case DTO::EUIDImageSubClassType::CONVERSATION_BG:
	{
		_bool isChange = Tick_LerpChange(&m_fProgress_Ratio, fTimeDelta);

		if (isChange)
		{
			return true;
		}
	}
	break;
	}
	return false;
}

CUIConversation_Image* CUIConversation_Image::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CUIConversation_Image* pInstance = new CUIConversation_Image(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CUIConversation_Image::Create, Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUIConversation_Image::Clone(void* pArg)
{
	CUIConversation_Image* pInstance = new CUIConversation_Image(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CUIConversation_Image::Clone, Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUIConversation_Image::Free()
{
	Super::Free();
}
