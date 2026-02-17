#include "pch.h"
#include "Client_Defines.h"
#include "Monster_Dummy_Body.h"

#include "Monster_Dummy.h"

#include "Bone.h"

// components
#include "ActionState.h"
#include "Shader.h"
#include "Model.h"
#include "PhysicsAttackOverlap.h"
#include "ComputeShader.h"

#include "GameInstance.h"

CMonster_Dummy_Body::CMonster_Dummy_Body(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: Super(pDevice, pDeviceContext)
{
}

CMonster_Dummy_Body::CMonster_Dummy_Body(const CMonster_Dummy_Body& rhs)
	: Super(rhs)
{
}

HRESULT CMonster_Dummy_Body::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMonster_Dummy_Body::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	Set_Name("Dummy_Monster_Body");

	MONSTERBODY_DESC* pDesc = static_cast<MONSTERBODY_DESC*>(pArg);
	if (FAILED(Ready_Components(pDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMonster_Dummy_Body::Awake(const _uint iCurrentLevelIndex)
{
	if (FAILED(Super::Awake(iCurrentLevelIndex)))
		return E_FAIL;

	CComputeShader* pAnimECS = static_cast<CComputeShader*>(Get_Script_Component(TEXT("ComputeShader_AnimE")));

	Get_Component<CModel>()->Change_Animation(pAnimECS, 1, true, true, false);

	return S_OK;
}

void CMonster_Dummy_Body::Update_Priority(_float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CMonster_Dummy_Body::Update(_float fTimeDelta)
{
	Super::Update(fTimeDelta);
}

void CMonster_Dummy_Body::Update_Late(_float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);
}

void CMonster_Dummy_Body::Ready_Before_Render(_float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);
}

void CMonster_Dummy_Body::OnCollision(_uint iMyColliderLayer, CGameObject* pOther)
{
	Super::OnCollision(iMyColliderLayer, pOther);
}

void CMonster_Dummy_Body::OnCollision_Enter(_uint iMyColliderLayer, CGameObject* pOther)
{
	Super::OnCollision_Enter(iMyColliderLayer, pOther);
}

void CMonster_Dummy_Body::OnCollision_Exit(_uint iMyColliderLayer, CGameObject* pOther)
{
	Super::OnCollision_Exit(iMyColliderLayer, pOther);
}

void CMonster_Dummy_Body::OnTrigger_Enter(_uint iMyColliderLayer, CGameObject* pOther)
{
	Super::OnTrigger_Enter(iMyColliderLayer, pOther);
}

void CMonster_Dummy_Body::OnTrigger_Exit(_uint iMyColliderLayer, CGameObject* pOther)
{
	Super::OnTrigger_Exit(iMyColliderLayer, pOther);
}

_bool CMonster_Dummy_Body::On_Hit(_uint iCollideMyLayer, ATTACK_DESC* pDesc, CGameObject* pOther)
{
	return Super::On_Hit(iCollideMyLayer, pDesc, pOther);
}

HRESULT CMonster_Dummy_Body::Render()
{
	if (FAILED(Super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMonster_Dummy_Body::Ready_Components(MONSTERBODY_DESC* pDesc)
{
	return S_OK;
}

HRESULT CMonster_Dummy_Body::Bind_ShaderResources()
{
	return S_OK;
}

CMonster_Dummy_Body* CMonster_Dummy_Body::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CMonster_Dummy_Body* pInstance = new CMonster_Dummy_Body(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CMonster_Dummy_Body::Create, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CMonster_Dummy_Body::Clone(void* pArg)
{
	CMonster_Dummy_Body* pInstance = new CMonster_Dummy_Body(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CMonster_Dummy_Body::Clone, Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMonster_Dummy_Body::Free()
{
	Super::Free();
}
