#include "pch.h"
#include "UILoading_Image.h"
#include "Client_Defines.h"
//=================
// Component
//=================
#include "Texture.h"
#include "Shader.h"
#include "VIBuffer_Rect_Tex.h"
#include "GameInstance.h"

CUILoading_Image::CUILoading_Image(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	:CUIDynamic_Image(pDevice, pDeviceContext)
{
}

CUILoading_Image::CUILoading_Image(const CUILoading_Image& rhs)
	:CUIDynamic_Image(rhs)
{
}

HRESULT CUILoading_Image::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUILoading_Image::Initialize(void* pArg)
{
	LOADING_IMAGE_DESC* pDesc = static_cast<LOADING_IMAGE_DESC*>(pArg);
	m_isInteract = true;
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;
	if (FAILED(Ready_Components(pDesc)))
		return E_FAIL;
	return S_OK;
}



HRESULT CUILoading_Image::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;

	if (FAILED(Attach_Personal_Info()))
		return E_FAIL;

	Set_Visible();

	return S_OK;
}

void CUILoading_Image::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CUILoading_Image::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
}

void CUILoading_Image::Update_Late(const _float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);
}

void CUILoading_Image::Ready_Before_Render(const _float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);
}

HRESULT CUILoading_Image::Render()
{
	if (!m_isVisible)
		return S_OK;
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;
	if (FAILED(Super::Render()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUILoading_Image::Ready_Components(LOADING_IMAGE_DESC* pDesc)
{
	Super::Ready_Components(pDesc);
	return S_OK;
}

HRESULT CUILoading_Image::Bind_ShaderResources()
{
	CShader* pShader = Get_Component<CShader>();
	if (FAILED(Get_Component<CTransform>()->Bind_ShaderResource(pShader)))
		return E_FAIL;
	if (FAILED(Super::Bind_ShaderResources()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUILoading_Image::Attach_Personal_Info()
{
	switch (m_eDImageSubClass)
	{
	case DTO::EUIDImageSubClassType::LOADING_BG:
	{
	}
	break;
	case DTO::EUIDImageSubClassType::LOADING_BG_TOP:
	{
	}
	break;
	case DTO::EUIDImageSubClassType::LOADING_BG_BOTTOM:
	{
	}
	break;
	case DTO::EUIDImageSubClassType::END:
	default:
		return E_FAIL;
	}
	return S_OK;
}

void CUILoading_Image::Initialize_Visible_Event()
{
	m_isFin_Event = false;
	m_isActive = false;

	switch (m_eDImageSubClass)
	{
	case DTO::EUIDImageSubClassType::LOADING_BG:
	{
	}
	break;
	case DTO::EUIDImageSubClassType::LOADING_BG_TOP:
	{
		Ready_Lerp_Movement(Vec2{ 0.f, -50.f }, Vec2{ 0.f, 0.f }, 0.5f, 2.f, m_fDelay);
	}
	break;
	case DTO::EUIDImageSubClassType::LOADING_BG_BOTTOM:
	{
		Ready_Lerp_Movement(Vec2{ 0.f, 50.f }, Vec2{ 0.f, 0.f }, 0.5f, 2.f, m_fDelay);
	}
	break;
	case DTO::EUIDImageSubClassType::END:
	default:
		break;
	}
}

_bool CUILoading_Image::Tick_Visible_Event(const _float fTimeDelta)
{
	if (m_eDImageSubClass == DTO::EUIDImageSubClassType::LOADING_BG)
	{
		m_isFin_Event = true;
		m_isActive = true;
		return true;
	}
	else if (m_eDImageSubClass == DTO::EUIDImageSubClassType::LOADING_BG_TOP || m_eDImageSubClass == DTO::EUIDImageSubClassType::LOADING_BG_BOTTOM)
	{
		const _bool isFade = Tick_Lerp_Movement(fTimeDelta);
		if (isFade)
		{
			m_isFin_Event = true;
			m_isActive = true;
			return true;
		}
	}
	return false;
}

CUILoading_Image* CUILoading_Image::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CUILoading_Image* pInstance = new CUILoading_Image(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CUILoading_Image::Create, Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUILoading_Image::Clone(void* pArg)
{
	CUILoading_Image* pInstance = new CUILoading_Image(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CUILoading_Image::Clone, Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUILoading_Image::Free()
{
	Super::Free();
}
