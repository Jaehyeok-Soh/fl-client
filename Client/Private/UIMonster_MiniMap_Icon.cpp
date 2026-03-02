#include "pch.h"
#include "UIMonster_MiniMap_Icon.h"
#include "Client_Defines.h"
#include "Client_EventDefine.h"
//=================
// Component
//=================
#include "MainPlayer.h"
#include "StatCom_Player.h"
#include "Texture.h"
#include "Shader.h"
#include "VIBuffer_Rect_Tex.h"
#include "GameInstance.h"

CUIMonster_MiniMap_Icon::CUIMonster_MiniMap_Icon(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	:CUIDynamic_Image(pDevice, pDeviceContext)
{
}

CUIMonster_MiniMap_Icon::CUIMonster_MiniMap_Icon(const CUIMonster_MiniMap_Icon& rhs)
	:CUIDynamic_Image(rhs)
{
}

HRESULT CUIMonster_MiniMap_Icon::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIMonster_MiniMap_Icon::Initialize(void* pArg)
{
	COMBO_IMAGE_DESC* pDesc = static_cast<COMBO_IMAGE_DESC*>(pArg);
	m_isInteract = true;
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;
	if (FAILED(Ready_Components(pDesc)))
		return E_FAIL;
	return S_OK;
}


HRESULT CUIMonster_MiniMap_Icon::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;

	if (FAILED(Attach_Personal_Info()))
		return E_FAIL;

	return S_OK;
}

void CUIMonster_MiniMap_Icon::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CUIMonster_MiniMap_Icon::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
}

void CUIMonster_MiniMap_Icon::Update_Late(const _float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);
	Tick_By_Type(fTimeDelta);
}

void CUIMonster_MiniMap_Icon::Ready_Before_Render(const _float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);
}

HRESULT CUIMonster_MiniMap_Icon::Render()
{
	if (!m_isVisible)
		return S_OK;
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;
	if (FAILED(Super::Render()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIMonster_MiniMap_Icon::Ready_Components(COMBO_IMAGE_DESC* pDesc)
{
	if (FAILED(Super::Ready_Components(pDesc)))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIMonster_MiniMap_Icon::Bind_ShaderResources()
{
	CShader* pShader = Get_Component<CShader>();
	if (FAILED(Get_Component<CTransform>()->Bind_ShaderResource(pShader)))
		return E_FAIL;

	if (FAILED(Super::Bind_ShaderResources()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIMonster_MiniMap_Icon::Attach_Personal_Info()
{
	CGameObject* pResult = m_pGameInstance->Get_GameObject_Front(ENUM_TO_UINT(ELevelType::STATIC), g_wszPlayerLayer);
	if (nullptr == pResult)
		return E_FAIL;

	m_pPlayerStatCom = static_cast<CStatCom_Player*>(pResult->Get_Component<CMyStat>());
	if (nullptr == m_pPlayerStatCom)
		return E_FAIL;

	return S_OK;
}

void CUIMonster_MiniMap_Icon::Tick_By_Type(const _float fTimeDelta)
{
}

void CUIMonster_MiniMap_Icon::OnUIEvent(ETriggerEventType eEvent, CGenericUI* pSender)
{
	if (!m_isActive)
		return;
}

void CUIMonster_MiniMap_Icon::Initialize_Visible_Event()
{
	m_isFin_Event = false;
	m_isActive = false;
}

_bool CUIMonster_MiniMap_Icon::Tick_Visible_Event(const _float fTimeDelta)
{
	return true;
}

void CUIMonster_MiniMap_Icon::Initialize_InVisible_Event()
{
}

_bool CUIMonster_MiniMap_Icon::Tick_InVisible_Event(const _float fTimeDelta)
{
	return true;
}

void CUIMonster_MiniMap_Icon::Convert_Count_To_Rank()
{
}

CUIMonster_MiniMap_Icon* CUIMonster_MiniMap_Icon::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CUIMonster_MiniMap_Icon* pInstance = new CUIMonster_MiniMap_Icon(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CUIMonster_MiniMap_Icon::Create, Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUIMonster_MiniMap_Icon::Clone(void* pArg)
{
	CUIMonster_MiniMap_Icon* pInstance = new CUIMonster_MiniMap_Icon(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CUIMonster_MiniMap_Icon::Clone, Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUIMonster_MiniMap_Icon::Free()
{
	Super::Free();
}
