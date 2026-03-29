#include "pch.h"
#include "Client_EventDefine.h"

#include "NPC_Veteran.h"
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

CNPC_Veteran::CNPC_Veteran(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: Super(pDevice, pDeviceContext)
{
}

CNPC_Veteran::CNPC_Veteran(const CNPC_Veteran& rhs)
	: Super(rhs)
{
}

HRESULT CNPC_Veteran::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;

	Set_Object_Enum_Tag(OBJECT_ENUM_TAG::NPC_VETERAN);

	return S_OK;
}

HRESULT CNPC_Veteran::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_PartObjects()))
		return E_FAIL;

	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;

	if (FAILED(Ready_BaseStates()))
		return E_FAIL;

	// 상호작용
	{
		Set_Interact_Enable();
		Set_Interact_DefaultEnable();
		//Interact_SetDefaultDialogue(210);
	}

	return S_OK;
}

HRESULT CNPC_Veteran::Awake(const _uint iCurrentLevelID)
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

void CNPC_Veteran::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CNPC_Veteran::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
}

void CNPC_Veteran::Update_Late(const _float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);
}

void CNPC_Veteran::Ready_Before_Render(const _float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);
}

HRESULT CNPC_Veteran::Render()
{
	if (FAILED(Super::Render()))
		return E_FAIL;

	return S_OK;
}

void CNPC_Veteran::OnCollision(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther)
{
	Super::OnCollision(iMyColliderLayer, iOtherLayer, pOther);
}

void CNPC_Veteran::OnCollision_Enter(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther, const COL_HIT_INFO& tHitInfo)
{
	Super::OnCollision_Enter(iMyColliderLayer, iOtherLayer, pOther, tHitInfo);
}

void CNPC_Veteran::OnCollision_Exit(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther)
{
	Super::OnCollision_Exit(iMyColliderLayer, iOtherLayer, pOther);
}


void CNPC_Veteran::OnTrigger_Enter(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther, const COL_HIT_INFO& tHitInfo)
{
	Super::OnTrigger_Enter(iMyColliderLayer, iOtherLayer, pOther, tHitInfo);
}

void CNPC_Veteran::OnTrigger_Exit(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther)
{
	Super::OnTrigger_Exit(iMyColliderLayer, iOtherLayer, pOther);
}

_bool CNPC_Veteran::On_Hit(const HIT_DESC& hitDesc)
{
	return true;
}

void CNPC_Veteran::Try_Attack(const HIT_DESC& hitDesc)
{
}

HRESULT CNPC_Veteran::Ready_PartObjects()
{
	return S_OK;
}

HRESULT CNPC_Veteran::Ready_Components(void* pArg)
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
		if (FAILED(Add_Component<CMonsterActionState>(0, L"Prototype_Component_ActionState_Monster", &desc)))
			return E_FAIL;
	}

	CMonsterControlContext::MONSTER_CONTROLCONTEXT_DESC desc{};
	desc.fMeleeRange = 2.f;
	desc.fAttackRange = 4.f;
	desc.fCloseRange = 1.f;
	desc.fDetectionRange = 7.f;
	desc.fSpeed = 1.f;
	//desc.iSkillCount;
	//desc.vecSkillRange;

	if (FAILED(Add_Component<CMonsterControlContext>(0 /*static*/, L"Prototype_Component_ControlContext_Monster", &desc)))
		return E_FAIL;
	//{
	//	CUIIcon_Component::UI_ICON_COMP_DESC Desc = {};
	//	if (FAILED(Add_Script_Component(L"UIIconComp", L"Prototype_ScriptComponent_UIIcon", &Desc)))
	//		return E_FAIL;
	//}
	return S_OK;
}

CNPC_Base::NPC_DESC CNPC_Veteran::Get_PreSetDesc(_uint iLevelId)
{
	CNPC_Base::NPC_DESC npcDesc = {};
	npcDesc.iLevelIndex = iLevelId;
	npcDesc.pTransform_Desc = nullptr;

	npcDesc.wstrNPCText.clear();
	npcDesc.wstrNPCName = L"베테랑 월석 사냥꾼";
	npcDesc.vUITextrOffset = { 0.f,3.f,0.f };

	npcDesc.wstrBodyModelTag = g_wszNPC_Veteran_Model_Prototype_Tag;
	npcDesc.wstrPartBodyPrototypeTag = g_wszNPC_Veteran_Body_Prototype_Tag;
	npcDesc.wstrNPCStateTag = g_wszNPC_Veteran_State_Tag;

	{
		PHYSICSCCT_DESC desc;
		desc.pOwner = nullptr;
		desc.bIsPlayer = false;
		desc.eType = EPhysicsCCTType::CAPSULE;
		desc.pOwnerMatrix = nullptr;
		desc.fRadius = 0.3f;
		desc.fHeight = 0.7f;
		desc.vExtens = { 2.f, 2.f, 2.f };

		desc.fContactOffset = 0.01f;
		desc.fStepOffset = 0.2f;
		desc.fSlopeLimit = 0.7f;

		desc.vLocalOffset = {};
		desc.vWorldOffset = {};

		desc.bIsHover = { false };
		desc.fHoverOffset = { 1.f };

		PHYSICSMATERIAL_DESC mtrlDesc{};
		mtrlDesc.eMaterial = EPhysicsMaterial::PLAYER;
		desc.tMaterial = mtrlDesc;

		desc.eFilterLayer = PHYSICSFILTERGROUP::Enum::NPC;
		desc.iFilterMask =
			PHYSICSFILTERGROUP::Enum::NPC
			| PHYSICSFILTERGROUP::Enum::PLAYER
			| PHYSICSFILTERGROUP::Enum::MONSTER
			| PHYSICSFILTERGROUP::Enum::MAP
			| PHYSICSFILTERGROUP::Enum::DETECT_INTERACT;

		desc.bGravity = { true };
		desc.fGravity = { -35.f };
		desc.MSpeed = { 0.f, 4.5f };
		desc.MAccelRate = { 0.f, 10.f };
		desc.MDeAccelRate = { 0.f, 10.f };

		npcDesc.tCCTDesc = desc;
	}

	return npcDesc;
}

void CNPC_Veteran::QuestEnter()
{
	Super::QuestEnter();
}

void CNPC_Veteran::QuestExit()
{
	Super::QuestExit();

	Interact_SetDefaultDialogue(234);
}

void CNPC_Veteran::Interact()
{
	Super::Interact();
}

CNPC_Veteran* CNPC_Veteran::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CNPC_Veteran* pInsatnce = new CNPC_Veteran(pDevice, pDeviceContext);
	if (FAILED(pInsatnce->Initialize_Prototype()))
	{
		MSG_BOX("CNPC_Veteran::Create, Failed");
		Safe_Release(pInsatnce);
	}

	return pInsatnce;
}

CGameObject* CNPC_Veteran::Clone(void* pArg)
{
	CNPC_Veteran* pClone = new CNPC_Veteran(*this);
	if (FAILED(pClone->Initialize(pArg)))
	{
		MSG_BOX("CNPC_Veteran::Clone, Failed");
		Safe_Release(pClone);
	}
	return pClone;
}

void CNPC_Veteran::Free()
{
	Super::Free();
}
