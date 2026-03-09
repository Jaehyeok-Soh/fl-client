#include "pch.h"
#include "UIPlayerAmmo_Progress.h"
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
#include "GameInstance.h"
#include <UI_Manager.h>

CUIPlayerAmmo_Progress::CUIPlayerAmmo_Progress(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	:CUIProgress_Bar(pDevice, pDeviceContext)
{
}

CUIPlayerAmmo_Progress::CUIPlayerAmmo_Progress(const CUIPlayerAmmo_Progress& rhs)
	:CUIProgress_Bar(rhs)
{
}

HRESULT CUIPlayerAmmo_Progress::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIPlayerAmmo_Progress::Initialize(void* pArg)
{
	PLAYER_AMMO_PROGRESS_DESC* pDesc = static_cast<PLAYER_AMMO_PROGRESS_DESC*>(pArg);
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;
	if (FAILED(Ready_Components(pDesc)))
		return E_FAIL;

	return S_OK;
}


HRESULT CUIPlayerAmmo_Progress::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;

	if (FAILED(Attach_Personal_Info()))
		return E_FAIL;

	return S_OK;
}

void CUIPlayerAmmo_Progress::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CUIPlayerAmmo_Progress::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);

	// CurRatio °»½Å
	Convert_Stat_To_Ratio();
}

void CUIPlayerAmmo_Progress::Update_Late(const _float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);
}

void CUIPlayerAmmo_Progress::Ready_Before_Render(const _float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);
}

HRESULT CUIPlayerAmmo_Progress::Render()
{
	if (!m_isVisible)
		return S_OK;
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;
	if (FAILED(Super::Render()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIPlayerAmmo_Progress::Ready_Components(PLAYER_AMMO_PROGRESS_DESC* pDesc)
{
	if (FAILED(Super::Ready_Components(pDesc)))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIPlayerAmmo_Progress::Bind_ShaderResources()
{
	Super::Bind_ShaderResources();
	CShader* pShader = Get_Component<CShader>();
	if (FAILED(Get_Component<CTransform>()->Bind_ShaderResource(pShader)))
		return E_FAIL;
	if (FAILED(Super::Bind_ShaderResources()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIPlayerAmmo_Progress::Attach_Personal_Info()
{
	CGameObject* pResult = m_pGameInstance->Get_GameObject_Front(ENUM_TO_UINT(ELevelType::STATIC), g_wszPlayerLayer);
	if (nullptr == pResult)
		return E_FAIL;

	m_pPlayerStatCom = static_cast<CStatCom_Player*>(pResult->Get_Component<CMyStat>());
	if (nullptr == m_pPlayerStatCom)
		return E_FAIL;

	auto* pPlayer = static_cast<CMainPlayer*>(pResult);
	if (nullptr == pPlayer)
		return E_FAIL;

	m_pGunParts = pPlayer->Get_Part<CGun>(ENUM_TO_UINT(CPlayer::Part::GUN));
	if (nullptr == m_pGunParts)
		return E_FAIL;

	return S_OK;
}

void CUIPlayerAmmo_Progress::Bind_Events()
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

	m_pGameInstance->Subscribe<CINEMATIC_START>(
		[this]() 
		{ 
			this->Set_Invisible();
		});
}

void CUIPlayerAmmo_Progress::Initialize_Visible_Event()
{
	m_isActive = false;
	m_isFin_Event = false;
	m_fTimeAcc = 0.f;
	m_fAlpha_Ratio = 0.f;
}

void CUIPlayerAmmo_Progress::Initialize_InVisible_Event()
{
	m_isFin_Event = false;
	m_fTimeAcc = 0.f;
}

_bool CUIPlayerAmmo_Progress::Tick_Visible_Event(const _float fTimeDelta)
{
	m_fAlpha_Ratio += fTimeDelta * 2.f;
	if (m_fAlpha_Ratio >= 1.f)
	{
		m_fAlpha_Ratio = 1.f;
		m_isFin_Event = true;
		m_isActive = true;
		return true;
	}
	return false;
}

_bool CUIPlayerAmmo_Progress::Tick_InVisible_Event(const _float fTimeDelta)
{
	m_isFin_Event = true;
	return true;
}


HRESULT CUIPlayerAmmo_Progress::Convert_Stat_To_Ratio()
{
	m_fProgress_Ratio = m_pGunParts->Get_CurButtlet().x / m_pGunParts->Get_CurButtlet().y;
	return S_OK;
}

CUIPlayerAmmo_Progress* CUIPlayerAmmo_Progress::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CUIPlayerAmmo_Progress* pInstance = new CUIPlayerAmmo_Progress(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CUIPlayerAmmo_Progress::Create, Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUIPlayerAmmo_Progress::Clone(void* pArg)
{
	CUIPlayerAmmo_Progress* pInstance = new CUIPlayerAmmo_Progress(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CUIPlayerAmmo_Progress::Clone, Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUIPlayerAmmo_Progress::Free()
{
	Super::Free();
}
