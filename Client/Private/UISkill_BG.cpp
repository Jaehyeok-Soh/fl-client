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
		return S_OK;
	case DTO::EUIDImageSubClassType::PLAYER_Q:
		return S_OK;
	case DTO::EUIDImageSubClassType::PLAYER_Z:
		return S_OK;
	case DTO::EUIDImageSubClassType::PLAYER_GUN:
		return S_OK;
	case DTO::EUIDImageSubClassType::PLAYER_DODGE:
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
	_float fRatio	= {};
	_bool isE		= { false };

	switch (m_eDImageSubClass)
	{
	case DTO::EUIDImageSubClassType::PLAYER_SKILL_BEGIN:
		break;
	case DTO::EUIDImageSubClassType::PLAYER_E:
		isE		= true;
		// 지금 E스킬 쿨타임 Ratio를 받음 
		fRatio	= m_pPlayerStatCom->Get_Skill(CStatCom_Player::E).tCoolTimer.fCoolTimeRatio;
		break;
	case DTO::EUIDImageSubClassType::PLAYER_Q:
		fRatio	= m_pPlayerStatCom->Get_Skill(CStatCom_Player::Q).tCoolTimer.fCoolTimeRatio;
		break;
	case DTO::EUIDImageSubClassType::PLAYER_Z:
		break;
	case DTO::EUIDImageSubClassType::PLAYER_GUN:
		break;
	case DTO::EUIDImageSubClassType::PLAYER_DODGE:
		fRatio	= m_pPlayerStatCom->Get_Timer(CStatCom_Player::TIMER_TYPE::DASH).fCoolTimeRatio;
		break;
	case DTO::EUIDImageSubClassType::PLAYER_SKILL_END:
		break;
	case DTO::EUIDImageSubClassType::END:
	default:
		break;
	}

	// 만약 이전에 CoolTime Ratio 가 0이였는데 
	if (0.f == m_fPreCoolTimeRatio)
	{
		// 지금 CoolTime Ratio 가 0보다 크다? -> 스킬을 사용했다.
		if (fRatio > 0.f)
		{
			// 초기화 
			if (isE)
			{
				Ready_Fade(0.3f, 0.f, 1.f, m_fDelay);
				m_isUseE = true;
			}
			else
			{
				Ready_Fade(0.1f, 0.f, 1.f, m_fDelay);
				m_isUseSkill = true;
				m_isSkillFlash = false;
			}
		}
	}
	m_fPreCoolTimeRatio = fRatio;
	
	if (KEY_BUTTON_DOWN(DIK_E))
	{
		Ready_Fade(0.3f, 0.f, 1.f, m_fDelay);
		m_fProgress_Ratio = 1.f;
		m_isUseE = true;
	}
	if (KEY_BUTTON_DOWN(DIK_Q))
	{
		Ready_Fade(0.1f, 0.f, 1.f, m_fDelay);
		m_fProgress_Ratio = 1.f;
		m_isUseSkill = true;
	}
	if (KEY_BUTTON_DOWN(DIK_LSHIFT))
	{
		Ready_Fade(0.1f, 0.f, 1.f, m_fDelay);
		m_fProgress_Ratio = 1.f;
		m_isUseSkill = true;
	}
}

void CUISkill_BG::Tick_Use_Skill_Event(const _float fTimeDelta)
{
	if (m_isUseE)
	{
		_bool is = Tick_Fade(fTimeDelta);

		if (is)
		{
			if (m_isFinUseE)
			{
				m_isUseE = false;
				m_isFinUseE = false;
			}
			else
			{
				Ready_Fade(0.3f, 1.f, 0.f, m_fDelay);
				m_isFinUseE = true;
			}
		}
	}

	if (m_isUseSkill)
	{
		_bool is = { false };

		if (!m_isSkillFlash)
			is = Tick_Fade(fTimeDelta);
		else
		{
			//m_fProgress_Ratio = m_pPlayerStatCom->Get_Skill(CStatCom_Player::Q).tCoolTimer.fCoolTimeRatio;
			m_fProgress_Ratio -= fTimeDelta;
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
