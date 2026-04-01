#include "pch.h"
#include "Client_Defines.h"
#include "Monster_Fly_Body.h"

#include "Monster_Dog.h"

#include "Bone.h"

// components
#include "ActionState.h"
#include "Shader.h"
#include "Model.h"
#include "PhysicsAttackOverlap.h"
#include "ComputeShader.h"

#include "GameInstance.h"

CMonster_Fly_Body::CMonster_Fly_Body(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: Super(pDevice, pDeviceContext)
{
}


CMonster_Fly_Body::CMonster_Fly_Body(const CMonster_Fly_Body& rhs)
	: Super(rhs)
{
}

HRESULT CMonster_Fly_Body::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;


	m_tCBMonsterEmotion.vSkinColor = {0.f,0.f,0.f,1.25f};

	return S_OK;
}

HRESULT CMonster_Fly_Body::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	Set_Name("Monster_Fly_Body");

	MONSTERBODY_DESC* pDesc = static_cast<MONSTERBODY_DESC*>(pArg);
	if (FAILED(Ready_Components(pDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMonster_Fly_Body::Awake(const _uint iCurrentLevelIndex)
{
	if (FAILED(Super::Awake(iCurrentLevelIndex)))
		return E_FAIL;

	CComputeShader* pAnimECS = static_cast<CComputeShader*>(Get_Script_Component(TEXT("ComputeShader_AnimE")));

	m_eEmotionType = EMonster_Emontion_Type::OnlyEye;

	Get_Component<CModel>()->Change_Animation(pAnimECS, 1, true, true, false);

	return S_OK;
}

HRESULT CMonster_Fly_Body::Ready_ShaderPass()
{
	CModel* pModel = Get_Component<CModel>();
	if (pModel == nullptr) return E_FAIL;

	_uint iMeshCount = pModel->Get_MeshCount();

	m_vecAnimShaderPass.reserve(iMeshCount);
	for (_uint i = 0; i < iMeshCount; ++i)
	{
		wstring wstrMtlName = pModel->Get_MaterialName(i);

		if (wstrMtlName.empty())
			continue;

		EAnimShaderPass ePass{ EAnimShaderPass::WithRenderFx };

		if (wstrMtlName.find(L"Face") != std::wstring::npos)
			ePass = EAnimShaderPass::MonsterFace;

		m_vecAnimShaderPass.push_back(_int(ePass));
	}

	return S_OK;
}


void CMonster_Fly_Body::Update_Priority(_float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CMonster_Fly_Body::Update(_float fTimeDelta)
{
	Super::Update(fTimeDelta);

}

void CMonster_Fly_Body::Update_Late(_float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);
}

void CMonster_Fly_Body::Ready_Before_Render(_float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);
}

void CMonster_Fly_Body::OnCollision(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther)
{
	Super::OnCollision(iMyColliderLayer, iOtherLayer, pOther);
}

void CMonster_Fly_Body::OnCollision_Enter(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther, const COL_HIT_INFO& tHitInfo)
{
	Super::OnCollision_Enter(iMyColliderLayer, iOtherLayer, pOther, tHitInfo);
}

void CMonster_Fly_Body::OnCollision_Exit(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther)
{
	Super::OnCollision_Exit(iMyColliderLayer, iOtherLayer, pOther);
}

void CMonster_Fly_Body::OnTrigger_Enter(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther, const COL_HIT_INFO& tHitInfo)
{
	Super::OnTrigger_Enter(iMyColliderLayer, iOtherLayer, pOther, tHitInfo);
}

void CMonster_Fly_Body::OnTrigger_Exit(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther)
{
	Super::OnTrigger_Exit(iMyColliderLayer, iOtherLayer, pOther);
}

_bool CMonster_Fly_Body::On_Hit(const HIT_DESC& hitDesc)
{
	return Super::On_Hit(hitDesc);
}

HRESULT CMonster_Fly_Body::Render()
{
	if (FAILED(Super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMonster_Fly_Body::Ready_Components(MONSTERBODY_DESC* pDesc)
{
	return S_OK;
}

HRESULT CMonster_Fly_Body::Bind_ShaderResources()
{
	return S_OK;
}

HRESULT CMonster_Fly_Body::Ready_DissolveEffect_Setting()
{
	using DS = DissolveEffectDesc;
	m_tDissolveDesc.Reset();
	m_tDissolveDesc.Add_DissolveFlag(DS::BIT_SPAWN_START,/* DS::BIT_USE_ALPHA_FADE, */DS::BIT_USE_DISSOLVE_MAP);
	m_tDissolveDesc.Set_Dissolve_Setting(2.f, 1.f);
	m_tDissolveDesc.Set_Spawn_Setting(1.f, 1.f);
	m_tDissolveDesc.Set_ObjectType(DS::DISSOLVE_OBJECTTYPE::TYPE_MONSTER);

	// 스폰 시간 & 디졸브 시간
	m_tDissolveDesc.ShaderData.fDissolveEdgeColor = SimpleMath::Vector3(0.9f, 0.2f, 1.0f);
	m_tDissolveDesc.ShaderData.fDissolveEdgeWidth = 0.06f;

	return S_OK;
}

void CMonster_Fly_Body::DissolveStart()
{
	using DS = DissolveEffectDesc;
	m_tDissolveDesc.Reset();
	m_tDissolveDesc.Add_DissolveFlag(DS::BIT_DISSOLVE_START, DS::BIT_USE_EDGE,/* DS::BIT_USE_ALPHA_FADE, */DS::BIT_USE_DISSOLVE_MAP);
}

CMonster_Fly_Body* CMonster_Fly_Body::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CMonster_Fly_Body* pInstance = new CMonster_Fly_Body(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CMonster_Fly_Body::Create, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CMonster_Fly_Body::Clone(void* pArg)
{
	CMonster_Fly_Body* pInstance = new CMonster_Fly_Body(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CMonster_Fly_Body::Clone, Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMonster_Fly_Body::Free()
{
	Super::Free();
}
