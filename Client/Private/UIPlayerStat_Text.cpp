#include "pch.h"
#include "UIPlayerStat_Text.h"
#include "Client_Defines.h"
#include "Client_EventDefine.h"
//=================
// Component
//=================
#include "MainPlayer.h"
#include "Gun.h"
#include "StatCom_Player.h"
#include "Texture.h"
#include "Shader.h"
#include "VIBuffer_Rect_Tex.h"
#include "MyStat.h"
#include "GameInstance.h"

CUIPlayerStat_Text::CUIPlayerStat_Text(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	:CUIText(pDevice, pDeviceContext)
{
}

CUIPlayerStat_Text::CUIPlayerStat_Text(const CUIPlayerStat_Text& rhs)
	:CUIText(rhs)
{
}

HRESULT CUIPlayerStat_Text::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIPlayerStat_Text::Initialize(void* pArg)
{
	PLAYER_STAT_DESC* pDesc = static_cast<PLAYER_STAT_DESC*>(pArg);

	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;
	if (FAILED(Ready_Components(pDesc)))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIPlayerStat_Text::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;

	if (FAILED(Attach_Personal_Info()))
		return E_FAIL;

	return S_OK;
}


void CUIPlayerStat_Text::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CUIPlayerStat_Text::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
}

void CUIPlayerStat_Text::Update_Late(const _float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);
}

void CUIPlayerStat_Text::Ready_Before_Render(const _float fTimeDelta)
{
	if (FAILED(Convert_Stat_To_Text()))
		return;

	Super::Ready_Before_Render(fTimeDelta);
}

