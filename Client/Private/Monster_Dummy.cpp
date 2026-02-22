#include "pch.h"
#include "Monster_Dummy.h"

#include "GameInstance.h"


#include "Model.h"
#include "PhysicsCCT.h"

CMonster_Dummy::CMonster_Dummy(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: Super(pDevice, pDeviceContext)
{
	//m_vecPartObjects.resize(Part::END, nullptr);
}

CMonster_Dummy::CMonster_Dummy(const CMonster_Dummy& rhs)
	: Super(rhs)
{
	//m_vecPartObjects.resize(Part::END, nullptr);
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
	
	Get_Component<CTransform>()->Set_Info(TRANSFORM_INFO_STATE::POS, Vec3{ 21.f, 17.5f, -1.f });

	Get_Component<CPhysicsCCT>()->Awake();

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

#ifdef _DEBUG
	m_pGameInstance->Push_DebugComponent(Get_Component<CPhysicsCCT>());
#endif // _DEBUG
}

HRESULT CMonster_Dummy::Render()
{
	if (FAILED(Super::Render()))
		return E_FAIL;

	return S_OK;
}

_int CMonster_Dummy::Get_AnimationIndex(const wstring& wstrName)
{
	//if (CBody* pBody = Get_Part<CBody>(Part::BODY))
	//{
	//	if (CModel* pModel = pBody->Get_Component<CModel>())
	//	{
	//		return pModel->Get_AnimationIndex(wstrName);
	//	}
	//	return -1;
	//}
	return -1;
}

_wstring CMonster_Dummy::Get_AnimationName(_uint iAniIndex)
{
	//if (CBody* pBody = Get_Part<CBody>(Part::BODY))
	//{
	//	if (CModel* pModel = pBody->Get_Component<CModel>())
	//	{
	//		return pModel->Get_AnimationName(iAniIndex);
	//	}
	//	return L"";
	//}
	return L"";
}

void CMonster_Dummy::OnCollision(_uint iMyColliderLayer, CGameObject* pOther)
{
}

void CMonster_Dummy::OnCollision_Enter(_uint iMyColliderLayer, CGameObject* pOther)
{
}

void CMonster_Dummy::OnCollision_Exit(_uint iMyColliderLayer, CGameObject* pOther)
{
}

void CMonster_Dummy::OnTrigger_Enter(_uint iMyColliderLayer, CGameObject* pOther)
{
}

void CMonster_Dummy::OnTrigger_Exit(_uint iMyColliderLayer, CGameObject* pOther)
{
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
	if (FAILED(Ready_CCT()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMonster_Dummy::Ready_CCT()
{
	PHYSICSCCT_DESC desc;
	desc.pOwner = this;
	desc.bIsPlayer = false;
	desc.eType = EPhysicsCCTType::CAPSULE;
	desc.pOwnerMatrix = &Get_Component<CTransform>()->Get_WorldMatrix();
	desc.fRadius = 0.5f;
	desc.fHeight = 1.f;
	desc.vExtens = { 0.f, 0.f, 0.f };

	PHYSICSMATERIAL_DESC mtrlDesc{};
	mtrlDesc.eMaterial = EPhysicsMaterial::PLAYER;
	desc.tMaterial = mtrlDesc;

	desc.eFilterLayer = PHYSICSFILTERGROUP::Enum::MONSTER;
	desc.iFilterMask = 0xFFFFFFFF;

	if (FAILED(Add_Component<CPhysicsCCT>(0, L"Prototype_Component_Physics_CCT", &desc)))
		return E_FAIL;

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
