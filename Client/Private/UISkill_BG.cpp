#include "pch.h"
#include "UISkill_BG.h"
#include "Client_Defines.h"

//=================
// Component
//=================
#include "StatCom_Player.h"
#include "Texture.h"
#include "Shader.h"
#include "VIBuffer_Rect_Tex.h"
#include "MyStat.h"
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
		return S_OK;
	case DTO::EUIDImageSubClassType::PLAYER_E:
		m_fMaxCoolTime = m_pPlayerStatCom->Get_Skill(CStatCom_Player::E).tCoolTimer.fMaxTime;
		return S_OK;
	case DTO::EUIDImageSubClassType::PLAYER_Q:
		m_fMaxCoolTime = m_pPlayerStatCom->Get_Skill(CStatCom_Player::Q).tCoolTimer.fMaxTime;
		return S_OK;
	case DTO::EUIDImageSubClassType::PLAYER_Z:
		return S_OK;
	case DTO::EUIDImageSubClassType::PLAYER_GUN:
		return S_OK;
	case DTO::EUIDImageSubClassType::PLAYER_DODGE:
		m_fMaxCoolTime = m_pPlayerStatCom->Get_Timer(CStatCom_Player::TIMER_TYPE::DASH).fMaxTime;
		return S_OK;
	case DTO::EUIDImageSubClassType::PLAYER_SKILL_END:
	case DTO::EUIDImageSubClassType::END:
	default:
		return E_FAIL;
	}

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
	if (m_strName == "Player_QSkill_fx")
		int a = 0;
	Super::Update_Priority(fTimeDelta);
	Trigger_User_Use_Skill();
}

void CUISkill_BG::Update(const _float fTimeDelta)
{
	if (m_strName == "Player_QSkill_fx")
		int a = 0;
	Super::Update(fTimeDelta);
	Tick_Use_Skill_Event(fTimeDelta);
}

void CUISkill_BG::Update_Late(const _float fTimeDelta)
{
	if (m_strName == "Player_QSkill_fx")
		int a = 0;
	Super::Update_Late(fTimeDelta);
}

void CUISkill_BG::Ready_Before_Render(const _float fTimeDelta)
{
	if (m_strName == "Player_QSkill_fx")
		int a = 0;
	Super::Ready_Before_Render(fTimeDelta);
}

HRESULT CUISkill_BG::Render()
{
	if (m_strName == "Player_QSkill_fx")
		int a = 0;

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
	_bool isE = { false };
	_bool is = { false };

	switch (m_eDImageSubClass)
	{
	case DTO::EUIDImageSubClassType::PLAYER_SKILL_BEGIN:
		break;
	case DTO::EUIDImageSubClassType::PLAYER_E:
		isE	= true;
		if (m_isUsingE)
			break;
		is = m_pPlayerStatCom->Get_Skill_Ptr(CStatCom_Player::E)->Is_OnSkill();
		break;
	case DTO::EUIDImageSubClassType::PLAYER_Q:
		m_fCurCoolTime = m_pPlayerStatCom->Get_Skill(CStatCom_Player::Q).tCoolTimer.fTimeAcc;
		if (m_isUsingSkill)
			break;
		is = m_pPlayerStatCom->Get_Skill_Ptr(CStatCom_Player::Q)->Is_OnSkill();
		break;
	case DTO::EUIDImageSubClassType::PLAYER_Z:
		break;
	case DTO::EUIDImageSubClassType::PLAYER_GUN:
		break;
	case DTO::EUIDImageSubClassType::PLAYER_DODGE:
	{
		_float f = m_pPlayerStatCom->Get_Timer(CStatCom_Player::TIMER_TYPE::DASH).fTimeAcc;

		if (m_fCurCoolTime == 0 && m_fCurCoolTime < f)
			if(!m_isUsingSkill)
				is = true;

		m_fCurCoolTime = f;
	}
	break;
	case DTO::EUIDImageSubClassType::PLAYER_SKILL_END:
		break;
	case DTO::EUIDImageSubClassType::END:
	default:
		break;
	}

	if (is)
	{
		if (isE)
		{
			Ready_Fade(0.3f, 0.f, 0.7f, m_fDelay);
			m_isUsingE = true;
			m_isFinUseE = false;
		}
		else
		{
			Ready_Fade(0.2f, 0.f, 0.7f, m_fDelay);
			m_isUsingSkill = true;
			m_isSkillFlash = false;
		}
	}
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
			m_fProgress_Ratio = 1.f - (m_fCurCoolTime / m_fMaxCoolTime);

			if (m_fProgress_Ratio <= 0.1f)
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
}

void CUISkill_BG::Initialize_Visible_Event()
{

}

_bool CUISkill_BG::Tick_Visible_Event(const _float fTimeDelta)
{
	return _bool();
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
