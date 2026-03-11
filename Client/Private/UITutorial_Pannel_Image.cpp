#include "pch.h"
#include "UITutorial_Pannel_Image.h"
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

CUITutorial_Pannel_Image::CUITutorial_Pannel_Image(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	:CUIDynamic_Image(pDevice, pDeviceContext)
{
}

CUITutorial_Pannel_Image::CUITutorial_Pannel_Image(const CUITutorial_Pannel_Image& rhs)
	:CUIDynamic_Image(rhs)
{
}

HRESULT CUITutorial_Pannel_Image::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUITutorial_Pannel_Image::Initialize(void* pArg)
{
	TUTORIAL_PANNEL_IMAGE_DESC* pDesc = static_cast<TUTORIAL_PANNEL_IMAGE_DESC*>(pArg);
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;
	if (FAILED(Ready_Components(pDesc)))
		return E_FAIL;
	return S_OK;
}


HRESULT CUITutorial_Pannel_Image::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;
	if (FAILED(Attach_Personal_Info()))
		return E_FAIL;

	return S_OK;
}

void CUITutorial_Pannel_Image::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CUITutorial_Pannel_Image::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
}

void CUITutorial_Pannel_Image::Update_Late(const _float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);
}

void CUITutorial_Pannel_Image::Ready_Before_Render(const _float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);
}

