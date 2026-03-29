#include "pch.h"
#include "GameObject.h"
#include "UIIcon_Component.h"
#include "Transform.h"
#include "Monster_Base.h"
#include "Monster_Body_Base.h"
#include "UIMinimap_Manager.h"
#include "GameInstance.h"

#define ZREF 10.f
#define MAX_SCALE_OFFSET 1.f
#define MIN_SCALE_OFFSET 0.3f


CUIIcon_Component::CUIIcon_Component()
	: m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}

CUIIcon_Component::CUIIcon_Component(const CUIIcon_Component& rhs)
	: m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CUIIcon_Component::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUIIcon_Component::Awake(_uint iLevelIndex)
{
	if (FAILED(Super::Awake(iLevelIndex)))
		return E_FAIL;



	return S_OK;
}

HRESULT CUIIcon_Component::Initialize(void* pArg)
{
	UI_ICON_COMP_DESC* pDesc = static_cast<UI_ICON_COMP_DESC*>(pArg);
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	m_pPlayer = m_pGameInstance->Get_GameObject_Front(ENUM_TO_UINT(ELevelType::STATIC), g_wszPlayerLayer);
	m_wstrTextureTag = pDesc->wstrIconTextureTag;

	if (nullptr == m_pPlayer)
		return E_FAIL;

	return S_OK;
}

void CUIIcon_Component::Update(const _float fTimeDelta)
{
	if (m_isRanged)
	{
		m_vScreenPos = CUIMinimap_Manager::GetInstance()->WorldPos_Convert_MinimapPosOffset(
			m_pOwner->Get_Component<CTransform>()->Get_Info(TRANSFORM_INFO_STATE::POS),
			m_pPlayer->Get_Component<CTransform>()->Get_Info(TRANSFORM_INFO_STATE::POS));
	}
}

CUIIcon_Component* CUIIcon_Component::Create()
{
	CUIIcon_Component* pInstance = new CUIIcon_Component();
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CUIIcon_Component::Create, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CUIIcon_Component::Clone(void* pArg)
{
	CUIIcon_Component* pInstance = new CUIIcon_Component();
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CUIIcon_Component::Clone, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUIIcon_Component::Free()
{
	Safe_Release(m_pGameInstance);
	Super::Free();
}
