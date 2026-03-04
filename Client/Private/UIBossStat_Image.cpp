#include "pch.h"
#include "UIBossStat_Image.h"
#include "Client_Defines.h"
#include "Client_EventDefine.h"
//=================
// Component
//=================
#include "WorldUI_Component.h"
#include "Texture.h"
#include "Shader.h"
#include "VIBuffer_Rect_Tex.h"
#include "GameInstance.h"

CUIBossStat_Image::CUIBossStat_Image(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	:CUIDynamic_Image(pDevice, pDeviceContext)
{
}

CUIBossStat_Image::CUIBossStat_Image(const CUIBossStat_Image& rhs)
	:CUIDynamic_Image(rhs)
{
}

HRESULT CUIBossStat_Image::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIBossStat_Image::Initialize(void* pArg)
{
	BOSS_STAT_IMAGE_DESC* pDesc = static_cast<BOSS_STAT_IMAGE_DESC*>(pArg);
	m_isInteract = true;
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;
	if (FAILED(Ready_Components(pDesc)))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIBossStat_Image::Attach_Personal_Info()
{
	return S_OK;
}

HRESULT CUIBossStat_Image::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;

	if (FAILED(Attach_Personal_Info()))
		return E_FAIL;
	if (m_isSpawned)
	{
		Set_Visible();
		m_isSpawned = false;
	}
	return S_OK;
}

void CUIBossStat_Image::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CUIBossStat_Image::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
}

void CUIBossStat_Image::Update_Late(const _float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);

	m_fTimeAcc += fTimeDelta;
	if (m_fTimeAcc > 3.f)
	{
		if (m_isBossEventTrigger)
			return;

		m_pGameInstance->Broadcast<BOSS_STAGING_EVENT_END>();
		m_isBossEventTrigger = true;
	}
}

void CUIBossStat_Image::Ready_Before_Render(const _float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);
}

HRESULT CUIBossStat_Image::Render()
{
	if (!m_isVisible)
		return S_OK;
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;
	if (FAILED(Super::Render()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIBossStat_Image::Ready_Components(BOSS_STAT_IMAGE_DESC* pDesc)
{
	if (FAILED(Super::Ready_Components(pDesc)))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIBossStat_Image::Bind_ShaderResources()
{
	CShader* pShader = Get_Component<CShader>();
	if (FAILED(Get_Component<CTransform>()->Bind_ShaderResource(pShader)))
		return E_FAIL;
	if (FAILED(Super::Bind_ShaderResources()))
		return E_FAIL;
	return S_OK;
}

void CUIBossStat_Image::OnUIEvent(ETriggerEventType eEvent, CGenericUI* pSender)
{
	if (!m_isActive)
		return;
}

void CUIBossStat_Image::Initialize_Visible_Event()
{
	m_isFin_Event	= false;
	m_isActive		= false;
}

_bool CUIBossStat_Image::Tick_Visible_Event(const _float fTimeDelta)
{
	return true;
}

HRESULT CUIBossStat_Image::Spawn_FromPool(void* pArg)
{
	if (FAILED(Super::Spawn_FromPool(pArg)))
		return E_FAIL;

	UI_PREFAB_DATA* pDesc = static_cast<UI_PREFAB_DATA*>(pArg);
	/* º¸½º ½ºÅÈ ÄÄÆ÷³ÍÆ® ºÎÂø */
	m_isBossEventTrigger = false;
	m_fTimeAcc = 0.f;
	m_isSpawned = true;
	m_pGameInstance->Broadcast<BOSS_STAGING_EVENT_START>();
	return S_OK;
}

HRESULT CUIBossStat_Image::Despawn_FromPool()
{
	if (FAILED(Super::Despawn_FromPool()))
		return E_FAIL;

	m_isVisible			= false;
	m_isPreVisible		= false;
	m_isVisibleTrigger	= false;
	return S_OK;
}

CUIBossStat_Image* CUIBossStat_Image::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CUIBossStat_Image* pInstance = new CUIBossStat_Image(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CUIBossStat_Image::Create, Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUIBossStat_Image::Clone(void* pArg)
{
	CUIBossStat_Image* pInstance = new CUIBossStat_Image(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CUIBossStat_Image::Clone, Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUIBossStat_Image::Free()
{
	Super::Free();
}
