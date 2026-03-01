#include "pch.h"
#include "Monster_Dog.h"
#include "Monster_Body_Base.h"
#include "MonsterActionState.h"
#include "MonsterControlContext.h"
#include "StateBase_Monster.h"
#include "Model.h"
#include "PhysicsCCT.h"
#include "ComputeShader.h"
#include "UI_Manager.h"
#include "GameInstance.h"

CMonster_Dog::CMonster_Dog(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: Super(pDevice, pDeviceContext)
{
	m_eMonsterType = EMonster_Type::Dog;
}

CMonster_Dog::CMonster_Dog(const CMonster_Dog& rhs)
	: Super(rhs)
{
}

HRESULT CMonster_Dog::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMonster_Dog::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	//if (FAILED(Ready_Ability()))
	//	return E_FAIL;

	Set_Name("Monster_Dog");

	if (FAILED(Ready_PartObjects()))
		return E_FAIL;

	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;

	if (FAILED(Ready_BaseStates()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMonster_Dog::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;
	{
		UI_PREFAB_DATA Desc = {};
		Desc.pTarget = this;
		CUI_Manager::GetInstance()->Request_Add_Prefab(iCurrentLevelID, EUIPrefabType::MONSTER_NAMEPLATE, iCurrentLevelID, &Desc);
	}
	return S_OK;
}

void CMonster_Dog::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CMonster_Dog::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
}

void CMonster_Dog::Update_Late(const _float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);
}

void CMonster_Dog::Ready_Before_Render(const _float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);
}

HRESULT CMonster_Dog::Render()
{
	if (FAILED(Super::Render()))
		return E_FAIL;

	return S_OK;
}

void CMonster_Dog::OnCollision(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther)
{
	Super::OnCollision(iMyColliderLayer, iOtherLayer, pOther);
}

void CMonster_Dog::OnCollision_Enter(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther, const COL_HIT_INFO& tHitInfo)
{
	Super::OnCollision_Enter(iMyColliderLayer, iOtherLayer, pOther, tHitInfo);
}

void CMonster_Dog::OnCollision_Exit(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther)
{
	Super::OnCollision_Exit(iMyColliderLayer, iOtherLayer, pOther);
}

void CMonster_Dog::OnTrigger_Enter(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther)
{
	Super::OnTrigger_Enter(iMyColliderLayer, iOtherLayer, pOther);
}

void CMonster_Dog::OnTrigger_Exit(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther)
{
	Super::OnTrigger_Exit(iMyColliderLayer, iOtherLayer, pOther);
}

_bool CMonster_Dog::On_Hit(const HIT_DESC& hitDesc)
{
	Super::On_Hit(hitDesc);

	return true;
}

void CMonster_Dog::Try_Attack(const HIT_DESC& hitDesc)
{
	Super::Try_Attack(hitDesc);
}

HRESULT CMonster_Dog::Ready_BaseStates()
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

HRESULT CMonster_Dog::Ready_PartObjects()
{
	return S_OK;
}

HRESULT CMonster_Dog::Ready_Components(void* pArg)
{
	// TODO : Dummy나 파생클래스의 Desc가 생긴다면 수정해야함
	MONSTER_DESC* pDesc = static_cast<MONSTER_DESC*>(pArg);

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

CMonster_Dog* CMonster_Dog::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CMonster_Dog* pInsatnce = new CMonster_Dog(pDevice, pDeviceContext);
	if (FAILED(pInsatnce->Initialize_Prototype()))
	{
		MSG_BOX("CMonster_Dog::Create, Failed");
		Safe_Release(pInsatnce);
	}

	return pInsatnce;
}

CGameObject* CMonster_Dog::Clone(void* pArg)
{
	CMonster_Dog* pClone = new CMonster_Dog(*this);
	if (FAILED(pClone->Initialize(pArg)))
	{
		MSG_BOX("CMonster_Dog::Clone, Failed");
		Safe_Release(pClone);
	}
	return pClone;
}

void CMonster_Dog::Free()
{
	Super::Free();
}
