#include "pch.h"
#include "UIDamageFont_Text.h"
#include "Client_Defines.h"

//=================
// Component
//=================
#include "StatCom_Player.h"
#include "Texture.h"
#include "Shader.h"
#include "VIBuffer_Rect_Tex.h"
#include "GameInstance.h"

CUIDamageFont_Text::CUIDamageFont_Text(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	:CUIText(pDevice, pDeviceContext)
{
}

CUIDamageFont_Text::CUIDamageFont_Text(const CUIDamageFont_Text& rhs)
	:CUIText(rhs)
{
}

HRESULT CUIDamageFont_Text::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIDamageFont_Text::Initialize(void* pArg)
{
	DAMAGE_FONT_DESC* pDesc = static_cast<DAMAGE_FONT_DESC*>(pArg);
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;
	if (FAILED(Ready_Components(pDesc)))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIDamageFont_Text::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;

	if (FAILED(Attach_Personal_Info()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUIDamageFont_Text::Attach_Personal_Info()
{
	CGameObject* pResult = m_pGameInstance->Get_GameObject_Front(ENUM_TO_UINT(ELevelType::STATIC), g_wszPlayerLayer);
	if (nullptr == pResult)
		return E_FAIL;

	m_pPlayerStatCom = static_cast<CStatCom_Player*>(pResult->Get_Component<CMyStat>());
	if (nullptr == m_pPlayerStatCom)
		return E_FAIL;

	return S_OK;
}

void CUIDamageFont_Text::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CUIDamageFont_Text::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
}

void CUIDamageFont_Text::Update_Late(const _float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);
}

void CUIDamageFont_Text::Ready_Before_Render(const _float fTimeDelta)
{
	if (FAILED(Convert_Stat_To_Text()))
		return;

	Super::Ready_Before_Render(fTimeDelta);
}

HRESULT CUIDamageFont_Text::Render()
{
	if (!m_isVisible)
		return S_OK;
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;
	if (FAILED(Super::Render()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIDamageFont_Text::Ready_Components(DAMAGE_FONT_DESC* pDesc)
{
	if (FAILED(Super::Ready_Components(pDesc)))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIDamageFont_Text::Bind_ShaderResources()
{
	CShader* pShader = Get_Component<CShader>();
	if (FAILED(Get_Component<CTransform>()->Bind_ShaderResource(pShader)))
		return E_FAIL;
	if (FAILED(Super::Bind_ShaderResources()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIDamageFont_Text::Convert_Stat_To_Text()
{
	switch (m_eTextSubClassType)
	{
	case DTO::EUITextSubClassType::BATTLE_DAMAGE_TEXT_BEGIN:
		break;
	case DTO::EUITextSubClassType::BATTLE_DAMAGE_TEXT_COMMON:
		break;
	case DTO::EUITextSubClassType::BATTLE_DAMAGE_TEXT_HIT:
		break;
	case DTO::EUITextSubClassType::BATTLE_DAMAGE_TEXT_CRITCAL:
		break;
	case DTO::EUITextSubClassType::BATTLE_DAMAGE_TEXT_CRITICAL_DAMAGE:
		break;
	case DTO::EUITextSubClassType::BATTLE_DAMAGE_TEXT_END:
		break;
	case DTO::EUITextSubClassType::END:
	default:
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CUIDamageFont_Text::Tick_By_Type(const _float fTimeDelta)
{
	switch (m_eTextSubClassType)
	{
	case DTO::EUITextSubClassType::BATTLE_DAMAGE_TEXT_BEGIN:
		break;
	case DTO::EUITextSubClassType::BATTLE_DAMAGE_TEXT_COMMON:
		// 개크게 나왔다가 줄어들기 일정시간 지나면 사라지기 
		break;
	case DTO::EUITextSubClassType::BATTLE_DAMAGE_TEXT_HIT:
		// 올라왔다가 다시 내려가기
		break;
	case DTO::EUITextSubClassType::BATTLE_DAMAGE_TEXT_CRITCAL:
		// 개크게 빛나게 나
		break;
	case DTO::EUITextSubClassType::BATTLE_DAMAGE_TEXT_CRITICAL_DAMAGE:
		break;
	case DTO::EUITextSubClassType::BATTLE_DAMAGE_TEXT_END:
		break;
	case DTO::EUITextSubClassType::END:
	default:
		return E_FAIL;
	}
}

void CUIDamageFont_Text::OnUIEvent(ETriggerEventType eEvent, CGenericUI* pSender)
{
	if (!m_isActive)
		return;
}

void CUIDamageFont_Text::Initialize_Visible_Event()
{
	m_isActive = false;
	m_isFin_Event = false;
}

void CUIDamageFont_Text::Initialize_InVisible_Event()
{
}

_bool CUIDamageFont_Text::Tick_Visible_Event(const _float fTimeDelta)
{
	m_isActive = true;
	m_isFin_Event = true;
	return true;
}

_bool CUIDamageFont_Text::Tick_InVisible_Event(const _float fTimeDelta)
{
	return true;
}

CUIDamageFont_Text* CUIDamageFont_Text::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CUIDamageFont_Text* pInstance = new CUIDamageFont_Text(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CUIDamageFont_Text::Create, Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUIDamageFont_Text::Clone(void* pArg)
{
	CUIDamageFont_Text* pInstance = new CUIDamageFont_Text(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CUIDamageFont_Text::Clone, Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUIDamageFont_Text::Free()
{
	Super::Free();
}
