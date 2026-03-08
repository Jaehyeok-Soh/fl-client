#include "pch.h"
#include "UIWeakness_Text.h"
#include "Client_Defines.h"
#include "Client_EventDefine.h"

//=================
// Component
//=================
#include "StatCom_Player.h"
#include "Texture.h"
#include "Shader.h"
#include "VIBuffer_Rect_Tex.h"
#include "UI_Manager.h"
#include "GameInstance.h"

CUIWeakness_Text::CUIWeakness_Text(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	:CUIText(pDevice, pDeviceContext)
{
}

CUIWeakness_Text::CUIWeakness_Text(const CUIWeakness_Text& rhs)
	:CUIText(rhs)
{
}

HRESULT CUIWeakness_Text::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIWeakness_Text::Initialize(void* pArg)
{
	WEAKNESS_TEXT_DESC* pDesc = static_cast<WEAKNESS_TEXT_DESC*>(pArg);
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;
	if (FAILED(Ready_Components(pDesc)))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIWeakness_Text::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;

	if (FAILED(Attach_Personal_Info()))
		return E_FAIL;

	return S_OK;
}


void CUIWeakness_Text::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CUIWeakness_Text::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
	Tick_By_Type(fTimeDelta);
}

void CUIWeakness_Text::Update_Late(const _float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);
}

void CUIWeakness_Text::Ready_Before_Render(const _float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);

	Tick_By_Type(fTimeDelta);

	if (FAILED(Convert_Stat_To_Text()))
		return;
}

