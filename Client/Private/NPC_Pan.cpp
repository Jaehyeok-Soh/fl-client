#include "pch.h"
#include "Client_EventDefine.h"

#include "NPC_Pan.h"
#include "NPC_Body_Base.h"

#include "GameInstance.h"

////////////
// State, Animation, Render
////////////
#include "MonsterActionState.h"
#include "MonsterControlContext.h"
#include "StateBase_Monster.h"
#include "Model.h"
#include "ComputeShader.h"

////////////
// Physics
////////////
#include "PhysicsCCT.h"

////////////
// UI
////////////
#include "UI_Manager.h"
#include "UIIcon_Component.h"

CNPC_Pan::CNPC_Pan(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: Super(pDevice, pDeviceContext)
{
}

CNPC_Pan::CNPC_Pan(const CNPC_Pan& rhs)
	: Super(rhs)
{
}

HRESULT CNPC_Pan::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;

	Set_Object_Enum_Tag(OBJECT_ENUM_TAG::NPC_PAN);

	return S_OK;
}

HRESULT CNPC_Pan::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	Set_Name("마령 판신");

	if (FAILED(Ready_PartObjects()))
		return E_FAIL;

	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;

	if (FAILED(Ready_BaseStates()))
		return E_FAIL;

	return S_OK;
}

HRESULT CNPC_Pan::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;
	//{
	//	UI_PREFAB_DATA tPrefabData = {};
	//	UI_NAMEPLATE_PREFAB_DATA Desc = {};
	//	Desc.pTarget = this;
	//	Desc.vOffset = Vec3{ 0.f, 1.f, 0.f };
	//	tPrefabData.Data = Desc;
	//	CUI_Manager::GetInstance()->Request_Add_Prefab(iCurrentLevelID, EUIPrefabType::MONSTER_NAMEPLATE, iCurrentLevelID, &tPrefabData);
	//}



	return S_OK;
}

void CNPC_Pan::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}


void CNPC_Pan::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
}

void CNPC_Pan::Update_Late(const _float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);
}

void CNPC_Pan::Ready_Before_Render(const _float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);
}

HRESULT CNPC_Pan::Render()
{
	if (FAILED(Super::Render()))
		return E_FAIL;

	return S_OK;
}

void CNPC_Pan::OnCollision(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther)
{
	Super::OnCollision(iMyColliderLayer, iOtherLayer, pOther);
}

void CNPC_Pan::OnCollision_Enter(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther, const COL_HIT_INFO& tHitInfo)
{
	Super::OnCollision_Enter(iMyColliderLayer, iOtherLayer, pOther, tHitInfo);
}

void CNPC_Pan::OnCollision_Exit(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther)
{
	Super::OnCollision_Exit(iMyColliderLayer, iOtherLayer, pOther);
}

void CNPC_Pan::OnTrigger_Enter(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther, const COL_HIT_INFO& tHitInfo)
{
	Super::OnTrigger_Enter(iMyColliderLayer, iOtherLayer, pOther, tHitInfo);
}

void CNPC_Pan::OnTrigger_Exit(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther)
{
	Super::OnTrigger_Exit(iMyColliderLayer, iOtherLayer, pOther);
}

_bool CNPC_Pan::On_Hit(const HIT_DESC& hitDesc)
{
	return true;
}

void CNPC_Pan::Try_Attack(const HIT_DESC& hitDesc)
{
}

HRESULT CNPC_Pan::Ready_PartObjects()
{
	return S_OK;
}

HRESULT CNPC_Pan::Ready_Components(void* pArg)
{
	// TODO : Dummy나 파생클래스의 Desc가 생긴다면 수정해야함
	NPC_DESC* pDesc = static_cast<NPC_DESC*>(pArg);

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
		desc.pOwnerModel = Get_Part<CNPC_Body_Base>(ENUM_TO_UINT(Part::BODY))->Get_Component<CModel>();
		desc.pOwnerAnimECS = static_cast<CComputeShader*>(Get_Part<CNPC_Body_Base>(ENUM_TO_UINT(Part::BODY))->Get_Script_Component(TEXT("ComputeShader_AnimE")));
		desc.wstrMonsterStateTag = pDesc->wstrNPCStateTag;
		desc.iLevelIndex = pDesc->iLevelIndex;
		if (FAILED(Add_Component<CMonsterActionState>(0, L"Prototype_Component_ActionState_NPC", &desc)))
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

	if (FAILED(Add_Component<CMonsterControlContext>(0 /*static*/, L"Prototype_Component_ControlContext_NPC", &desc)))
		return E_FAIL;
	//{
//	CUIIcon_Component::UI_ICON_COMP_DESC Desc = {};
//	if (FAILED(Add_Script_Component(L"UIIconComp", L"Prototype_ScriptComponent_UIIcon", &Desc)))
//		return E_FAIL;
//}
	return S_OK;
}

CNPC_Base::NPC_DESC CNPC_Pan::Get_PreSetDesc(_uint iLevelId)
{
	return NPC_DESC();
}

void CNPC_Pan::QuestEnter()
{
}

void CNPC_Pan::QuestExit()
{
}

void CNPC_Pan::Interact()
{
}

CNPC_Pan* CNPC_Pan::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	return nullptr;
}

CGameObject* CNPC_Pan::Clone(void* pArg)
{
	return nullptr;
}

void CNPC_Pan::Free()
{
}
