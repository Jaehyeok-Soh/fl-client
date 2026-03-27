#include "pch.h"
#include "UIBossAction_Text.h"
#include "Client_Defines.h"
#include "Client_EventDefine.h"

//=================
// Component
//=================
#include "StatCom_Player.h"
#include "Texture.h"
#include "Shader.h"
#include "VIBuffer_Rect_Tex.h"
#include "GameInstance.h"

CUIBossAction_Text::CUIBossAction_Text(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	:CUIText(pDevice, pDeviceContext)
{
}

CUIBossAction_Text::CUIBossAction_Text(const CUIBossAction_Text& rhs)
	:CUIText(rhs)
{
}

HRESULT CUIBossAction_Text::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIBossAction_Text::Initialize(void* pArg)
{
	BOSS_ACTION_TEXT_DESC* pDesc = static_cast<BOSS_ACTION_TEXT_DESC*>(pArg);
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;
	if (FAILED(Ready_Components(pDesc)))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIBossAction_Text::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;

	if (FAILED(Attach_Personal_Info()))
		return E_FAIL;

	return S_OK;
}


void CUIBossAction_Text::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CUIBossAction_Text::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
}

void CUIBossAction_Text::Update_Late(const _float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);
}

void CUIBossAction_Text::Ready_Before_Render(const _float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);

	Tick_By_Type(fTimeDelta);

	if (FAILED(Convert_Stat_To_Text()))
		return;
}