HRESULT CUIPlayerStat_Text::Render()
{
	if (!m_isVisible)
		return S_OK;
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;
	if (FAILED(Super::Render()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIPlayerStat_Text::Ready_Components(PLAYER_STAT_DESC* pDesc)
{
	if (FAILED(Super::Ready_Components(pDesc)))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIPlayerStat_Text::Bind_ShaderResources()
{
	CShader* pShader = Get_Component<CShader>();
	if (FAILED(Get_Component<CTransform>()->Bind_ShaderResource(pShader)))
		return E_FAIL;
	if (FAILED(Super::Bind_ShaderResources()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIPlayerStat_Text::Attach_Personal_Info()
{
	CGameObject* pResult = m_pGameInstance->Get_GameObject_Front(ENUM_TO_UINT(ELevelType::STATIC), g_wszPlayerLayer);
	if (nullptr == pResult)
		return E_FAIL;

	m_pPlayerStatCom = static_cast<CStatCom_Player*>(pResult->Get_Component<CMyStat>());
	if (nullptr == m_pPlayerStatCom)
		return E_FAIL;

	if (m_eTextSubClassType == DTO::EUITextSubClassType::PLAYER_STAT_TEXT_CUR_BULLET_COUNT ||
		m_eTextSubClassType == DTO::EUITextSubClassType::PLAYER_STAT_TEXT_MAX_BULLET_COUNT)
	{
		auto* pPlayer = static_cast<CMainPlayer*>(pResult);
		if (nullptr == pPlayer)
			return E_FAIL;

		m_pGunParts = pPlayer->Get_Part<CGun>(ENUM_TO_UINT(CPlayer::Part::GUN));
		if (nullptr == m_pGunParts)
			return E_FAIL;
	}

	m_pGameInstance->Subscribe<BOSS_STAGING_EVENT_START>([this]() { this->Set_Invisible(); });
	m_pGameInstance->Subscribe<BOSS_STAGING_EVENT_END>([this]() { this->Set_Visible(); });

	return S_OK;
}

HRESULT CUIPlayerStat_Text::Convert_Stat_To_Text()
{
	switch (m_eTextSubClassType)
	{
	case DTO::EUITextSubClassType::NONE_OWNER:
		break;
	case DTO::EUITextSubClassType::PLAYER_STAT_TEXT_BEGIN:
		m_wstrText = L"-";
		break;
	case DTO::EUITextSubClassType::PLAYER_STAT_TEXT_LV:
		break;
	case DTO::EUITextSubClassType::PLAYER_STAT_TEXT_HP:
		m_wstrText = Float_To_Wstring(m_pPlayerStatCom->Get_Stat_Vec2(CStatCom_Player::STAT_TYPE::HP).x, 0);
		break;

	case DTO::EUITextSubClassType::PLAYER_STAT_TEXT_ARMOR:
		m_wstrText = Float_To_Wstring(m_pPlayerStatCom->Get_Stat_Vec2(CStatCom_Player::STAT_TYPE::DEFENSE).x, 0);
		break;

	case DTO::EUITextSubClassType::PLAYER_STAT_TEXT_ENERGY:
		m_wstrText = Float_To_Wstring(m_pPlayerStatCom->Get_Stat_Vec2(CStatCom_Player::STAT_TYPE::MENTAL).x, 0);
		break;

	case DTO::EUITextSubClassType::PLAYER_STAT_TEXT_ESKILL_TYPE:
		m_wstrText = SKILL_TYPE_ToWstring(m_pPlayerStatCom->Get_Skill(CStatCom_Player::Attack_State::E).eSkillType);
		break;

	case DTO::EUITextSubClassType::PLAYER_STAT_TEXT_ESKILL_COOLTIME:
	{
		_float fMaxCool = m_pPlayerStatCom->Get_Skill(CStatCom_Player::Attack_State::E).tCoolTimer.fMaxTime;
		_float fCurCool = m_pPlayerStatCom->Get_Skill(CStatCom_Player::Attack_State::E).tCoolTimer.fTimeAcc;
		m_wstrText = Float_To_Wstring(fMaxCool - fCurCool, 1);
		break;
	}
	case DTO::EUITextSubClassType::PLAYER_STAT_TEXT_ESKILL_COST:
		m_wstrText = Float_To_Wstring(m_pPlayerStatCom->Get_Skill(CStatCom_Player::Attack_State::E).fNeedMental, 0);
		break;
	case DTO::EUITextSubClassType::PLAYER_STAT_TEXT_QSKILL_TYPE:
		m_wstrText = SKILL_TYPE_ToWstring(m_pPlayerStatCom->Get_Skill(CStatCom_Player::Attack_State::Q).eSkillType);
		break;
	case DTO::EUITextSubClassType::PLAYER_STAT_TEXT_QSKILL_COOLTIME:
	{
		_float fMaxCool = m_pPlayerStatCom->Get_Skill(CStatCom_Player::Attack_State::Q).tCoolTimer.fMaxTime;
		_float fCurCool = m_pPlayerStatCom->Get_Skill(CStatCom_Player::Attack_State::Q).tCoolTimer.fTimeAcc;
		m_wstrText = Float_To_Wstring(fMaxCool - fCurCool, 1);
		break;
	}
	case DTO::EUITextSubClassType::PLAYER_STAT_TEXT_QSKILL_COST:
		m_wstrText = Float_To_Wstring(m_pPlayerStatCom->Get_Skill(CStatCom_Player::Attack_State::Q).fNeedMental, 0);
		break;
	case DTO::EUITextSubClassType::PLAYER_STAT_TEXT_ZSKILL_TYPE:
		break;
	case DTO::EUITextSubClassType::PLAYER_STAT_TEXT_ZSKILL_COOLTIME:
		break;
	case DTO::EUITextSubClassType::PLAYER_STAT_TEXT_ZSKILL_COST:
		break;
	case DTO::EUITextSubClassType::PLAYER_STAT_TEXT_DODGESKILL_COOLTIME:
	{
		_float fMaxCool = m_pPlayerStatCom->Get_Timer(CStatCom_Player::TIMER_TYPE::DASH).fMaxTime;
		_float fCurCool = m_pPlayerStatCom->Get_Timer(CStatCom_Player::TIMER_TYPE::DASH).fTimeAcc;
		m_wstrText = Float_To_Wstring(fMaxCool - fCurCool, 1);
		break;
	}
	case DTO::EUITextSubClassType::PLAYER_STAT_TEXT_DODGESKILL_COUNT:
		m_wstrText = std::to_wstring(m_pPlayerStatCom->Get_Count(CStatCom_Player::TIMER_TYPE::DASH));
		break;
	case DTO::EUITextSubClassType::PLAYER_STAT_TEXT_MAX_BULLET_COUNT:
		m_wstrText = Float_To_Wstring(m_pGunParts->Get_TotalButtlet().x, 0);
		break;
	case DTO::EUITextSubClassType::PLAYER_STAT_TEXT_CUR_BULLET_COUNT:
		m_wstrText = Float_To_Wstring(m_pGunParts->Get_CurButtlet().x, 0);
		break;
	case DTO::EUITextSubClassType::END:
	default:
		return E_FAIL;
	}
	return S_OK;
}


void CUIPlayerStat_Text::OnUIEvent(ETriggerEventType eEvent, CGenericUI* pSender)
{
	if (!m_isActive)
		return;

	if (eEvent == ETriggerEventType::PRESS_ENTER)
	{
		if (m_isVisible)
			Set_Invisible();
		else
			Set_Visible();
	}
}

void CUIPlayerStat_Text::Initialize_Visible_Event()
{
	m_isActive		= false;
	m_isFin_Event	= false;
	Ready_Fade_Text(1.f, 0.f, 1.f, m_fDelay);
}

void CUIPlayerStat_Text::Initialize_InVisible_Event()
{
}

_bool CUIPlayerStat_Text::Tick_Visible_Event(const _float fTimeDelta)
{
	return Tick_Fade_Text(fTimeDelta);
}

_bool CUIPlayerStat_Text::Tick_InVisible_Event(const _float fTimeDelta)
{
	return true;
}

CUIPlayerStat_Text* CUIPlayerStat_Text::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CUIPlayerStat_Text* pInstance = new CUIPlayerStat_Text(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CUIPlayerStat_Text::Create, Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUIPlayerStat_Text::Clone(void* pArg)
{
	CUIPlayerStat_Text* pInstance = new CUIPlayerStat_Text(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CUIPlayerStat_Text::Clone, Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUIPlayerStat_Text::Free()
{
	Super::Free();
}
