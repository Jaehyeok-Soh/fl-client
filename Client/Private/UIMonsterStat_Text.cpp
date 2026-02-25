#include "pch.h"
#include "UIMonsterStat_Text.h"
#include "Client_Defines.h"

//=================
// Component
//=================
#include "StatCom_Player.h"
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

	if (FAILED(Attach_Personal_Info(iCurrentLevelID)))
		return E_FAIL;

	return S_OK;
}

HRESULT CUIMonsterStat_Text::Attach_Personal_Info(const _uint iCurrentLevelID)
{
	CGameObject* pResult = m_pGameInstance->Get_GameObject_Front(iCurrentLevelID, g_wszPlayerLayer);
	if (nullptr == pResult)
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
	if (FAILED(Convert_Stat_To_Text()))
		return;

	Super::Ready_Before_Render(fTimeDelta);
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

HRESULT CUIMonsterStat_Text::Convert_Stat_To_Text()
{
	switch (m_eTextSubClassType)
	{
	case DTO::EUITextSubClassType::MONSTER_STAT_TEXT_LV:break;
		m_wstrText = L"Lv.10";
	case DTO::EUITextSubClassType::MONSTER_STAT_TEXT_NICKNAME:break;
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
}

_bool CUIMonsterStat_Text::Tick_Visible_Event(const _float fTimeDelta)
{
	m_isActive = true;
	m_isFin_Event = true;
	return true;
}

_bool CUIMonsterStat_Text::Tick_InVisible_Event(const _float fTimeDelta)
{
	return true;
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
