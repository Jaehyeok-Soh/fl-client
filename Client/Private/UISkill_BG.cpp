#include "pch.h"
#include "UISkill_BG.h"
#include "Client_Defines.h"

//=================
// Component
//=================
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

	return S_OK;
}

void CUISkill_BG::Update_Priority(const _float fTimeDelta)
{
	Trigger_User_Use_Skill();
	Super::Update_Priority(fTimeDelta);
}

void CUISkill_BG::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
}

void CUISkill_BG::Update_Late(const _float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);

	if (m_isUseESkillEventStart)
	{
		m_fAlpha_Ratio += fTimeDelta * 5.f;
		if (m_fAlpha_Ratio >= 1.f)
		{
			m_isUseESkillEventStart = FALSE;
			m_isUseESkillEvnetEnd = TRUE;
			m_fAlpha_Ratio = 1.f;
		}
	}
	else if(m_isUseESkillEvnetEnd)
	{
		m_fAlpha_Ratio -= fTimeDelta * 5.f;
		if (m_fAlpha_Ratio <= 0.f)
		{
			m_isUseESkillEvnetEnd = FALSE;
			m_fAlpha_Ratio = 0.f;
		}
	}

	if (m_isUseSkillEventStart)
	{
		m_fAlpha_Ratio += fTimeDelta * 10.f;
		if (m_fAlpha_Ratio >= 1.f)
		{
			m_isUseSkillEventStart = FALSE;
			m_isUseSkillEventEnd = TRUE;
			m_fAlpha_Ratio = 0.8f;
		}
	}
	else if (m_isUseSkillEventEnd)
	{
		m_fProgress_Ratio -= fTimeDelta ;
	}

}

void CUISkill_BG::Ready_Before_Render(const _float fTimeDelta)
{
	Acting_By_InteractState();
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
	if (m_isUseESkillEventStart)
		return;

	if (KEY_BUTTON_DOWN(DIK_E))
	{
		if (m_eDImageSubClass == DTO::EUIDImageSubClassType::PLAYER_E)
			m_isUseESkillEventStart = TRUE;
	}
	else if (KEY_BUTTON_DOWN(DIK_Q))
	{
		if (m_eDImageSubClass == DTO::EUIDImageSubClassType::PLAYER_Q)
		{
			m_isUseSkillEventStart = TRUE;
			m_fAlpha_Ratio = 0.f;
			m_fProgress_Ratio = 1.f;
		}
	}
	else if (KEY_BUTTON_DOWN(DIK_Z))
	{
		if (m_eDImageSubClass == DTO::EUIDImageSubClassType::PLAYER_Z)
		{
			m_isUseSkillEventStart = TRUE;
			m_fAlpha_Ratio = 0.f;
			m_fProgress_Ratio = 1.f;
		}
	}
	else if (KEY_BUTTON_DOWN(DIK_LSHIFT))
	{
		if (m_eDImageSubClass == DTO::EUIDImageSubClassType::PLAYER_DODGE)
		{
			m_isUseSkillEventStart = TRUE;
			m_fAlpha_Ratio = 0.f;
			m_fProgress_Ratio = 1.f;
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
