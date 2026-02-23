#include "pch.h"
#include "UIPrefab_MonsterNameplate.h"
#include "Client_Defines.h"

//=================
// Component
//=================
#include "Texture.h"
#include "Shader.h"
#include "VIBuffer_Rect_Tex.h"
#include "MyStat.h"
#include "GameInstance.h"

CUIPrefab_MonsterNameplate::CUIPrefab_MonsterNameplate(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	:CGenericUI(pDevice, pDeviceContext)
{
}

CUIPrefab_MonsterNameplate::CUIPrefab_MonsterNameplate(const CUIPrefab_MonsterNameplate& rhs)
	:CGenericUI(rhs)
{
}

HRESULT CUIPrefab_MonsterNameplate::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIPrefab_MonsterNameplate::Initialize(void* pArg)
{
	PREFAB_MONSTER_NAMEPLATE_DESC* pDesc = static_cast<PREFAB_MONSTER_NAMEPLATE_DESC*>(pArg);
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;
	if (FAILED(Ready_Components(pDesc)))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIPrefab_MonsterNameplate::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;

	return S_OK;
}

void CUIPrefab_MonsterNameplate::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CUIPrefab_MonsterNameplate::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
}

void CUIPrefab_MonsterNameplate::Update_Late(const _float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);
}

void CUIPrefab_MonsterNameplate::Ready_Before_Render(const _float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);
}

HRESULT CUIPrefab_MonsterNameplate::Render()
{
	if (!m_isVisible)
		return S_OK;

	if (FAILED(Super::Render()))
		return E_FAIL;

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	Get_Component<CShader>()->Apply();
	Get_Component<CVIBuffer>()->Bind_Resource();
	Get_Component<CVIBuffer>()->Render();
	return S_OK;
}

void CUIPrefab_MonsterNameplate::OnUIEvent(ETriggerEventType eEvent, CGenericUI* pSender)
{
}

void CUIPrefab_MonsterNameplate::Initialize_Visible_Event()
{
	m_isFin_Event = false;
}

void CUIPrefab_MonsterNameplate::Initialize_InVisible_Event()
{
	m_isFin_Event = false;
}

_bool CUIPrefab_MonsterNameplate::Tick_Visible_Event(const _float fTimeDelta)
{
	return true;
}

_bool CUIPrefab_MonsterNameplate::Tick_InVisible_Event(const _float fTimeDelta)
{
	m_isFin_Event = true;
	return true;
}

HRESULT CUIPrefab_MonsterNameplate::Ready_Components(PREFAB_MONSTER_NAMEPLATE_DESC* pDesc)
{
	return S_OK;
}

HRESULT CUIPrefab_MonsterNameplate::Bind_ShaderResources()
{
	CShader* pShader = Get_Component<CShader>();
	if (FAILED(Get_Component<CTransform>()->Bind_ShaderResource(pShader)))
		return E_FAIL;

	Super::Bind_ShaderResources();
	return S_OK;
}

CUIPrefab_MonsterNameplate* CUIPrefab_MonsterNameplate::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CUIPrefab_MonsterNameplate* pInstance = new CUIPrefab_MonsterNameplate(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CUIPrefab_MonsterNameplate::Create, Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUIPrefab_MonsterNameplate::Clone(void* pArg)
{
	CUIPrefab_MonsterNameplate* pInstance = new CUIPrefab_MonsterNameplate(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CUIPrefab_MonsterNameplate::Clone, Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUIPrefab_MonsterNameplate::Free()
{
	Super::Free();
}
