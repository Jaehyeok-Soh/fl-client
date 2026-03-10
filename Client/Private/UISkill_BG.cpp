#include "pch.h"
#include "UISkill_BG.h"
#include "Client_Defines.h"
#include "Client_EventDefine.h"
//=================
// Component
//=================
#include "StatCom_Player.h"
#include "StateBase_Player.h"
#include "Texture.h"
#include "Shader.h"
#include "VIBuffer_Rect_Tex.h"
#include "MyStat.h"
#include "UI_Manager.h"
#include "GameInstance.h"

CUISkill_BG::CUISkill_BG(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	:CUIDynamic_Image(pDevice, pDeviceContext)
{
}

CUISkill_BG::CUISkill_BG(const CUISkill_BG& rhs)
	:CUIDynamic_Image(rhs)
{
}

HRESULT CUISkill_BG::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUISkill_BG::Initialize(void* pArg)
{
	SKILL_BG_DESC* pDesc = static_cast<SKILL_BG_DESC*>(pArg);
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;
	if (FAILED(Ready_Components(pDesc)))
		return E_FAIL;
	return S_OK;
}

HRESULT CUISkill_BG::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;

	if (FAILED(Attach_Personal_Info()))
		return E_FAIL;
	return S_OK;
}

void CUISkill_BG::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
	Trigger_User_Use_Skill();
}

void CUISkill_BG::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
	Tick_Use_Skill_Event(fTimeDelta);
}

void CUISkill_BG::Update_Late(const _float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);
}

void CUISkill_BG::Ready_Before_Render(const _float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);
}

HRESULT CUISkill_BG::Render()
{
	if (!m_isVisible)
		return S_OK;
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;
	if (FAILED(Super::Render()))
		return E_FAIL;
	return S_OK;
}

void CUISkill_BG::Trigger_User_Use_Skill()
{
}

void CUISkill_BG::Tick_Use_Skill_Event(const _float fTimeDelta)
{
	if (m_isUsingE)
	{
		_bool is = Tick_Fade(fTimeDelta);
		if (is)
		{
			if (m_isFinUseE)
			{
				m_isUsingE = false;
				m_isFinUseE = false;
			}
			else
			{
				Ready_Fade(0.3f, 1.f, 0.f, m_fDelay);
				m_isFinUseE = true;
			}
		}
	}
	else if (m_isUsingSkill)
	{
		_bool is = { false };

		if (!m_isSkillFlash)
			is = Tick_Fade(fTimeDelta);
		else
		{
			switch (m_eDImageSubClass)
			{
			case DTO::EUIDImageSubClassType::PLAYER_Q:
				m_fProgress_Ratio = 1.f - (m_pPlayerStatCom->Get_Skill(CStatCom_Player::Attack_State::Q).tCoolTimer.fTimeAcc / m_fMaxCoolTime);
				break;
			case DTO::EUIDImageSubClassType::PLAYER_DODGE:
				m_fProgress_Ratio = 1.f - (m_pPlayerStatCom->Get_Timer(CStatCom_Player::TIMER_TYPE::DASH).fTimeAcc / m_fMaxCoolTime);
				break;
			}
			if (m_fProgress_Ratio <= 0.01f)
			{                                                                                                                                     
				m_isUsingSkill = false;
				m_fProgress_Ratio = 0.f;
			}
			return;
		}
		if (is)
		{
			if (!m_isSkillFlash)
			{
				m_isSkillFlash = true;
			}
		}
	}

	if (m_eDImageSubClass == DTO::EUIDImageSubClassType::PLAYER_DODGE)
	{
		m_fProgress_Ratio = 1.f - (m_pPlayerStatCom->Get_Timer(CStatCom_Player::TIMER_TYPE::DASH).fTimeAcc / m_fMaxCoolTime);

		if (m_fProgress_Ratio >= 0.99f)
			m_fProgress_Ratio = 0.f;
	}
}

HRESULT CUISkill_BG::Ready_Components(SKILL_BG_DESC* pDesc)
{
	if (FAILED(Super::Ready_Components(pDesc)))
		return E_FAIL;
	return S_OK;
}

