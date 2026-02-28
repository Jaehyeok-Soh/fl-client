#include "Engine_pch.h"
#include "ContainerObject.h"
#include "PartObject.h"
#include "Collider.h"
#include "Model.h"
#include "GameInstance.h"

CContainerObject::CContainerObject(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: Super(pDevice, pDeviceContext)
{
}

CContainerObject::CContainerObject(const CContainerObject& rhs)
	: Super(rhs)
{
}

HRESULT CContainerObject::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CContainerObject::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CContainerObject::Awake(const _uint iCurretLevelIndex)
{
	if (FAILED(Super::Awake(iCurretLevelIndex)))
		return E_FAIL;

	for (CPartObject*& pPart : m_vecPartObjects)
	{
		if (pPart)
			pPart->Awake(iCurretLevelIndex);
	}

	return S_OK;
}

void CContainerObject::Update_Priority(_float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
	for (CPartObject* &pPart : m_vecPartObjects)
	{
		if (pPart)
			pPart->Update_Priority(fTimeDelta);
	}
}

void CContainerObject::Update(_float fTimeDelta)
{
	Super::Update(fTimeDelta);
	for (CPartObject*& pPart : m_vecPartObjects)
	{
		if (pPart)
			pPart->Update(fTimeDelta);
	}
}

void CContainerObject::Update_Late(_float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);
	for (CPartObject*& pPart : m_vecPartObjects)
	{
		if (pPart)
			pPart->Update_Late(fTimeDelta);
	}
}

void CContainerObject::Ready_Before_Render(_float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);
	for (CPartObject*& pPart : m_vecPartObjects)
	{
		if (pPart)
			pPart->Ready_Before_Render(fTimeDelta);
	}
}

void CContainerObject::Set_AttackCollider(_uint iPartIndex, _bool bActive, ATTACK_DESC* pDesc)
{
	if (iPartIndex >= m_vecPartObjects.size())
		return;

	m_vecPartObjects[iPartIndex]->Set_AttackCollider(bActive, pDesc);
}

HRESULT CContainerObject::Render()
{
	return S_OK;
}

void CContainerObject::OnCollision(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther)
{
}

void CContainerObject::OnCollision_Enter(_uint iMyColliderLayer, _uint iOtherColliderLayer, CGameObject* pOther, const COL_HIT_INFO& tHitInfo)
{
}

void CContainerObject::OnCollision_Exit(_uint iMyColliderLayer, _uint iOtherColliderLayer, CGameObject* pOther)
{
}

void CContainerObject::OnTrigger_Enter(_uint iMyColliderLayer, _uint iOtherColliderLayer, CGameObject* pOther)
{
}

void CContainerObject::OnTrigger_Exit(_uint iMyColliderLayer, _uint iOtherColliderLayer, CGameObject* pOther)
{
}

void CContainerObject::Set_Render(_bool bRender)
{
	for (auto& part : m_vecPartObjects)
	{
		if (part)
			part->Set_Render(bRender);
	}
}

void CContainerObject::Remove_Part(_uint iPartID)
{
	if (m_vecPartObjects[iPartID])
		Safe_Release(m_vecPartObjects[iPartID]);

	m_vecPartObjects[iPartID] = nullptr;
}

HRESULT CContainerObject::Add_Part(CPartObject* pPart, _uint iPartID)
{
	if (m_vecPartObjects[iPartID])
		return E_FAIL;

	pPart->Set_Parent(this);
	m_vecPartObjects[iPartID] = pPart;
	return S_OK;
}

HRESULT CContainerObject::Add_Part(_uint iPartID, _uint iPrototypeLevelIndex, const  wstring& wstrPrototypeTag, void* pArg)
{
	if (m_vecPartObjects[iPartID])
		return E_FAIL;

	if (CBase* pClone = m_pGameInstance->Clone_Prototype(EPrototypeType::GAMEOBJECT, iPrototypeLevelIndex, wstrPrototypeTag, pArg))
	{
		CPartObject* pPartObject = static_cast<CPartObject*>(pClone);
		pPartObject->Set_Parent(this);
		m_vecPartObjects[iPartID] = pPartObject;
		return S_OK;
	}

	return E_FAIL;
}

HRESULT CContainerObject::Change_Part(CPartObject* pPart, _uint iPartID)
{
	if (!pPart)
		return E_FAIL;

	if (m_vecPartObjects[iPartID])
		Safe_Release(m_vecPartObjects[iPartID]);

	m_vecPartObjects[iPartID] = pPart;
	return S_OK;
}

HRESULT CContainerObject::Change_Part(_uint iPartID, _uint iPrototypeLevelIndex, const  wstring& wstrPrototypeTag, void* pArg)
{
	if (m_vecPartObjects[iPartID])
		Safe_Release(m_vecPartObjects[iPartID]);

	if (CBase* pClone = m_pGameInstance->Clone_Prototype(EPrototypeType::GAMEOBJECT, iPrototypeLevelIndex, wstrPrototypeTag, pArg))
	{
		CPartObject* pPartObject = static_cast<CPartObject*>(pClone);
		m_vecPartObjects[iPartID] = pPartObject;
		return S_OK;
	}
	return E_FAIL;
}

void CContainerObject::Free()
{
	for (CPartObject*& pPart : m_vecPartObjects)
		Safe_Release(pPart);

	m_vecPartObjects.clear();
	Super::Free();
}
