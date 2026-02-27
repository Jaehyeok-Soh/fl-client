#include "pch.h"
#include "Monster_Dummy.h"
#include "Monster_Body_Base.h"
#include "MonsterActionState.h"
#include "MonsterControlContext.h"
#include "StateBase_Monster.h"
#include "Model.h"
#include "PhysicsCCT.h"
#include "ComputeShader.h"
#include "GameInstance.h"

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

	//if (FAILED(Ready_Ability()))
	//	return E_FAIL;

	Set_Name("Monster_Dummy");

	if (FAILED(Ready_PartObjects()))
		return E_FAIL;

	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;

	if (FAILED(Ready_BaseStates()))
		return E_FAIL;

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

void CMonster_Dummy::OnCollision(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther)
{
	Super::OnCollision(iMyColliderLayer, iOtherLayer, pOther);
}

void CMonster_Dummy::OnCollision_Enter(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther, const COL_HIT_INFO& tHitInfo)
{
	Super::OnCollision_Enter(iMyColliderLayer, iOtherLayer, pOther, tHitInfo);
}

void CMonster_Dummy::OnCollision_Exit(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther)
{
	Super::OnCollision_Exit(iMyColliderLayer, iOtherLayer, pOther);
}

void CMonster_Dummy::OnTrigger_Enter(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther)
{
	Super::OnTrigger_Enter(iMyColliderLayer, iOtherLayer, pOther);
}

void CMonster_Dummy::OnTrigger_Exit(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther)
{
	Super::OnTrigger_Exit(iMyColliderLayer, iOtherLayer, pOther);
}

HRESULT CMonster_Dummy::Ready_BaseStates()
{
	CMonsterActionState* pActionState = { nullptr };
	CModel* pModel = Get_Part<CMonster_Body_Base>(Part::BODY)->Get_Component<CModel>();
	if (!pModel)
		return E_FAIL;

	if (!(pActionState = Get_Component<CMonsterActionState>()))
		return E_FAIL;

	TIME_COUNTER tStateLifeTime = {};
	TIME_COUNTER tStateCoolDownTime = {};

	return S_OK;
}

HRESULT CMonster_Dummy::Ready_PartObjects()
{
	return S_OK;
}

HRESULT CMonster_Dummy::Ready_Components(void* pArgs)
{
	// TODO : Dummy나 파생클래스의 Desc가 생긴다면 수정해야함
	MONSTER_DESC* pDesc = static_cast<MONSTER_DESC*>(pArgs);

	//typedef struct tagMonsterControlContextDesc
	//{
	//	_float fMeleeRange = {};
	//	_float fAttackRange = {};
	//	_float fCloseRange = {};
	//	_float fDetectionRange = {};
	//	_float fSpeed = {};
	//	_int iSkillCount = { -1 };
	//	vector<_int> vecSkillRange;
	//}MONSTER_CONTROLCONTEXT_DESC;
	{
		CMonsterActionState::MONSTERACTIONSTATE_DESC desc = {};
		desc.pOwnerModel = Get_Part<CMonster_Body_Base>(ENUM_TO_UINT(Part::BODY))->Get_Component<CModel>();
		desc.pOwnerAnimECS = static_cast<CComputeShader*>(Get_Part<CMonster_Body_Base>(ENUM_TO_UINT(Part::BODY))->Get_Script_Component(TEXT("ComputeShader_AnimE")));
		desc.wstrMonsterStateTag = pDesc->wstrMonsterStateTag;
		desc.iLevelIndex = pDesc->iLevelIndex;
		if (FAILED(Add_Component<CMonsterActionState>(0, L"Prototype_Component_ActionState_Monster", &desc)))
			return E_FAIL;
	}

	CMonsterControlContext::MONSTER_CONTROLCONTEXT_DESC desc{};
	desc.fMeleeRange = 2.f;
	desc.fAttackRange = 4.f;
	desc.fCloseRange = 1.f;
	desc.fDetectionRange = 15.f;
	desc.fSpeed = 1.f;
	//desc.iSkillCount;
	//desc.vecSkillRange;

	if (FAILED(Add_Component<CMonsterControlContext>(0 /*static*/, L"Prototype_Component_ControlContext_Monster", &desc)))
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
