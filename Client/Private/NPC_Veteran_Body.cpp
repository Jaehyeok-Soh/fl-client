#include "pch.h"
#include "Client_Defines.h"
#include "NPC_Veteran_Body.h"

#include "NPC_Veteran.h"

#include "GameInstance.h"

////////////
// State, Animation, Render
////////////
#include "Bone.h"
#include "ActionState.h"
#include "Shader.h"
#include "Model.h"
#include "ComputeShader.h"

CNPC_Veteran_Body::CNPC_Veteran_Body(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: Super(pDevice, pDeviceContext)
{
}

CNPC_Veteran_Body::CNPC_Veteran_Body(const CNPC_Veteran_Body& rhs)
	: Super(rhs)
{
}

HRESULT CNPC_Veteran_Body::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CNPC_Veteran_Body::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	Set_Name("NPC_Villager_1_Body");

	NPCBODY_DESC* pDesc = static_cast<NPCBODY_DESC*>(pArg);
	if (FAILED(Ready_Components(pDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CNPC_Veteran_Body::Awake(const _uint iCurrentLevelIndex)
{
	if (FAILED(Super::Awake(iCurrentLevelIndex)))
		return E_FAIL;

	CComputeShader* pAnimECS = static_cast<CComputeShader*>(Get_Script_Component(TEXT("ComputeShader_AnimE")));

	Get_Component<CModel>()->Change_Animation(pAnimECS, 1, true, true, false);

	return S_OK;
}

void CNPC_Veteran_Body::Update_Priority(_float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CNPC_Veteran_Body::Update(_float fTimeDelta)
{
	Super::Update(fTimeDelta);
}

void CNPC_Veteran_Body::Update_Late(_float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);
}

void CNPC_Veteran_Body::Ready_Before_Render(_float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);
}

void CNPC_Veteran_Body::OnCollision(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther)
{
	Super::OnCollision(iMyColliderLayer, iOtherLayer, pOther);
}

void CNPC_Veteran_Body::OnCollision_Enter(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther, const COL_HIT_INFO& tHitInfo)
{
	Super::OnCollision_Enter(iMyColliderLayer, iOtherLayer, pOther, tHitInfo);
}

void CNPC_Veteran_Body::OnCollision_Exit(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther)
{
	Super::OnCollision_Exit(iMyColliderLayer, iOtherLayer, pOther);
}

void CNPC_Veteran_Body::OnTrigger_Enter(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther, const COL_HIT_INFO& tHitInfo)
{
	Super::OnTrigger_Enter(iMyColliderLayer, iOtherLayer, pOther, tHitInfo);
}

void CNPC_Veteran_Body::OnTrigger_Exit(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther)
{
	Super::OnTrigger_Exit(iMyColliderLayer, iOtherLayer, pOther);
}

_bool CNPC_Veteran_Body::On_Hit(const HIT_DESC& hitDesc)
{
	return true;
}

HRESULT CNPC_Veteran_Body::Render()
{
	if (FAILED(Super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CNPC_Veteran_Body::Ready_Components(NPCBODY_DESC* pDesc)
{
	return S_OK;
}

HRESULT CNPC_Veteran_Body::Bind_ShaderResources()
{
	return S_OK;
}

CNPC_Veteran_Body* CNPC_Veteran_Body::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CNPC_Veteran_Body* pInstance = new CNPC_Veteran_Body(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CNPC_Veteran_Body::Create, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CNPC_Veteran_Body::Clone(void* pArg)
{
	CNPC_Veteran_Body* pInstance = new CNPC_Veteran_Body(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CNPC_Veteran_Body::Clone, Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CNPC_Veteran_Body::Free()
{
	Super::Free();
}