HRESULT CUISkill_BG::Bind_ShaderResources()
{
	CShader* pShader = Get_Component<CShader>();
	if (FAILED(Get_Component<CTransform>()->Bind_ShaderResource(pShader)))
		return E_FAIL;
	if (FAILED(Super::Bind_ShaderResources()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUISkill_BG::Attach_Personal_Info()
{
	CGameObject* pResult = m_pGameInstance->Get_GameObject_Front(ENUM_TO_UINT(ELevelType::STATIC), g_wszPlayerLayer);
	if (nullptr == pResult)
		return E_FAIL;
	m_pPlayerStatCom = static_cast<CStatCom_Player*>(pResult->Get_Component<CMyStat>());
	if (nullptr == m_pPlayerStatCom)
		return E_FAIL;

	switch (m_eDImageSubClass)
	{
	case DTO::EUIDImageSubClassType::NONE_OWNER:
		break;
	case DTO::EUIDImageSubClassType::PLAYER_E:
		m_fMaxCoolTime = m_pPlayerStatCom->Get_Skill(CStatCom_Player::E).tCoolTimer.fMaxTime;
		m_pGameInstance->Subscribe<PLAYER_SKILL_TRIGGERED>([this](_uint iKey)
			{
				if (static_cast<CStateBase_Player::STATEKEY>(iKey) == CStateBase_Player::STATEKEY::E)
				{
					this->Ready_Fade(0.3f, 0.f, 0.7f, m_fDelay);
					this->m_isUsingE = true;
					this->m_isFinUseE = false;
				}
			});
		break;
	case DTO::EUIDImageSubClassType::PLAYER_Q:
		m_fMaxCoolTime = m_pPlayerStatCom->Get_Skill(CStatCom_Player::Q).tCoolTimer.fMaxTime;
		m_pGameInstance->Subscribe<PLAYER_SKILL_TRIGGERED>([this](_uint iKey)
			{
				if (static_cast<CStateBase_Player::STATEKEY>(iKey) == CStateBase_Player::STATEKEY::Q)
				{
					this->Ready_Fade(0.2f, 0.f, 0.7f, m_fDelay);
					this->m_isUsingSkill = true;
					this->m_isSkillFlash = false;
				}
			});
		break;
	case DTO::EUIDImageSubClassType::PLAYER_Z:
		break;
	case DTO::EUIDImageSubClassType::PLAYER_GUN:
		break;
	case DTO::EUIDImageSubClassType::PLAYER_DODGE:
		m_fMaxCoolTime = m_pPlayerStatCom->Get_Timer(CStatCom_Player::TIMER_TYPE::DASH).fMaxTime;
		m_fAlpha_Ratio = 0.7f;
		//m_pGameInstance->Subscribe<PLAYER_SKILL_TRIGGERED>([this](_uint iKey)
		//	{
		//		if (static_cast<CStateBase_Player::STATEKEY>(iKey) == CStateBase_Player::STATEKEY::SHIFT)
		//		{
		//			this->Ready_Fade(0.2f, 0.f, 0.7f, m_fDelay);
		//			this->m_isUsingSkill = true;
		//			this->m_isSkillFlash = false;
		//			this->m_fProgress_Ratio = 1.f;
		//		}
		//	});
		break;
	case DTO::EUIDImageSubClassType::PLAYER_SKILL_END:
		break;
	case DTO::EUIDImageSubClassType::END:
	default:
		return E_FAIL;
	}
	return S_OK;
}

void CUISkill_BG::Initialize_Visible_Event()
{

}

_bool CUISkill_BG::Tick_Visible_Event(const _float fTimeDelta)
{
	return true;
}

void CUISkill_BG::Bind_Events()
{
	m_vecEventHandles.push_back(
		m_pUIManager->Get_UIEvents().Subscribe([this](const UIEVENT_DESC& Desc)
			{
				if (EUIEventID::MENU_CLOSE == Desc.eEventID)
				{
					this->Set_Visible();
				}
			})
	);
	m_vecEventHandles.push_back(
		m_pUIManager->Get_UIEvents().Subscribe([this](const UIEVENT_DESC& Desc)
			{
				if (EUIEventID::MENU_OPEN == Desc.eEventID)
				{
					this->Set_Invisible();
				}
			})
	);
}

CUISkill_BG* CUISkill_BG::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CUISkill_BG* pInstance = new CUISkill_BG(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CUISkill_BG::Create, Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUISkill_BG::Clone(void* pArg)
{
	CUISkill_BG* pInstance = new CUISkill_BG(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CUISkill_BG::Clone, Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUISkill_BG::Free()
{
	Super::Free();
}
