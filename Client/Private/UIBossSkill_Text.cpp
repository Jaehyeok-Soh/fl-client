#include "pch.h"
#include "UIBossSkill_Text.h"
#include "Client_Defines.h"
#include "Client_EventDefine.h"

//=================
// Component
//=================
#include "Texture.h"
#include "Shader.h"
#include "VIBuffer_Rect_Tex.h"
#include "UI_Manager.h"
#include "GameInstance.h"

CUIBossSkill_Text::CUIBossSkill_Text(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	:CUIText(pDevice, pDeviceContext)
{
}

CUIBossSkill_Text::CUIBossSkill_Text(const CUIBossSkill_Text& rhs)
	:CUIText(rhs)
{
}

HRESULT CUIBossSkill_Text::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIBossSkill_Text::Initialize(void* pArg)
{
	BOSS_SKILL_TEXT_DESC* pDesc = static_cast<BOSS_SKILL_TEXT_DESC*>(pArg);
	m_iNumbering = pDesc->iNumbering;

	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;
	if (FAILED(Ready_Components(pDesc)))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIBossSkill_Text::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;

	if (FAILED(Attach_Personal_Info()))
		return E_FAIL;

	return S_OK;
}


void CUIBossSkill_Text::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CUIBossSkill_Text::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
	Tick_By_Type(fTimeDelta);
}

void CUIBossSkill_Text::Update_Late(const _float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);
}

void CUIBossSkill_Text::Ready_Before_Render(const _float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);
}

HRESULT CUIBossSkill_Text::Render()
{
	if (!m_isVisible)
		return S_OK;
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;
	if (FAILED(Super::Render()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIBossSkill_Text::Ready_Components(BOSS_SKILL_TEXT_DESC* pDesc)
{
	if (FAILED(Super::Ready_Components(pDesc)))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIBossSkill_Text::Bind_ShaderResources()
{
	CShader* pShader = Get_Component<CShader>();
	if (FAILED(Get_Component<CTransform>()->Bind_ShaderResource(pShader)))
		return E_FAIL;
	if (FAILED(Super::Bind_ShaderResources()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIBossSkill_Text::Attach_Personal_Info()
{
	return S_OK;
}

HRESULT CUIBossSkill_Text::Convert_Stat_To_Text()
{
	return S_OK;
}

void CUIBossSkill_Text::Bind_Events()
{
	Super::Bind_Events();


	m_vecEventHandles.push_back(
		m_pGameInstance->Subscribe<BOSS_SKILL_ON>(
			[this]()
			{
				this->Set_Visible();
			}));
	m_vecEventHandles.push_back(
		m_pGameInstance->Subscribe<BOSS_SKILL_OFF>(
			[this]()
			{
				this->Set_Invisible();
			}));
}

void CUIBossSkill_Text::Tick_By_Type(const _float fTimeDelta)
{
}

void CUIBossSkill_Text::Initialize_Visible_Event()
{
	Ready_Fade_Text(0.4f, 0.f, 1.f, m_fDelay);
}

void CUIBossSkill_Text::Initialize_InVisible_Event()
{
}

_bool CUIBossSkill_Text::Tick_Visible_Event(const _float fTimeDelta)
{
	_bool isFade = Tick_Fade_Text(fTimeDelta);

	if (isFade)
	{
		return true;
	}
	return false;
}

_bool CUIBossSkill_Text::Tick_InVisible_Event(const _float fTimeDelta)
{
	return true;
}

CUIBossSkill_Text* CUIBossSkill_Text::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CUIBossSkill_Text* pInstance = new CUIBossSkill_Text(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CUIBossSkill_Text::Create, Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUIBossSkill_Text::Clone(void* pArg)
{
	CUIBossSkill_Text* pInstance = new CUIBossSkill_Text(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CUIBossSkill_Text::Clone, Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUIBossSkill_Text::Free()
{
	Super::Free();
}
