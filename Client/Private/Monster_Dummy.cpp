#include "pch.h"
#include "Monster_Dummy.h"

#include "GameInstance.h"

#include "StatComponent.h"
#include "Model.h"
#include "PhysicsCCT.h"

CMonster_Dummy::CMonster_Dummy(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: Super(pDevice, pDeviceContext)
{
}

CMonster_Dummy::CMonster_Dummy(const CMonster_Dummy& rhs)
	: Super(rhs)
{
}

HRESULT CMonster_Dummy::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMonster_Dummy::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	Set_Name("Monster_Dummy");

	if (FAILED(Ready_PartObjects()))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_BaseStates()))
		return E_FAIL;

	//if (FAILED(Ready_Ability()))
	//	return E_FAIL;

	return S_OK;
}

HRESULT CMonster_Dummy::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;
	
	return S_OK;
}

void CMonster_Dummy::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CMonster_Dummy::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
}

void CMonster_Dummy::Update_Late(const _float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);
}

void CMonster_Dummy::Ready_Before_Render(const _float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);
}

HRESULT CMonster_Dummy::Render()
{
	if (FAILED(Super::Render()))
		return E_FAIL;

	return S_OK;
}

void CMonster_Dummy::OnCollision(_uint iMyColliderLayer, CGameObject* pOther)
{
	Super::OnCollision(iMyColliderLayer, pOther);
}

void CMonster_Dummy::OnCollision_Enter(_uint iMyColliderLayer, CGameObject* pOther)
{
	Super::OnCollision_Enter(iMyColliderLayer, pOther);
}

void CMonster_Dummy::OnCollision_Exit(_uint iMyColliderLayer, CGameObject* pOther)
{
	Super::OnCollision_Exit(iMyColliderLayer, pOther);
}

void CMonster_Dummy::OnTrigger_Enter(_uint iMyColliderLayer, CGameObject* pOther)
{
	Super::OnTrigger_Enter(iMyColliderLayer, pOther);
}

void CMonster_Dummy::OnTrigger_Exit(_uint iMyColliderLayer, CGameObject* pOther)
{
	Super::OnTrigger_Exit(iMyColliderLayer, pOther);
}

HRESULT CMonster_Dummy::Ready_BaseStates()
{
	return S_OK;
}

HRESULT CMonster_Dummy::Ready_PartObjects()
{
	return S_OK;
}

HRESULT CMonster_Dummy::Ready_Components()
{
	return S_OK;
}

CMonster_Dummy* CMonster_Dummy::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CMonster_Dummy* pInsatnce = new CMonster_Dummy(pDevice, pDeviceContext);
	if (FAILED(pInsatnce->Initialize_Prototype()))
	{
		MSG_BOX("CMonster_Dummy::Create, Failed");
		Safe_Release(pInsatnce);
	}

	return pInsatnce;
}

CGameObject* CMonster_Dummy::Clone(void* pArg)
{
	CMonster_Dummy* pClone = new CMonster_Dummy(*this);
	if (FAILED(pClone->Initialize(pArg)))
	{
		MSG_BOX("CMonster_Dummy::Clone, Failed");
		Safe_Release(pClone);
	}
	return pClone;
}

void CMonster_Dummy::Free()
{
	Super::Free();
}
