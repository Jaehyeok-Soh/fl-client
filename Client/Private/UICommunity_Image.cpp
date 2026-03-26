#include "pch.h"
#include "UICommunity_Image.h"
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
#include "GameInstance.h"

CUICommunity_Image::CUICommunity_Image(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	:CUIDynamic_Image(pDevice, pDeviceContext)
{
}

CUICommunity_Image::CUICommunity_Image(const CUICommunity_Image& rhs)
	:CUIDynamic_Image(rhs)
{
}

HRESULT CUICommunity_Image::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUICommunity_Image::Initialize(void* pArg)
{
	COMMUNITY_IMAGE_DESC* pDesc = static_cast<COMMUNITY_IMAGE_DESC*>(pArg);
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;
	if (FAILED(Ready_Components(pDesc)))
		return E_FAIL;

	return S_OK;
}


HRESULT CUICommunity_Image::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;
	if (FAILED(Attach_Personal_Info()))
		return E_FAIL;

	return S_OK;
}

void CUICommunity_Image::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CUICommunity_Image::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
	Tick_By_Type(fTimeDelta);
}

void CUICommunity_Image::Update_Late(const _float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);
}

void CUICommunity_Image::Ready_Before_Render(const _float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);
}

HRESULT CUICommunity_Image::Render()
{
	if (!m_isVisible)
		return S_OK;
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;
	if (FAILED(Super::Render()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUICommunity_Image::Ready_Components(COMMUNITY_IMAGE_DESC* pDesc)
{
	if (FAILED(Super::Ready_Components(pDesc)))
		return E_FAIL;
	return S_OK;
}

HRESULT CUICommunity_Image::Bind_ShaderResources()
{
	CShader* pShader = Get_Component<CShader>();
	if (FAILED(Get_Component<CTransform>()->Bind_ShaderResource(pShader)))
		return E_FAIL;
	if (FAILED(Super::Bind_ShaderResources()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUICommunity_Image::Attach_Personal_Info()
{
	m_vOriginColor = m_vColorTint;
	m_vOriginGradColor = m_vGradiantColorTint;

	return S_OK;
}

void CUICommunity_Image::Bind_Events()
{
	m_vecEventHandles.push_back(
		m_pUIManager->Get_UIEvents().Subscribe([this](const UIEVENT_DESC& Desc)
			{
				if (EUIEventID::MENU_OPEN == Desc.eEventID)
				{
					this->Set_Invisible();
				}
			})
	);

	m_vecEventHandles.push_back(
		m_pGameInstance->Subscribe<CINEMATIC_START>(
			[this]()
			{
				this->Set_Invisible();
			})
	);

	m_vecEventHandles.push_back(
		m_pGameInstance->Subscribe<INTERACT_DETECT>([this](CGameObject* pObj)
			{
				Set_Active(true);
				this->Set_Visible();

				m_pInteractObj = pObj;
			})
	);

	m_vecEventHandles.push_back(
		m_pGameInstance->Subscribe<INTERACT_LOST>([this](CGameObject* pObj)
			{
				m_pInteractObj = nullptr;
				this->Set_Invisible();
			})
	);

	m_vecEventHandles.push_back(
		m_pGameInstance->Subscribe<INTERACT_ENTER>([this](CGameObject* pObj)
			{
				this->Set_Invisible();
				m_isClick = true;
			})
	);
}

void CUICommunity_Image::Tick_By_Type(const _float fTimeDelta)
{
	switch (m_eDImageSubClass)
	{
	case DTO::EUIDImageSubClassType::COMMUNITY_KEY:
		break;
	case DTO::EUIDImageSubClassType::COMMUNITY_ICON:
		break;
	case DTO::EUIDImageSubClassType::COMMUNITY_ARROW:
		break;
	case DTO::EUIDImageSubClassType::COMMUNITY_BG:
		if (KEY_BUTTON_UP(DIK_F))
		{
			m_pGameInstance->Broadcast<INTERACT_ENTER>(m_pInteractObj);
			m_pGameInstance->Play_OneShot(0, TO_HASH("UI_COMMUNITY_ENTER"), 1.f);
		}
		break;
	case DTO::EUIDImageSubClassType::COMMUNITY_OUTLINE:
		break;
	case DTO::EUIDImageSubClassType::COMMUNITY_DECO:
		break;
	}
}

void CUICommunity_Image::Initialize_Visible_Event()
{
	switch (m_eDImageSubClass)
	{
	case DTO::EUIDImageSubClassType::COMMUNITY_KEY:
	case DTO::EUIDImageSubClassType::COMMUNITY_ICON:
	case DTO::EUIDImageSubClassType::COMMUNITY_ARROW:
	case DTO::EUIDImageSubClassType::COMMUNITY_BG:
	case DTO::EUIDImageSubClassType::COMMUNITY_OUTLINE:
		Ready_Fade(0.5f, 0.f, 1.f, m_fDelay);
		break;

	case DTO::EUIDImageSubClassType::COMMUNITY_DECO:
		Ready_Fade(0.5f, 0.f, 0.5f, m_fDelay);
		break;
	}

}

_bool CUICommunity_Image::Tick_Visible_Event(const _float fTimeDelta)
{
	_bool isFade = Tick_Fade(fTimeDelta);

	if (isFade)
	{
		return true;
	}

	return false;
}

void CUICommunity_Image::Initialize_InVisible_Event()
{
	switch (m_eDImageSubClass)
	{
	case DTO::EUIDImageSubClassType::COMMUNITY_KEY:
	case DTO::EUIDImageSubClassType::COMMUNITY_ICON:
	case DTO::EUIDImageSubClassType::COMMUNITY_ARROW:
	case DTO::EUIDImageSubClassType::COMMUNITY_BG:
		Ready_Fade(0.3f, 1.f, 0.f, m_fDelay);
		break;
	case DTO::EUIDImageSubClassType::COMMUNITY_OUTLINE:
	{
		if (m_isClick)
		{
			Ready_ChageColor(0.12f, 
				m_vOriginColor, m_vOriginGradColor,
				Vec4{ 1.f, 0.98f, 0.8f, 0.f }, m_vOriginGradColor,
				3.f, m_fDelay);
		}

		Ready_Fade(0.3f, 1.f, 0.f, m_fDelay);
	}
		break;


	case DTO::EUIDImageSubClassType::COMMUNITY_DECO:
		Ready_Fade(0.3f, 0.5f, 0.f, m_fDelay);
		break;
	}

}

_bool CUICommunity_Image::Tick_InVisible_Event(const _float fTimeDelta)
{
	_bool isFade = Tick_Fade(fTimeDelta);
	

	switch (m_eDImageSubClass)
	{
	case DTO::EUIDImageSubClassType::COMMUNITY_KEY:
	case DTO::EUIDImageSubClassType::COMMUNITY_ICON:
	case DTO::EUIDImageSubClassType::COMMUNITY_ARROW:
	case DTO::EUIDImageSubClassType::COMMUNITY_BG:
	{
		if (isFade)
		{
			m_isClick = false;
			Set_Active(false);
			return true;
		}
	}
	break;
	case DTO::EUIDImageSubClassType::COMMUNITY_OUTLINE:
	{
		if (m_isClick)
		{
			_bool isColor = Tick_ChageColor(fTimeDelta);
			if (isColor && !m_isPaulse)
			{
				m_isPaulse = true;
				Ready_ChageColor(0.12f, 
					Vec4{ 1.f, 0.98f, 0.8f, 0.f }, m_vOriginGradColor,
					m_vOriginColor, m_vOriginGradColor,
					3.f, m_fDelay);
			}

			if (isFade && isColor)
			{
				m_vColorTint = m_vOriginColor;
				m_vGradiantColorTint = m_vOriginGradColor;

				Set_Active(false);
				m_isClick = false;
				return true;
			}
		}
		else
		{
			if (isFade)
			{
				m_vColorTint = m_vOriginColor;
				m_vGradiantColorTint = m_vOriginGradColor;

				Set_Active(false);
				m_isClick = false;
				return true;
			}
		}
	}
	break;
	case DTO::EUIDImageSubClassType::COMMUNITY_DECO:
	{
		if (isFade)
		{
			Set_Active(false);
			m_isClick = false;
			return true;
		}
	}
		break;
	}

	return false;
}

CUICommunity_Image* CUICommunity_Image::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CUICommunity_Image* pInstance = new CUICommunity_Image(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CUICommunity_Image::Create, Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUICommunity_Image::Clone(void* pArg)
{
	CUICommunity_Image* pInstance = new CUICommunity_Image(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CUICommunity_Image::Clone, Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUICommunity_Image::Free()
{
	Super::Free();
}