HRESULT CUIBossAction_Text::Render()
{
	if (!m_isVisible)
		return S_OK;
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;
	if (FAILED(Super::Render()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIBossAction_Text::Ready_Components(BOSS_ACTION_TEXT_DESC* pDesc)
{
	if (FAILED(Super::Ready_Components(pDesc)))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIBossAction_Text::Bind_ShaderResources()
{
	CShader* pShader = Get_Component<CShader>();
	if (FAILED(Get_Component<CTransform>()->Bind_ShaderResource(pShader)))
		return E_FAIL;
	if (FAILED(Super::Bind_ShaderResources()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIBossAction_Text::Attach_Personal_Info()
{
	return S_OK;
}

void CUIBossAction_Text::Tick_By_Type(const _float fTimeDelta)
{
}

HRESULT CUIBossAction_Text::Convert_Stat_To_Text()
{
	return S_OK;
}

void CUIBossAction_Text::Bind_Events()
{
	switch (m_eTextSubClassType)
	{
	case DTO::EUITextSubClassType::BOSS_CIVILA_ACTION_BEGIN:
		break;
	case DTO::EUITextSubClassType::BOSS_CIVILA_ACTION_WORLD_TEXT:
		m_vecEventHandles.push_back(
			m_pGameInstance->Subscribe<XIBILA_BOSS_ACTION_ON>([this]()
				{
					this->Set_Visible();
				}));
		m_vecEventHandles.push_back(
			m_pGameInstance->Subscribe<XIBILA_BOSS_ACTION_OFF>([this]()
				{
					this->Set_Invisible();
				}));
		break;
	case DTO::EUITextSubClassType::BOSS_CIVILA_ACTION_NAME_TEXT:
		m_vecEventHandles.push_back(
			m_pGameInstance->Subscribe<XIBILA_BOSS_ACTION_ON>([this]()
				{
					this->Set_Visible();
				}));
		m_vecEventHandles.push_back(
			m_pGameInstance->Subscribe<XIBILA_BOSS_ACTION_OFF>([this]()
				{
					this->Set_Invisible();
				}));
		break;
	case DTO::EUITextSubClassType::BOSS_CIVILA_ACTION_NAME_NIGHTMARE_TEXT:
		m_vecEventHandles.push_back(
			m_pGameInstance->Subscribe<XIBILA_BOSS_ACTION_ON>([this]()
				{
					this->Set_Visible();
				}));
		m_vecEventHandles.push_back(
			m_pGameInstance->Subscribe<XIBILA_BOSS_ACTION_OFF>([this]()
				{
					this->Set_Invisible();
				}));
		break;
	case DTO::EUITextSubClassType::BOSS_CIVILA_ACTION_END:
		break;
	}
}

void CUIBossAction_Text::Initialize_Visible_Event()
{
	switch (m_eTextSubClassType)
	{
	case DTO::EUITextSubClassType::BOSS_CIVILA_ACTION_BEGIN:
		break;
	case DTO::EUITextSubClassType::BOSS_CIVILA_ACTION_WORLD_TEXT:
		Ready_Fade_Text(1.f, 0.f, 1.f, 0.5f);
		break;
	case DTO::EUITextSubClassType::BOSS_CIVILA_ACTION_NAME_TEXT:
		Ready_Lerp_Movement(Vec2{ 10.f, 0.f }, Vec2{ 0.f, 0.f }, 1.f, 0.7f, m_fDelay);
		Ready_Fade_Text(1.f, 0.f, 1.f, m_fDelay);
		break;
	case DTO::EUITextSubClassType::BOSS_CIVILA_ACTION_NAME_NIGHTMARE_TEXT:
		Ready_Fade_Text(1.f, 0.f, 1.f, 0.5f);
		break;
	case DTO::EUITextSubClassType::BOSS_CIVILA_ACTION_END:
		break;
	}
}

void CUIBossAction_Text::Initialize_InVisible_Event()
{
	switch (m_eTextSubClassType)
	{
	case DTO::EUITextSubClassType::BOSS_CIVILA_ACTION_BEGIN:
		break;
	case DTO::EUITextSubClassType::BOSS_CIVILA_ACTION_WORLD_TEXT:
		Ready_Fade_Text(0.5f, 1.f, 0.f, m_fDelay);
		break;
	case DTO::EUITextSubClassType::BOSS_CIVILA_ACTION_NAME_TEXT:
		Ready_Lerp_Movement(Vec2{ 0.f, 0.f }, Vec2{ 10.f, 0.f }, 1.f, 0.7f, m_fDelay);
		Ready_Fade_Text(1.f, 1.f, 0.f, m_fDelay);
		break;
	case DTO::EUITextSubClassType::BOSS_CIVILA_ACTION_NAME_NIGHTMARE_TEXT:
		Ready_Fade_Text(0.5f, 1.f, 0.f, m_fDelay);
		break;
	case DTO::EUITextSubClassType::BOSS_CIVILA_ACTION_END:
		break;
	}
}

_bool CUIBossAction_Text::Tick_Visible_Event(const _float fTimeDelta)
{
	switch (m_eTextSubClassType)
	{
	case DTO::EUITextSubClassType::BOSS_CIVILA_ACTION_BEGIN:
		break;
	case DTO::EUITextSubClassType::BOSS_CIVILA_ACTION_WORLD_TEXT:
		return Tick_Fade_Text(fTimeDelta);
	case DTO::EUITextSubClassType::BOSS_CIVILA_ACTION_NAME_TEXT:
	{
		_bool isMove = Tick_Lerp_Movement(fTimeDelta);
		_bool isFade = Tick_Fade_Text(fTimeDelta);
		if (isMove && isFade)
			return true;
	}
	break;
	case DTO::EUITextSubClassType::BOSS_CIVILA_ACTION_NAME_NIGHTMARE_TEXT:
		return Tick_Fade_Text(fTimeDelta);
	case DTO::EUITextSubClassType::BOSS_CIVILA_ACTION_END:
		break;
	}

	return false;
}

_bool CUIBossAction_Text::Tick_InVisible_Event(const _float fTimeDelta)
{
	switch (m_eTextSubClassType)
	{
	case DTO::EUITextSubClassType::BOSS_CIVILA_ACTION_BEGIN:
		break;
	case DTO::EUITextSubClassType::BOSS_CIVILA_ACTION_WORLD_TEXT:
		return Tick_Fade_Text(fTimeDelta);
	case DTO::EUITextSubClassType::BOSS_CIVILA_ACTION_NAME_TEXT:
	{
		_bool isMove = Tick_Lerp_Movement(fTimeDelta);
		_bool isFade = Tick_Fade_Text(fTimeDelta);
		if (isMove && isFade)
			return true;
	}
	break;
	case DTO::EUITextSubClassType::BOSS_CIVILA_ACTION_NAME_NIGHTMARE_TEXT:
		return Tick_Fade_Text(fTimeDelta);
	case DTO::EUITextSubClassType::BOSS_CIVILA_ACTION_END:
		break;
	}
	return false;
}

CUIBossAction_Text* CUIBossAction_Text::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CUIBossAction_Text* pInstance = new CUIBossAction_Text(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CUIBossAction_Text::Create, Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUIBossAction_Text::Clone(void* pArg)
{
	CUIBossAction_Text* pInstance = new CUIBossAction_Text(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CUIBossAction_Text::Clone, Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUIBossAction_Text::Free()
{
	Super::Free();
}
