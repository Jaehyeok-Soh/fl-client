#include "pch.h"
#include "UIScreenPulse_Image.h"
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

CUIScreenPulse_Image::CUIScreenPulse_Image(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	:CUIDynamic_Image(pDevice, pDeviceContext)
{
}

CUIScreenPulse_Image::CUIScreenPulse_Image(const CUIScreenPulse_Image& rhs)
	:CUIDynamic_Image(rhs)
{
}

HRESULT CUIScreenPulse_Image::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIScreenPulse_Image::Initialize(void* pArg)
{
	SCREENPULSE_IMAGE_DESC* pDesc = static_cast<SCREENPULSE_IMAGE_DESC*>(pArg);
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;
	if (FAILED(Ready_Components(pDesc)))
		return E_FAIL;

	return S_OK;
}


HRESULT CUIScreenPulse_Image::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;
	if (FAILED(Attach_Personal_Info()))
		return E_FAIL;

	return S_OK;
}

void CUIScreenPulse_Image::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CUIScreenPulse_Image::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
	Tick_By_Type(fTimeDelta);
}

void CUIScreenPulse_Image::Update_Late(const _float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);
}

void CUIScreenPulse_Image::Ready_Before_Render(const _float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);
}

HRESULT CUIScreenPulse_Image::Render()
{
	if (!m_isVisible)
		return S_OK;
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;
	if (FAILED(Super::Render()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIScreenPulse_Image::Ready_Components(SCREENPULSE_IMAGE_DESC* pDesc)
{
	if (FAILED(Super::Ready_Components(pDesc)))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIScreenPulse_Image::Bind_ShaderResources()
{
	CShader* pShader = Get_Component<CShader>();
	if (FAILED(Get_Component<CTransform>()->Bind_ShaderResource(pShader)))
		return E_FAIL;

	if (FAILED(pShader->Get_Variable("g_vNoiseUVScale")->SetRawValue(&m_vNoiseUVScale, 0, sizeof(Vec2))))
		return E_FAIL;
	if (FAILED(pShader->Get_Variable("g_vNoiseUVScroll")->SetRawValue(&m_vNoiseUVScroll, 0, sizeof(Vec2))))
		return E_FAIL;
	if (FAILED(pShader->Get_Variable("g_fTime")->SetRawValue(&m_fTime, 0, sizeof(_float))))
		return E_FAIL;
	if (FAILED(pShader->Get_Variable("g_fGlowDistort")->SetRawValue(&m_fGlowDistort, 0, sizeof(_float))))
		return E_FAIL;
	if (FAILED(pShader->Get_Variable("g_fGlowPulseSpeed")->SetRawValue(&m_fGlowPulseSpeed, 0, sizeof(_float))))
		return E_FAIL;
	if (FAILED(pShader->Get_Variable("g_fGlowIntensity")->SetRawValue(&m_fGlowIntensity, 0, sizeof(_float))))
		return E_FAIL;

	if (FAILED(Super::Bind_ShaderResources()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIScreenPulse_Image::Attach_Personal_Info()
{
	m_vNoiseUVScale = Vec2{ 1.f, 1.f };
	m_vNoiseUVScroll = Vec2{ 0.1f, 0.1f };
	m_fGlowDistort = 0.003f;
	m_fGlowPulseSpeed = 5.f;
	m_fGlowIntensity = 3.f;
	m_vColorTint = Vec4{ 0.7f, 0.f, 0.f, 1.f };
	m_vGradiantColorTint = Vec4{ 0.7f, 0.f, 0.f, 1.f };

	m_fWidth = 2650.f;
	m_fHeight = 1300.f;
	m_fAlpha_Ratio = 0.3f;
	return S_OK;
}

void CUIScreenPulse_Image::Bind_Events()
{
	Super::Bind_Events();
	// 메뉴 Event
	m_vecEventHandles.push_back(
		m_pUIManager->Get_UIEvents().Subscribe([this](const UIEVENT_DESC& Desc)
			{
				if (EUIEventID::MENU_CLOSE == Desc.eEventID)
				{
					if (this->m_isVisibleNow)
					{
						this->Set_Visible();
						this->Set_Active(true);
					}
				}
			}));
	m_vecEventHandles.push_back(
		m_pUIManager->Get_UIEvents().Subscribe([this](const UIEVENT_DESC& Desc)
			{
				if (EUIEventID::MENU_OPEN == Desc.eEventID)
				{
					this->Set_Invisible();
				}
			}));

	// 시네마틱 Event -> 검정색 UI 위 아래에서 나오는 Event
	m_vecEventHandles.push_back(
		m_pGameInstance->Subscribe<CINEMATIC_START>(
			[this]()
			{
				this->Set_Invisible();
			}));
	m_vecEventHandles.push_back(
		m_pGameInstance->Subscribe<CINEMATIC_END>([this]()
			{
				if (this->m_isVisibleNow)
				{
					this->Set_Visible();
					this->Set_Active(true);
				}
			}));

	// 대화 Event
	m_vecEventHandles.push_back(
		m_pGameInstance->Subscribe<DIALOGUE_BEGIN>([this](_int iId)
			{
				this->Set_Invisible();
			}));
	m_vecEventHandles.push_back(
		m_pGameInstance->Subscribe<DIALOGUE_END>([this]()
			{
				if (this->m_isVisibleNow)
				{
					this->Set_Visible();
					this->Set_Active(true);
				}
			}));

	// 패널 Events
	m_vecEventHandles.push_back(
		m_pUIManager->Get_UIEvents().Subscribe([this](const UIEVENT_DESC& Desc)
			{
				if (EUIEventID::TUTORIAL_PANNEL_START == Desc.eEventID)
				{
					this->Set_Invisible();
				}
			}));
	m_vecEventHandles.push_back(
		m_pUIManager->Get_UIEvents().Subscribe([this](const UIEVENT_DESC& Desc)
			{
				if (EUIEventID::TUTORIAL_PANNEL_END == Desc.eEventID)
				{
					if (this->m_isVisibleNow)
					{
						this->Set_Visible();
						this->Set_Active(true);
					}
				}
			}));

	// Low Hp
	m_vecEventHandles.push_back(
		m_pUIManager->Get_UIEvents().Subscribe([this](const UIEVENT_DESC& Desc)
			{
				if (EUIEventID::PLAYER_NORMAL_HP == Desc.eEventID)
				{
					this->m_isVisibleNow = false;
					this->Set_Invisible();
				}
			}));
	m_vecEventHandles.push_back(
		m_pUIManager->Get_UIEvents().Subscribe([this](const UIEVENT_DESC& Desc)
			{
				if (EUIEventID::PLAYER_LOW_HP == Desc.eEventID)
				{
					this->Set_Visible();
					this->m_isVisibleNow = true;
					this->Set_Active(true);
				}
			}));
}

void CUIScreenPulse_Image::Tick_By_Type(const _float fTimeDelta)
{
	m_fTime += fTimeDelta;
}

void CUIScreenPulse_Image::Initialize_Visible_Event()
{
}

_bool CUIScreenPulse_Image::Tick_Visible_Event(const _float fTimeDelta)
{
	return true;
}

void CUIScreenPulse_Image::Initialize_InVisible_Event()
{
}

_bool CUIScreenPulse_Image::Tick_InVisible_Event(const _float fTimeDelta)
{
	Set_Active(false);
	return true;
}

CUIScreenPulse_Image* CUIScreenPulse_Image::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CUIScreenPulse_Image* pInstance = new CUIScreenPulse_Image(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CUIScreenPulse_Image::Create, Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUIScreenPulse_Image::Clone(void* pArg)
{
	CUIScreenPulse_Image* pInstance = new CUIScreenPulse_Image(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CUIScreenPulse_Image::Clone, Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUIScreenPulse_Image::Free()
{
	Super::Free();
}