HRESULT CUITutorial_Pannel_Image::Render()
{
	if (!m_isVisible)
		return S_OK;
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;
	if (FAILED(Super::Render()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUITutorial_Pannel_Image::Ready_Components(TUTORIAL_PANNEL_IMAGE_DESC* pDesc)
{
	if (FAILED(Super::Ready_Components(pDesc)))
		return E_FAIL;
	return S_OK;
}

HRESULT CUITutorial_Pannel_Image::Bind_ShaderResources()
{
	CShader* pShader = Get_Component<CShader>();
	if (FAILED(Get_Component<CTransform>()->Bind_ShaderResource(pShader)))
		return E_FAIL;
	if (FAILED(Super::Bind_ShaderResources()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUITutorial_Pannel_Image::Attach_Personal_Info()
{
	switch (m_eDImageSubClass)
	{
	case DTO::EUIDImageSubClassType::TUTORIAL_PANNEL_TOP_BG:		// 윗 배경
		break;
	case DTO::EUIDImageSubClassType::TUTORIAL_PANNEL_BG:			// 디졸브 배경
		break;
	case DTO::EUIDImageSubClassType::TUTORIAL_PANNEL_BOTTOM_BG:		// 투명한 아래 배경
		break;
	case DTO::EUIDImageSubClassType::TUTORIAL_PANNEL_TOP_ICON:		// ? 아이콘
		break;
	case DTO::EUIDImageSubClassType::TUTORIAL_PANNEL_ICON:			// 설명 이미지
		break;
	case DTO::EUIDImageSubClassType::TUTORIAL_PANNEL_BUTTON:		// 버튼 0이 Prev, 1이 Next
		break;
	case DTO::EUIDImageSubClassType::TUTORIAL_PANNEL_BUTTON_FX:
		break;
	case DTO::EUIDImageSubClassType::TUTORIAL_PANNEL_BUTTON_OUTLINE:
		break;
	case DTO::EUIDImageSubClassType::TUTORIAL_PANNEL_NUM_PAGE_ICON:
		break;
	}

	if (m_isSpawned)
	{
		Set_Visible();
		m_isSpawned = false;
	}
	return S_OK;
}

void CUITutorial_Pannel_Image::Bind_Events()
{
	m_vecEventHandles.push_back(
		m_pUIManager->Get_UIEvents().Subscribe([this](const UIEVENT_DESC& Desc)
			{
				if (EUIEventID::TUTORIAL_PANNEL_START == Desc.eEventID)
				{
					this->Set_Visible();
				}
			})
	);
	m_vecEventHandles.push_back(
		m_pUIManager->Get_UIEvents().Subscribe([this](const UIEVENT_DESC& Desc)
			{
				if (EUIEventID::TUTORIAL_PANNEL_START == Desc.eEventID)
				{
					this->Set_Invisible();
				}
			})
	);
}

void CUITutorial_Pannel_Image::Tick_By_Type(const _float fTimeDelta)
{
	switch (m_eDImageSubClass)
	{
	case DTO::EUIDImageSubClassType::TUTORIAL_PANNEL_TOP_BG:		// 윗 배경
		break;
	case DTO::EUIDImageSubClassType::TUTORIAL_PANNEL_BG:			// 디졸브 배경
		break;
	case DTO::EUIDImageSubClassType::TUTORIAL_PANNEL_BOTTOM_BG:		// 투명한 아래 배경
		break;
	case DTO::EUIDImageSubClassType::TUTORIAL_PANNEL_TOP_ICON:		// ? 아이콘
		break;
	case DTO::EUIDImageSubClassType::TUTORIAL_PANNEL_ICON:			// 설명 이미지
		break;
	case DTO::EUIDImageSubClassType::TUTORIAL_PANNEL_BUTTON:		// 버튼 0이 Prev, 1이 Next
		break;
	case DTO::EUIDImageSubClassType::TUTORIAL_PANNEL_BUTTON_FX:
		break;
	case DTO::EUIDImageSubClassType::TUTORIAL_PANNEL_BUTTON_OUTLINE:
		break;
	case DTO::EUIDImageSubClassType::TUTORIAL_PANNEL_NUM_PAGE_ICON:
		break;
	}
}

void CUITutorial_Pannel_Image::Initialize_Visible_Event()
{
	m_isFin_Event = false;
	m_isActive = false;

	switch (m_eDImageSubClass)
	{
	case DTO::EUIDImageSubClassType::TUTORIAL_PANNEL_TOP_BG:		// 윗 배경
		Ready_Lerp_Movement(Vec2{ 0.f, 235.5 }, Vec2{ 0.f, 0.f }, 1.5f, 3.f, 0.5f, true);
		break;
	case DTO::EUIDImageSubClassType::TUTORIAL_PANNEL_BG:			// 디졸브 배경
		Ready_LerpChange(1.f, 1.f, 0.f, 2.f, 1.f);
		break;
	case DTO::EUIDImageSubClassType::TUTORIAL_PANNEL_BOTTOM_BG:		// 투명한 아래 배경
		Ready_Fade(1.f, 0.f, 0.6f, 1.f);
		break;
	case DTO::EUIDImageSubClassType::TUTORIAL_PANNEL_TOP_ICON:		// ? 아이콘
		Ready_Lerp_Movement(Vec2{ 0.f, 235.5 }, Vec2{ 0.f, 0.f }, 1.5f, 3.f, 0.5f, true);
		Ready_LerpChange(0.3f, 2.f, 1.f, 1.f, 0.f);
		break;
	case DTO::EUIDImageSubClassType::TUTORIAL_PANNEL_ICON:			// 설명 이미지
		Ready_Fade(1.f, 0.f, 1.f, 1.f);
		break;
	case DTO::EUIDImageSubClassType::TUTORIAL_PANNEL_BUTTON:		// 버튼 0이 Prev, 1이 Next
		Ready_Fade(1.f, 0.f, 1.f, 1.f);
		break;
	case DTO::EUIDImageSubClassType::TUTORIAL_PANNEL_BUTTON_FX:
		Ready_Fade(1.f, 0.f, 1.f, 1.f);
		break;
	case DTO::EUIDImageSubClassType::TUTORIAL_PANNEL_BUTTON_OUTLINE:
		Ready_Fade(1.f, 0.f, 1.f, 1.f);
		break;
	case DTO::EUIDImageSubClassType::TUTORIAL_PANNEL_NUM_PAGE_ICON:
		Ready_Fade(1.f, 0.f, 1.f, 1.f);
		break;
	}
}

_bool CUITutorial_Pannel_Image::Tick_Visible_Event(const _float fTimeDelta)
{
	m_isActive = true;

	switch (m_eDImageSubClass)
	{
	case DTO::EUIDImageSubClassType::TUTORIAL_PANNEL_TOP_BG:		// 윗 배경
	{
		_bool is = Tick_Lerp_Movement(fTimeDelta);

		if (is)
		{
			m_isFin_Event	= true;
			m_isActive		= true;

			return true;
		}
	}
	break;
	case DTO::EUIDImageSubClassType::TUTORIAL_PANNEL_BG:			// 디졸브 배경
	{
		_bool is = Tick_LerpChange(&m_fProgress_Ratio, fTimeDelta);
		if (is)
		{
			m_isFin_Event = true;
			m_isActive = true;

			return true;
		}
	}
	break;
	case DTO::EUIDImageSubClassType::TUTORIAL_PANNEL_BOTTOM_BG:		// 투명한 아래 배경
	{
		_bool is = Tick_Fade(fTimeDelta);
		if (is)
		{
			m_isFin_Event = true;
			m_isActive = true;

			return true;
		}
	}
	break;
	case DTO::EUIDImageSubClassType::TUTORIAL_PANNEL_TOP_ICON:		// ? 아이콘
	{
		_bool is = Tick_Lerp_Movement(fTimeDelta);
		_bool isScale = Tick_LerpChange(&m_fScale, fTimeDelta);
		if (is && isScale)
		{
			m_isFin_Event = true;
			m_isActive = true;

			return true;
		}
	}
	break;
	case DTO::EUIDImageSubClassType::TUTORIAL_PANNEL_ICON:			// 설명 이미지
	{
		_bool is = Tick_Fade(fTimeDelta);
		if (is)
		{
			m_isFin_Event = true;
			m_isActive = true;

			return true;
		}
	}
	break;
	case DTO::EUIDImageSubClassType::TUTORIAL_PANNEL_BUTTON:		// 버튼 0이 Prev, 1이 Next
	{
		_bool is = Tick_Fade(fTimeDelta);
		if (is)
		{
			m_isFin_Event = true;
			m_isActive = true;

			return true;
		}
	}
	break;
	case DTO::EUIDImageSubClassType::TUTORIAL_PANNEL_BUTTON_FX:
	{
		_bool is = Tick_Fade(fTimeDelta);
		if (is)
		{
			m_isFin_Event = true;
			m_isActive = true;

			return true;
		}
	}
	break;
	case DTO::EUIDImageSubClassType::TUTORIAL_PANNEL_BUTTON_OUTLINE:
	{
		_bool is = Tick_Fade(fTimeDelta);
		if (is)
		{
			m_isFin_Event = true;
			m_isActive = true;

			return true;
		}
	}
	break;
	case DTO::EUIDImageSubClassType::TUTORIAL_PANNEL_NUM_PAGE_ICON:
	{
		_bool is = Tick_Fade(fTimeDelta);
		if (is)
		{
			m_isFin_Event = true;
			m_isActive = true;

			return true;
		}
	}
	break;
	}
	return false;
}

void CUITutorial_Pannel_Image::Initialize_InVisible_Event()
{
	m_isActive = false;
	m_isFin_Event = false;

	switch (m_eDImageSubClass)
	{
	case DTO::EUIDImageSubClassType::TUTORIAL_PANNEL_TOP_BG:		// 윗 배경
		break;
	case DTO::EUIDImageSubClassType::TUTORIAL_PANNEL_BG:			// 디졸브 배경
		break;
	case DTO::EUIDImageSubClassType::TUTORIAL_PANNEL_BOTTOM_BG:		// 투명한 아래 배경
		break;
	case DTO::EUIDImageSubClassType::TUTORIAL_PANNEL_TOP_ICON:		// ? 아이콘
		break;
	case DTO::EUIDImageSubClassType::TUTORIAL_PANNEL_ICON:			// 설명 이미지
		break;
	case DTO::EUIDImageSubClassType::TUTORIAL_PANNEL_BUTTON:		// 버튼 0이 Prev, 1이 Next
		break;
	case DTO::EUIDImageSubClassType::TUTORIAL_PANNEL_BUTTON_FX:
		break;
	case DTO::EUIDImageSubClassType::TUTORIAL_PANNEL_BUTTON_OUTLINE:
		break;
	case DTO::EUIDImageSubClassType::TUTORIAL_PANNEL_NUM_PAGE_ICON:
		break;
	}
}

_bool CUITutorial_Pannel_Image::Tick_InVisible_Event(const _float fTimeDelta)
{

	switch (m_eDImageSubClass)
	{
	case DTO::EUIDImageSubClassType::TUTORIAL_PANNEL_TOP_BG:		// 윗 배경
		break;
	case DTO::EUIDImageSubClassType::TUTORIAL_PANNEL_BG:			// 디졸브 배경
		break;
	case DTO::EUIDImageSubClassType::TUTORIAL_PANNEL_BOTTOM_BG:		// 투명한 아래 배경
		break;
	case DTO::EUIDImageSubClassType::TUTORIAL_PANNEL_TOP_ICON:		// ? 아이콘
		break;
	case DTO::EUIDImageSubClassType::TUTORIAL_PANNEL_ICON:			// 설명 이미지
		break;
	case DTO::EUIDImageSubClassType::TUTORIAL_PANNEL_BUTTON:		// 버튼 0이 Prev, 1이 Next
		break;
	case DTO::EUIDImageSubClassType::TUTORIAL_PANNEL_BUTTON_FX:
		break;
	case DTO::EUIDImageSubClassType::TUTORIAL_PANNEL_BUTTON_OUTLINE:
		break;
	case DTO::EUIDImageSubClassType::TUTORIAL_PANNEL_NUM_PAGE_ICON:
		break;
	}

	Request_SetDead();
	m_isFin_Event = true;
	m_isActive = true;
	return true;
}

HRESULT CUITutorial_Pannel_Image::Spawn_FromPool(void* pArg)
{
	if (FAILED(Super::Spawn_FromPool(pArg)))
		return E_FAIL;

	UI_PREFAB_DATA* pDesc = static_cast<UI_PREFAB_DATA*>(pArg);

	if (auto* pDamageFont = std::get_if<UI_TUTORIAL_PANNEL_PREFAB_DATA>(&pDesc->Data))
	{

	}
	m_isSpawned = true;
	m_isDeadRequest = false;
	return S_OK;
}

HRESULT CUITutorial_Pannel_Image::Despawn_FromPool()
{
	if (FAILED(Super::Despawn_FromPool()))
		return E_FAIL;

	m_isVisible = false;
	m_isVisibleTrigger = false;
	m_isPreVisible = false;
	return S_OK;
}

CUITutorial_Pannel_Image* CUITutorial_Pannel_Image::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CUITutorial_Pannel_Image* pInstance = new CUITutorial_Pannel_Image(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CUITutorial_Pannel_Image::Create, Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUITutorial_Pannel_Image::Clone(void* pArg)
{
	CUITutorial_Pannel_Image* pInstance = new CUITutorial_Pannel_Image(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CUITutorial_Pannel_Image::Clone, Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUITutorial_Pannel_Image::Free()
{
	Super::Free();
}
