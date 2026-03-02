#include "pch.h"
#include "UIMiniMap_Monster_Icon.h"
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

CUIMiniMap_Monster_Icon::CUIMiniMap_Monster_Icon(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	:CUIDynamic_Image(pDevice, pDeviceContext)
{
}

CUIMiniMap_Monster_Icon::CUIMiniMap_Monster_Icon(const CUIMiniMap_Monster_Icon& rhs)
	:CUIDynamic_Image(rhs)
{
}

HRESULT CUIMiniMap_Monster_Icon::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIMiniMap_Monster_Icon::Initialize(void* pArg)
{
	MINIMAP_MONSTER_ICON_DESC* pDesc = static_cast<MINIMAP_MONSTER_ICON_DESC*>(pArg);
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;
	if (FAILED(Ready_Components(pDesc)))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIMiniMap_Monster_Icon::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;

	if (FAILED(Attach_Personal_Info()))
		return E_FAIL;

	return S_OK;
}

void CUIMiniMap_Monster_Icon::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CUIMiniMap_Monster_Icon::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
	Proj_World_To_Screen(m_pTarget->Get_Component<CTransform>()->Get_Info(TRANSFORM_INFO_STATE::LOOK));
	Rotate_MonsterIcon();
}


void CUIMiniMap_Monster_Icon::Update_Late(const _float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);
	Tick_By_Type(fTimeDelta);
}

void CUIMiniMap_Monster_Icon::Ready_Before_Render(const _float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);
}

HRESULT CUIMiniMap_Monster_Icon::Render()
{
	if (!m_isVisible)
		return S_OK;
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;
	if (FAILED(Super::Render()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIMiniMap_Monster_Icon::Ready_Components(MINIMAP_MONSTER_ICON_DESC* pDesc)
{
	if (FAILED(Super::Ready_Components(pDesc)))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIMiniMap_Monster_Icon::Bind_ShaderResources()
{
	CShader* pShader = Get_Component<CShader>();
	if (FAILED(Get_Component<CTransform>()->Bind_ShaderResource(pShader)))
		return E_FAIL;

	if (FAILED(Super::Bind_ShaderResources()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIMiniMap_Monster_Icon::Attach_Personal_Info()
{


	return S_OK;
}

void CUIMiniMap_Monster_Icon::Tick_By_Type(const _float fTimeDelta)
{
}

void CUIMiniMap_Monster_Icon::OnUIEvent(ETriggerEventType eEvent, CGenericUI* pSender)
{
	if (!m_isActive)
		return;
}

void CUIMiniMap_Monster_Icon::Initialize_Visible_Event()
{
	m_isFin_Event = false;
	m_isActive = false;
}

_bool CUIMiniMap_Monster_Icon::Tick_Visible_Event(const _float fTimeDelta)
{
	return true;
}

void CUIMiniMap_Monster_Icon::Initialize_InVisible_Event()
{
}

_bool CUIMiniMap_Monster_Icon::Tick_InVisible_Event(const _float fTimeDelta)
{
	return true;
}

void CUIMiniMap_Monster_Icon::Convert_Count_To_Rank()
{
}

HRESULT CUIMiniMap_Monster_Icon::Spawn_FromPool(void* pArg)
{
	UI_PREFAB_DATA* pDesc = static_cast<UI_PREFAB_DATA*>(pArg);
	m_pTarget = pDesc->pTarget;
	if (nullptr == m_pTarget)
		return E_FAIL;

	CGameObject* pResult = m_pGameInstance->Get_GameObject_Front(ENUM_TO_UINT(ELevelType::STATIC), g_wszPlayerLayer);
	if (nullptr == pResult)
		return E_FAIL;

	m_pPlayer = static_cast<CMainPlayer*>(pResult);
	if (nullptr == m_pPlayer)
		return E_FAIL;
	
	return S_OK;
}

HRESULT CUIMiniMap_Monster_Icon::Despawn_FromPool()
{
	return S_OK;
}

void CUIMiniMap_Monster_Icon::Proj_World_To_Screen(const Vec3& vLook)
{
	Vec2 vDir2D = Vec2{ vLook.x, vLook.z };
	if (vDir2D.Length() > 1e-6f)
		vDir2D.Normalize();
	m_fRadian = atan2f(vDir2D.x, vDir2D.y);
}

void CUIMiniMap_Monster_Icon::Rotate_MonsterIcon()
{
	Get_Component<CTransform>()->Rotation(Vec3{ 0.f, 0.f, -1.f }, m_fRadian);
}

CUIMiniMap_Monster_Icon* CUIMiniMap_Monster_Icon::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CUIMiniMap_Monster_Icon* pInstance = new CUIMiniMap_Monster_Icon(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CUIMiniMap_Monster_Icon::Create, Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUIMiniMap_Monster_Icon::Clone(void* pArg)
{
	CUIMiniMap_Monster_Icon* pInstance = new CUIMiniMap_Monster_Icon(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CUIMiniMap_Monster_Icon::Clone, Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUIMiniMap_Monster_Icon::Free()
{
	Super::Free();
}
