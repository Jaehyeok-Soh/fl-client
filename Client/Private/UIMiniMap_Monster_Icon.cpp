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
#include "UIIcon_Component.h"
#include "UIMinimap_Manager.h"
#include "UI_Manager.h"
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
	m_fRadian = CUIMinimap_Manager::GetInstance()->WorldLook_Convert_To_Radian(
		m_pTarget->Get_Component<CTransform>()->Get_Info(TRANSFORM_INFO_STATE::LOOK));

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
	if (!m_pTargetIconComp->Get_isRanged())
	{
		Set_Invisible();
		Set_Dead();
	}
	m_vMoveOffset = m_pTargetIconComp->Get_ScreenPos();
}


void CUIMiniMap_Monster_Icon::Bind_Events()
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
}

void CUIMiniMap_Monster_Icon::Initialize_Visible_Event()
{
	m_isFin_Event = false;
	m_isActive = false;
}

_bool CUIMiniMap_Monster_Icon::Tick_Visible_Event(const _float fTimeDelta)
{
	m_isFin_Event = true;
	m_isActive = true;
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
	if (FAILED(Super::Spawn_FromPool(pArg)))
		return E_FAIL; 

	UI_PREFAB_DATA* pDesc = static_cast<UI_PREFAB_DATA*>(pArg);
	if (auto* pMinimapMonster = std::get_if<UI_MINIMAP_MONSTER_ICON_PREFAB_DATA>(&pDesc->Data))
	{
		m_pTarget = pMinimapMonster->pTarget;
		if (nullptr == m_pTarget)
			return E_FAIL;

		m_pTargetIconComp = dynamic_cast<CUIIcon_Component*>(m_pTarget->Get_Script_Component(L"UIIconComp"));
		if (nullptr == m_pTargetIconComp)
			return E_FAIL;

	}

	//CGameObject* pResult = m_pGameInstance->Get_GameObject_Front(ENUM_TO_UINT(ELevelType::STATIC), g_wszPlayerLayer);
	//if (nullptr == pResult)
	//	return E_FAIL;

	//m_pPlayer = static_cast<CMainPlayer*>(pResult);
	//if (nullptr == m_pPlayer)
	//	return E_FAIL;

	Set_Visible();
	Set_Active(true);

	return S_OK;
}

HRESULT CUIMiniMap_Monster_Icon::Despawn_FromPool()
{
	if (FAILED(Super::Despawn_FromPool()))
		return E_FAIL;

	Set_Active(false);
	return S_OK;
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
