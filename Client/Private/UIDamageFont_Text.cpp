#include "pch.h"
#include "UIDamageFont_Text.h"
#include "Client_Defines.h"

//=================
// Component
//=================
#include "WorldUI_Component.h"
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

	if (m_isSpawned)
	{
		Set_Visible();
		m_isSpawned = false;
	}

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
	Tick_By_Type(fTimeDelta);
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
		// 크게 나왔다가 줄어들기 일정시간 지나면 사라지기 
		break;
	case DTO::EUITextSubClassType::BATTLE_DAMAGE_TEXT_HIT:
		// 올라왔다가 다시 내려가기
		break;
	case DTO::EUITextSubClassType::BATTLE_DAMAGE_TEXT_CRITCAL:
	{
		if (m_isFin_Event)
		{
			//m_vFontColor.x = 1.f + ((m_vOriginFontColor.x - 1.f) * t);
			//m_vFontColor.y = 1.f + ((m_vOriginFontColor.y - 1.f) * t);
			//m_vFontColor.z = 1.f + ((m_vOriginFontColor.z - 1.f) * t);
			//m_vFontColor.w = 1.f + ((m_vOriginFontColor.w - 1.f) * t);
		}
	}
	break;
	case DTO::EUITextSubClassType::BATTLE_DAMAGE_TEXT_CRITICAL_DAMAGE:
		// 
		break;
	case DTO::EUITextSubClassType::BATTLE_DAMAGE_TEXT_END:
		break;
	case DTO::EUITextSubClassType::END:
	default:
		return E_FAIL;
	}
	return S_OK;
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
	switch (m_eTextSubClassType)
	{
	case DTO::EUITextSubClassType::BATTLE_DAMAGE_TEXT_BEGIN:
		break;
	case DTO::EUITextSubClassType::BATTLE_DAMAGE_TEXT_COMMON:
		break;
	case DTO::EUITextSubClassType::BATTLE_DAMAGE_TEXT_HIT:
		break;
	case DTO::EUITextSubClassType::BATTLE_DAMAGE_TEXT_CRITCAL:
	case DTO::EUITextSubClassType::BATTLE_DAMAGE_TEXT_CRITICAL_DAMAGE:
	{
		m_fTimeAcc += fTimeDelta;
		const _float fDuration = 0.1f;
		_float t = m_fTimeAcc / fDuration;
		if (1.f < t)
		{
			m_fDamageFontScaleOffet = 1.f;
			m_pWorldUIComp->Request_ScaleOffset(m_fDamageFontScaleOffet);
			m_vFontColor = m_vOriginFontColor;
			m_isActive = true;
			m_isFin_Event = true;
			return true;
		}

		m_fDamageFontScaleOffet = 10.f + ((-9.f) * t);
		m_pWorldUIComp->Request_ScaleOffset(m_fDamageFontScaleOffet);
		return false;
	}
	break;
	case DTO::EUITextSubClassType::BATTLE_DAMAGE_TEXT_END:
		break;
	case DTO::EUITextSubClassType::END:
	default:
		return true;
	}

	return false;
}

_bool CUIDamageFont_Text::Tick_InVisible_Event(const _float fTimeDelta)
{
	return true;
}

HRESULT CUIDamageFont_Text::Spawn_FromPool(void* pArg)
{
	UI_PREFAB_DATA* pDesc = static_cast<UI_PREFAB_DATA*>(pArg);

	auto* pComp = Get_Script_Component(L"WorldUIComponent");
	if (nullptr == pComp)
		return E_FAIL;

	m_pWorldUIComp = static_cast<CWorldUI_Component*>(pComp);
	if (nullptr == m_pWorldUIComp)
		return E_FAIL;

	m_pWorldUIComp->Set_TargetPos(pDesc->DamageFontData.vHitPos);
	m_bDead = false;

	switch (m_eTextSubClassType)
	{
	case DTO::EUITextSubClassType::BATTLE_DAMAGE_TEXT_BEGIN:
		break;
	case DTO::EUITextSubClassType::BATTLE_DAMAGE_TEXT_COMMON:
		break;
	case DTO::EUITextSubClassType::BATTLE_DAMAGE_TEXT_HIT:
		break;
	case DTO::EUITextSubClassType::BATTLE_DAMAGE_TEXT_CRITCAL:
	{
		m_vOriginFontColor			= pDesc->DamageFontData.vFontColor;
		m_fDamageFontScaleOffet		= 3.f;
		m_fTimeAcc					= 0.f;
		m_isSpawned = true;
	}
	break;
	case DTO::EUITextSubClassType::BATTLE_DAMAGE_TEXT_CRITICAL_DAMAGE:
	{
		m_vOriginFontColor			= pDesc->DamageFontData.vFontColor;
		m_fDamageFontScaleOffet		= 3.f;
		m_fTimeAcc					= 0.f;
		m_isSpawned					= true;
	}
	break;
	case DTO::EUITextSubClassType::BATTLE_DAMAGE_TEXT_END:
		break;
	case DTO::EUITextSubClassType::END:
	default:
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CUIDamageFont_Text::Despawn_FromPool()
{
	m_vFontColor = m_vOriginFontColor;
	m_fDamageFontScaleOffet = 1.f;
	m_isVisible = false;
	m_isPreVisible = false;
	m_isVisibleTrigger = false;
	m_isSpawned = false;
	return S_OK;
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
