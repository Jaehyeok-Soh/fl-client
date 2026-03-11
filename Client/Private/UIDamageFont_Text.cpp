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

#define DESTROY_TIME 0.5f

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
	if (FAILED(Convert_Stat_To_Text()))
		return;
}

void CUIDamageFont_Text::Ready_Before_Render(const _float fTimeDelta)
{
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

void CUIDamageFont_Text::Tick_By_Type(const _float fTimeDelta)
{
	switch (m_eTextSubClassType)
	{
	case DTO::EUITextSubClassType::BATTLE_DAMAGE_TEXT_BEGIN:
		break;
	case DTO::EUITextSubClassType::BATTLE_DAMAGE_TEXT_COMMON:
	{
		if (m_isFinVisibleEvent)
		{
			if (Tick_Lerp_Movement(fTimeDelta))
				Set_Invisible();
		}
	}
	break;
	case DTO::EUITextSubClassType::BATTLE_DAMAGE_TEXT_HIT:
	{
		if (m_isFinVisibleEvent)
		{
			m_isFin_HitFontEvent = Tick_Lerp_Movement(fTimeDelta);

			if (m_isFin_HitFontEvent)
			{
				if (!m_isHitFontEventTrigger)
				{
					m_isHitFontEventTrigger = true;
				}
				else
				{
					Set_Invisible();
					break;
				}

				if (m_isHitFontEventTrigger)
				{
					Ready_Lerp_Movement(Vec2{ 0.f, -10.f }, Vec2{ 0.f, 20.f }, 0.7f, 1.5f, m_fDelay);
				}
			}
		}
	}
	break;
	case DTO::EUITextSubClassType::BATTLE_DAMAGE_TEXT_CRITCAL:
	{
		if (m_isFinVisibleEvent)
		{
			m_fDamageFont_TimeAcc += fTimeDelta;
			_float t = m_fDamageFont_TimeAcc / DESTROY_TIME;

			if (t > 1.f)
			{
				m_vFontColor = m_vOriginFontColor;
				Set_Invisible();
				break;
			}
			m_vFontColor.x = 1.f + ((m_vOriginFontColor.x - 1.f) * t);
			m_vFontColor.y = 1.f + ((m_vOriginFontColor.y - 1.f) * t);
			m_vFontColor.z = 1.f + ((m_vOriginFontColor.z - 1.f) * t);
		}
	}
	break;
	case DTO::EUITextSubClassType::BATTLE_DAMAGE_TEXT_CRITICAL_DAMAGE:
	{
		if (m_isFinVisibleEvent)
		{
			m_fDamageFont_TimeAcc += fTimeDelta;
			m_vFontColor = m_vOriginFontColor;

			if (m_fDamageFont_TimeAcc > DESTROY_TIME)
				Set_Invisible();
		}
	}
	break;
	case DTO::EUITextSubClassType::BATTLE_DAMAGE_TEXT_END:
		break;
	case DTO::EUITextSubClassType::END:
	default:
		return;
	}

	return;
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

void CUIDamageFont_Text::Initialize_Visible_Event()
{
	m_isActive = false;
	m_isFin_Event = false;
	m_isFinVisibleEvent = false;
}

void CUIDamageFont_Text::Initialize_InVisible_Event()
{
	m_isActive = false;
	m_isFin_Event = false;
	m_isFinVisibleEvent = false;
	Ready_Fade_Text(1.f, 1.f, 0.f, m_fDelay);
}

_bool CUIDamageFont_Text::Tick_Visible_Event(const _float fTimeDelta)
{
	switch (m_eTextSubClassType)
	{
	case DTO::EUITextSubClassType::BATTLE_DAMAGE_TEXT_BEGIN:
		break;
	case DTO::EUITextSubClassType::BATTLE_DAMAGE_TEXT_COMMON:
	{
		m_fDamageFont_TimeAcc += fTimeDelta;
		_float t = m_fDamageFont_TimeAcc / 0.1f;
		if (t >= 1.f)
		{
			Ready_Lerp_Movement(Vec2{ 0.f, 0.f }, Vec2{ 0.f, -30.f }, DESTROY_TIME, 3.f, m_fDelay);

			m_fDamageFontScaleOffet		= 1.f;
			m_pWorldUIComp->Request_ScaleOffset(m_fDamageFontScaleOffet);
			m_isActive					= true;
			m_isFinVisibleEvent			= true;
			m_fDamageFont_TimeAcc		= 0.f;
			return true;
		}
		m_fDamageFontScaleOffet			= 1.5f + ((-0.5f) * t);
		m_pWorldUIComp->Request_ScaleOffset(m_fDamageFontScaleOffet);
	}
	break;
	case DTO::EUITextSubClassType::BATTLE_DAMAGE_TEXT_HIT:
	{
		Ready_Lerp_Movement(Vec2{ 0.f, 0.f }, Vec2{ 0.f, -10.f }, 0.5f, 0.5f, m_fDelay);
		m_isActive						= true;
		m_isFin_Event					= true;
		m_isFinVisibleEvent = true;

		m_isFin_HitFontEvent			= false;
		m_isHitFontEventTrigger			= false;
		return true;
	}
	break;
	case DTO::EUITextSubClassType::BATTLE_DAMAGE_TEXT_CRITCAL:
	case DTO::EUITextSubClassType::BATTLE_DAMAGE_TEXT_CRITICAL_DAMAGE:
	{
		m_fDamageFont_TimeAcc += fTimeDelta;
		const _float fDuration			= 0.1f;
		_float t						= m_fDamageFont_TimeAcc / fDuration;
		if (1.f < t)
		{
			m_fDamageFontScaleOffet		= 1.f;
			m_pWorldUIComp->Request_ScaleOffset(m_fDamageFontScaleOffet);
			m_vFontColor				= Vec4{1.f, 1.f, 1.f, 1.f};
			m_isActive					= true;
			m_isFin_Event				= true;
			m_isFinVisibleEvent			= true;
			m_fDamageFont_TimeAcc		= 0.f;
			return true;
		}
		m_fDamageFontScaleOffet			= 5.f + ((-4.f) * t);
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
	_bool is = Tick_Fade_Text(fTimeDelta);
	if (is)
	{
		Request_SetDead();
		m_isActive = true;
		m_isFin_Event = true;
		m_fDamageFont_TimeAcc = 0.f;
		m_isFinVisibleEvent = false;
		m_isHitFontEventTrigger = false;
		m_isFin_HitFontEvent = false;
		return true;
	}
	return false;
}

HRESULT CUIDamageFont_Text::Spawn_FromPool(void* pArg)
{
	if (FAILED(Super::Spawn_FromPool(pArg)))
		return E_FAIL;

	UI_PREFAB_DATA* pDesc = static_cast<UI_PREFAB_DATA*>(pArg);
	auto* pComp = Get_Script_Component(L"WorldUIComponent");
	if (nullptr == pComp)
		return E_FAIL;
	m_pWorldUIComp = static_cast<CWorldUI_Component*>(pComp);
	if (nullptr == m_pWorldUIComp)
		return E_FAIL;

	m_isDeadRequest				= false;
	m_isFinVisibleEvent			= false;
	m_isFin_HitFontEvent		= false;
	m_isHitFontEventTrigger		= false;
	m_fDamageFontScaleOffet		= 1.f;

	if (auto* pDamageFont = std::get_if<UI_DAMAGEFONT_PREFAB_DATA>(&pDesc->Data))
	{
		_float x = pDamageFont->vRandOffset.x;
		_float y = pDamageFont->vRandOffset.y;
		_float z = pDamageFont->vRandOffset.z;

		if (m_eTextSubClassType == DTO::EUITextSubClassType::BATTLE_DAMAGE_TEXT_CRITICAL_DAMAGE ||
			m_eTextSubClassType == DTO::EUITextSubClassType::BATTLE_DAMAGE_TEXT_CRITCAL)
		{
			if(pDamageFont->vHitPos.x > 100000 || pDamageFont->vHitPos.x < -100000)
				m_pWorldUIComp->Set_Target(pDamageFont->pTarget);
			else
				m_pWorldUIComp->Set_TargetPos(Vec3{ pDamageFont->vHitPos.x,  pDamageFont->vHitPos.y, pDamageFont->vHitPos.z});
		}
		else
		{
			if (pDamageFont->vHitPos.x > 100000 || pDamageFont->vHitPos.x < -100000)
				m_pWorldUIComp->Set_Target(pDamageFont->pTarget);
			else
				m_pWorldUIComp->Set_TargetPos(Vec3{ pDamageFont->vHitPos.x + x,  pDamageFont->vHitPos.y + y, pDamageFont->vHitPos.z + z });
		}

		switch (m_eTextSubClassType)
		{
		case DTO::EUITextSubClassType::BATTLE_DAMAGE_TEXT_BEGIN:
			break;
		case DTO::EUITextSubClassType::BATTLE_DAMAGE_TEXT_COMMON:
		{
			m_wstrText				= std::to_wstring(pDamageFont->iDamage);
			m_vOriginFontColor		= pDamageFont->vFontColor;
			m_vFontColor			= m_vOriginFontColor;
			m_fDamageFontScaleOffet = 1.5f;
			m_isSpawned				= true;
		}
		break;
		case DTO::EUITextSubClassType::BATTLE_DAMAGE_TEXT_HIT:
		{
			m_wstrText				= std::to_wstring(pDamageFont->iDamage);
			m_vOriginFontColor		= m_vFontColor;
			m_isSpawned				= true;
		}
		break;
		case DTO::EUITextSubClassType::BATTLE_DAMAGE_TEXT_CRITICAL_DAMAGE:
		{
			m_wstrText				= std::to_wstring(pDamageFont->iDamage) + L"!";
		}
		case DTO::EUITextSubClassType::BATTLE_DAMAGE_TEXT_CRITCAL:
		{
			m_vOriginFontColor		= pDamageFont->vFontColor;
			m_vFontColor			= m_vOriginFontColor;
			m_fDamageFontScaleOffet = 5.f;
			m_isSpawned				= true;
		}
		break;
		case DTO::EUITextSubClassType::BATTLE_DAMAGE_TEXT_END:
			break;
		case DTO::EUITextSubClassType::END:
		default:
			return E_FAIL;
		}
	}
	return S_OK;
}

HRESULT CUIDamageFont_Text::Despawn_FromPool()
{
	if (FAILED(Super::Despawn_FromPool()))
		return E_FAIL;

	m_vFontColor				= m_vOriginFontColor;
	m_fDamageFontScaleOffet		= 1.f;
	m_isVisible					= false;
	m_isPreVisible				= false;
	m_isVisibleTrigger			= false;
	m_isSpawned					= false;
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
