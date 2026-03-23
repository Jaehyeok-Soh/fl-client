#include "pch.h"
#include "ChangeLevelObject.h"

#include "Level_Loading.h"

#include "GameInstance.h"

CChangeLevelObject::CChangeLevelObject(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CInteractiveObject(pDevice, pDeviceContext),
	m_eChangeLevelType{ ELevelType::END }
{
}

CChangeLevelObject::CChangeLevelObject(const CChangeLevelObject& rhs)
	: CInteractiveObject(rhs),
	m_eChangeLevelType{ rhs.m_eChangeLevelType }
{
}

HRESULT CChangeLevelObject::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;

	Set_Object_Enum_Tag(OBJECT_ENUM_TAG::OBJECT_INTERACT_CHANGELEVEL);

	return S_OK;
}

HRESULT CChangeLevelObject::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	INTERACT_LEVELCHANGE_DESC* pDesc = static_cast<INTERACT_LEVELCHANGE_DESC*>(pArg);

	m_eChangeLevelType = pDesc->eChangeLevelType;

	Set_Name("장소 이동");

	// 상호작용
	{
		Set_Interact_Enable();
		Set_Interact_DefaultEnable();
	}

	return S_OK;
}

HRESULT CChangeLevelObject::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;

	return S_OK;
}

void CChangeLevelObject::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CChangeLevelObject::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
}

void CChangeLevelObject::Update_Late(const _float fTimeelta)
{
	Super::Update_Late(fTimeelta);
}

void CChangeLevelObject::Ready_Before_Render(const _float fTimeDelta)
{
}

HRESULT CChangeLevelObject::Render()
{
	return S_OK;
}

CChangeLevelObject* CChangeLevelObject::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CChangeLevelObject* pInstance = new CChangeLevelObject(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		Safe_Release(pInstance);
		MSG_BOX("CChangeLevelObject is failed to Create");
		return nullptr;
	}
	return pInstance;
}

CGameObject* CChangeLevelObject::Clone(void* pArg)
{
	CChangeLevelObject* pInstance = new CChangeLevelObject(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		Safe_Release(pInstance);
		MSG_BOX(" CChangeLevelObject is failed to Clone");
		return nullptr;
	}

	return pInstance;
}

void CChangeLevelObject::Free()
{
	Super::Free();
}

void CChangeLevelObject::Interact()
{
	Super::Interact();

	m_pGameInstance->Request_ChangeLevel(ENUM_TO_UINT(ELevelType::LOADING), CLevel_Loading::Create(m_pDevice, m_pDeviceContext, m_eChangeLevelType));
}

void CChangeLevelObject::QuestEnter()
{
	Super::QuestEnter();
}

void CChangeLevelObject::QuestExit()
{
	Super::QuestExit();
}
