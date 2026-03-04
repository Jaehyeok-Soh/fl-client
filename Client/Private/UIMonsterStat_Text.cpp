#include "pch.h"
#include "UIMonsterStat_Text.h"
#include "Client_Defines.h"
#include "Client_EventDefine.h"

//=================
// Component
//=================
#include "WorldUI_Component.h"
#include "MyStat.h"
#include "Texture.h"
#include "Shader.h"
#include "VIBuffer_Rect_Tex.h"
#include "GameInstance.h"

CUIMonsterStat_Text::CUIMonsterStat_Text(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	:CUIText(pDevice, pDeviceContext)
{
}

CUIMonsterStat_Text::CUIMonsterStat_Text(const CUIMonsterStat_Text& rhs)
	:CUIText(rhs)
{
}

HRESULT CUIMonsterStat_Text::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIMonsterStat_Text::Initialize(void* pArg)
{
	MONSTER_STAT_DESC* pDesc = static_cast<MONSTER_STAT_DESC*>(pArg);
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;
	if (FAILED(Ready_Components(pDesc)))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIMonsterStat_Text::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;

	if (FAILED(Attach_Personal_Info()))
		return E_FAIL;

	return S_OK;
}


void CUIMonsterStat_Text::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CUIMonsterStat_Text::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
}

void CUIMonsterStat_Text::Update_Late(const _float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);
}

void CUIMonsterStat_Text::Ready_Before_Render(const _float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);
	if (FAILED(Convert_Stat_To_Text()))
		return;
}

HRESULT CUIMonsterStat_Text::Render()
{
	if (!m_isVisible)
		return S_OK;
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;
	if (FAILED(Super::Render()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIMonsterStat_Text::Ready_Components(MONSTER_STAT_DESC* pDesc)
{
	if (FAILED(Super::Ready_Components(pDesc)))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIMonsterStat_Text::Bind_ShaderResources()
{
	CShader* pShader = Get_Component<CShader>();
	if (FAILED(Get_Component<CTransform>()->Bind_ShaderResource(pShader)))
		return E_FAIL;
	if (FAILED(Super::Bind_ShaderResources()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIMonsterStat_Text::Attach_Personal_Info()
{
	m_pGameInstance->Subscribe<MONSTER_DEAD_EVENT_START>(
		[this](CGameObject* pDead)
		{
			if (pDead == m_pTargetMoster)
				this->Set_Invisible();
		});

	return S_OK;
}

HRESULT CUIMonsterStat_Text::Convert_Stat_To_Text()
{
	switch (m_eTextSubClassType)
	{
	case DTO::EUITextSubClassType::MONSTER_STAT_TEXT_LV:
		m_wstrText = L"Lv 1"; // UIFIX //
		break;
	case DTO::EUITextSubClassType::MONSTER_STAT_TEXT_NICKNAME:
		m_wstrText = Engine_Utils::ToWString(m_pTargetStat->Get_Owner()->Get_Name());
		break;
	case DTO::EUITextSubClassType::END:
	default:
		return E_FAIL;
	}
	return S_OK;
}

void CUIMonsterStat_Text::OnUIEvent(ETriggerEventType eEvent, CGenericUI* pSender)
{
	if (!m_isActive)
		return;
}

void CUIMonsterStat_Text::Initialize_Visible_Event()
{
	m_isActive = false;
	m_isFin_Event = false;
}

void CUIMonsterStat_Text::Initialize_InVisible_Event()
{
	m_isActive = false;
	m_isFin_Event = false;
	Ready_Fade(1.f, 1.f, 0.f, 1.f);
}

_bool CUIMonsterStat_Text::Tick_Visible_Event(const _float fTimeDelta)
{
	m_isActive = true;
	m_isFin_Event = true;
	return true;
}

_bool CUIMonsterStat_Text::Tick_InVisible_Event(const _float fTimeDelta)
{
	if (Tick_Fade(fTimeDelta))
	{
		Request_SetDead();
		m_fAlpha_Ratio = 1.f;
		m_isFin_Event = true;
		m_isActive = true;
		return true;
	}
	return false;
}

HRESULT CUIMonsterStat_Text::Spawn_FromPool(void* pArg)
{
	UI_PREFAB_DATA* pDesc = static_cast<UI_PREFAB_DATA*>(pArg);

	auto* pComp = Get_Script_Component(L"WorldUIComponent");
	if (nullptr == pComp)
		return E_FAIL;

	m_pWorldUIComp = static_cast<CWorldUI_Component*>(pComp);
	if (nullptr == m_pWorldUIComp)
		return E_FAIL;

	if (FAILED(Super::Spawn_FromPool(pArg)))
		return E_FAIL;

	m_pWorldUIComp->Set_Target(pDesc->pTarget);
	m_pTargetMoster = pDesc->pTarget;
	m_pWorldUIComp->Set_TargetWorldOffset(pDesc->NamePlateData.vOffset);

	m_pTargetStat = pDesc->pTarget->Get_Component<CMyStat>();
	if (nullptr == m_pTargetStat)
		return E_FAIL;
	
	return S_OK;
}

HRESULT CUIMonsterStat_Text::Despawn_FromPool()
{
	if (FAILED(Super::Despawn_FromPool()))
		return E_FAIL;

	return S_OK;
}

CUIMonsterStat_Text* CUIMonsterStat_Text::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CUIMonsterStat_Text* pInstance = new CUIMonsterStat_Text(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CUIMonsterStat_Text::Create, Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUIMonsterStat_Text::Clone(void* pArg)
{
	CUIMonsterStat_Text* pInstance = new CUIMonsterStat_Text(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CUIMonsterStat_Text::Clone, Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUIMonsterStat_Text::Free()
{
	Super::Free();
}