HRESULT CUIWeakness_Text::Render()
{
	if (!m_isVisible)
		return S_OK;
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;
	if (FAILED(Super::Render()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIWeakness_Text::Ready_Components(WEAKNESS_TEXT_DESC* pDesc)
{
	if (FAILED(Super::Ready_Components(pDesc)))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIWeakness_Text::Bind_ShaderResources()
{
	CShader* pShader = Get_Component<CShader>();
	if (FAILED(Get_Component<CTransform>()->Bind_ShaderResource(pShader)))
		return E_FAIL;
	if (FAILED(Super::Bind_ShaderResources()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIWeakness_Text::Attach_Personal_Info()
{
	switch (m_eTextSubClassType)
	{
	case DTO::EUITextSubClassType::BATTLE_WEAKNESS_BEGIN:
		break;
	case DTO::EUITextSubClassType::BATTLE_WEAKNESS_TEXT:
		break;
	case DTO::EUITextSubClassType::BATTLE_WEAKNESS_WORLD_TEXT_LEFT:
		break;
	case DTO::EUITextSubClassType::BATTLE_WEAKNESS_WORLD_TEXT_RIGHT:
		break;
	case DTO::EUITextSubClassType::BSTTLE_WEAKNESS_END:
		break;
	case DTO::EUITextSubClassType::END:
	default:
		return E_FAIL;
	}
	return S_OK;
}


void CUIWeakness_Text::Bind_Events()
{
	m_tEventHandle0 = (m_pGameInstance->Subscribe<BOSS_GROGGY>([this]()
		{
			this->Set_Visible();
		})
		);


	m_vecEventHandles.push_back(m_pUIManager->Get_UIEvents().Subscribe([this](const UIEVENT_DESC& Desc)
		{
			if (EUIEventID::WEAKNESS_FIN == Desc.eEventID)
				this->Set_Invisible();
		})
	);

	switch (m_eTextSubClassType)
	{
	case DTO::EUITextSubClassType::BATTLE_WEAKNESS_BEGIN:
		break;
	case DTO::EUITextSubClassType::BATTLE_WEAKNESS_TEXT:
		break;
	case DTO::EUITextSubClassType::BATTLE_WEAKNESS_WORLD_TEXT_LEFT:
		break;
	case DTO::EUITextSubClassType::BATTLE_WEAKNESS_WORLD_TEXT_RIGHT:
		break;
	case DTO::EUITextSubClassType::BSTTLE_WEAKNESS_END:
		break;
	case DTO::EUITextSubClassType::END:
	default:
		break;
	}
}

void CUIWeakness_Text::Tick_By_Type(const _float fTimeDelta)
{
	switch (m_eTextSubClassType)
	{
	case DTO::EUITextSubClassType::BATTLE_WEAKNESS_BEGIN:
		break;
	case DTO::EUITextSubClassType::BATTLE_WEAKNESS_TEXT:
		break;
	case DTO::EUITextSubClassType::BATTLE_WEAKNESS_WORLD_TEXT_LEFT:
		break;
	case DTO::EUITextSubClassType::BATTLE_WEAKNESS_WORLD_TEXT_RIGHT:
		break;
	case DTO::EUITextSubClassType::BSTTLE_WEAKNESS_END:
		break;
	case DTO::EUITextSubClassType::END:
	default:
		break;
	}
}

HRESULT CUIWeakness_Text::Convert_Stat_To_Text()
{
	return S_OK;
}

void CUIWeakness_Text::Initialize_Visible_Event()
{
	switch (m_eTextSubClassType)
	{
	case DTO::EUITextSubClassType::BATTLE_WEAKNESS_BEGIN:
		break;
	case DTO::EUITextSubClassType::BATTLE_WEAKNESS_TEXT:
		Ready_LerpChange(0.5f, 3.f, 1.f, 3.f, m_fDelay);
		break;
	case DTO::EUITextSubClassType::BATTLE_WEAKNESS_WORLD_TEXT_LEFT:
		Ready_Lerp_Movement(Vec2{ -50.f, 0.f }, Vec2{ 0.f, 0.f }, 0.5f, 2.f, m_fDelay);
		break;
	case DTO::EUITextSubClassType::BATTLE_WEAKNESS_WORLD_TEXT_RIGHT:
		Ready_Lerp_Movement(Vec2{ 50.f, 0.f }, Vec2{ 0.f, 0.f }, 0.5f, 2.f, m_fDelay);
		break;
	case DTO::EUITextSubClassType::BSTTLE_WEAKNESS_END:
		break;
	case DTO::EUITextSubClassType::END:
	default:
		break;
	}
}

void CUIWeakness_Text::Initialize_InVisible_Event()
{
	switch (m_eTextSubClassType)
	{
	case DTO::EUITextSubClassType::BATTLE_WEAKNESS_BEGIN:
		break;
	case DTO::EUITextSubClassType::BATTLE_WEAKNESS_TEXT:
		Ready_Fade_Text(0.5f, 1.f, 0.f, 1.f);
		break;
	case DTO::EUITextSubClassType::BATTLE_WEAKNESS_WORLD_TEXT_LEFT:
		Ready_Fade_Text(0.5f, 1.f, 0.f, 1.f);
		break;
	case DTO::EUITextSubClassType::BATTLE_WEAKNESS_WORLD_TEXT_RIGHT:
		Ready_Fade_Text(0.5f, 1.f, 0.f, 1.f);
		break;
	case DTO::EUITextSubClassType::BSTTLE_WEAKNESS_END:
		break;
	case DTO::EUITextSubClassType::END:
	default:
		break;
	}
}

_bool CUIWeakness_Text::Tick_Visible_Event(const _float fTimeDelta)
{
	switch (m_eTextSubClassType)
	{
	case DTO::EUITextSubClassType::BATTLE_WEAKNESS_BEGIN:
		break;
	case DTO::EUITextSubClassType::BATTLE_WEAKNESS_TEXT:
		Tick_LerpChange(&m_fScaleOffset, fTimeDelta);
		break;
	case DTO::EUITextSubClassType::BATTLE_WEAKNESS_WORLD_TEXT_LEFT:
		return Tick_Lerp_Movement(fTimeDelta);
		break;
	case DTO::EUITextSubClassType::BATTLE_WEAKNESS_WORLD_TEXT_RIGHT:
		return Tick_Lerp_Movement(fTimeDelta);
		break;
	case DTO::EUITextSubClassType::BSTTLE_WEAKNESS_END:
		break;
	case DTO::EUITextSubClassType::END:
	default:
		break;
	}
	return false;
}

_bool CUIWeakness_Text::Tick_InVisible_Event(const _float fTimeDelta)
{
	switch (m_eTextSubClassType)
	{
	case DTO::EUITextSubClassType::BATTLE_WEAKNESS_BEGIN:
		break;
	case DTO::EUITextSubClassType::BATTLE_WEAKNESS_TEXT:
		return Tick_Fade_Text(fTimeDelta);
		break;
	case DTO::EUITextSubClassType::BATTLE_WEAKNESS_WORLD_TEXT_LEFT:
		return Tick_Fade_Text(fTimeDelta);
		break;
	case DTO::EUITextSubClassType::BATTLE_WEAKNESS_WORLD_TEXT_RIGHT:
		return Tick_Fade_Text(fTimeDelta);
		break;
	case DTO::EUITextSubClassType::BSTTLE_WEAKNESS_END:
		break;
	case DTO::EUITextSubClassType::END:
	default:
		break;
	}
	return false;
}

CUIWeakness_Text* CUIWeakness_Text::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CUIWeakness_Text* pInstance = new CUIWeakness_Text(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CUIWeakness_Text::Create, Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUIWeakness_Text::Clone(void* pArg)
{
	CUIWeakness_Text* pInstance = new CUIWeakness_Text(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CUIWeakness_Text::Clone, Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUIWeakness_Text::Free()
{
	Super::Free();
}
