#include "pch.h"
#include "Client_Defines.h"

#include "Monster_Boomer_Body.h"
#include "Monster_Boomer.h"

#include "Bone.h"

// components
#include "ActionState.h"
#include "Shader.h"
#include "Model.h"
#include "PhysicsAttackOverlap.h"
#include "ComputeShader.h"

#include "GameInstance.h"

CMonster_Boomer_Body::CMonster_Boomer_Body(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: Super(pDevice, pDeviceContext)
{
}

CMonster_Boomer_Body::CMonster_Boomer_Body(const CMonster_Boomer_Body& rhs)
	: Super(rhs)
{
}

HRESULT CMonster_Boomer_Body::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMonster_Boomer_Body::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	Set_Name("Monster_Boomer_Body");

	MONSTERBODY_DESC* pDesc = static_cast<MONSTERBODY_DESC*>(pArg);
	if (FAILED(Ready_Components(pDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMonster_Boomer_Body::Awake(const _uint iCurrentLevelIndex)
{
	if (FAILED(Super::Awake(iCurrentLevelIndex)))
		return E_FAIL;

	CComputeShader* pAnimECS = static_cast<CComputeShader*>(Get_Script_Component(TEXT("ComputeShader_AnimE")));

	Get_Component<CModel>()->Change_Animation(pAnimECS, 1, true, true, false);

	return S_OK;
}

void CMonster_Boomer_Body::Update_Priority(_float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CMonster_Boomer_Body::Update(_float fTimeDelta)
{
	Super::Update(fTimeDelta);
}

void CMonster_Boomer_Body::Update_Late(_float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);
}

void CMonster_Boomer_Body::Ready_Before_Render(_float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);
}

void CMonster_Boomer_Body::OnCollision(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther)
{
	Super::OnCollision(iMyColliderLayer, iOtherLayer, pOther);
}

void CMonster_Boomer_Body::OnCollision_Enter(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther, const COL_HIT_INFO& tHitInfo)
{
	Super::OnCollision_Enter(iMyColliderLayer, iOtherLayer, pOther, tHitInfo);
}

void CMonster_Boomer_Body::OnCollision_Exit(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther)
{
	Super::OnCollision_Exit(iMyColliderLayer, iOtherLayer, pOther);
}

void CMonster_Boomer_Body::OnTrigger_Enter(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther, const COL_HIT_INFO& tHitInfo)
{
	Super::OnTrigger_Enter(iMyColliderLayer, iOtherLayer, pOther, tHitInfo);
}

void CMonster_Boomer_Body::OnTrigger_Exit(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther)
{
	Super::OnTrigger_Exit(iMyColliderLayer, iOtherLayer, pOther);
}

_bool CMonster_Boomer_Body::On_Hit(const HIT_DESC& hitDesc)
{
	return Super::On_Hit(hitDesc);
}

HRESULT CMonster_Boomer_Body::Render()
{
	if (FAILED(Super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMonster_Boomer_Body::Ready_Components(MONSTERBODY_DESC* pDesc)
{
	return S_OK;
}

HRESULT CMonster_Boomer_Body::Bind_ShaderResources()
{
	return S_OK;
}

CMonster_Boomer_Body* CMonster_Boomer_Body::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CMonster_Boomer_Body* pInstance = new CMonster_Boomer_Body(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CMonster_Boomer_Body::Create, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CMonster_Boomer_Body::Clone(void* pArg)
{
	CMonster_Boomer_Body* pInstance = new CMonster_Boomer_Body(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CMonster_Boomer_Body::Clone, Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMonster_Boomer_Body::Free()
{
	Super::Free();
}
