#include "pch.h"
#include "Client_EventDefine.h"

#include "NPC_Kid_1.h"
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

CNPC_Kid_1::CNPC_Kid_1(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: Super(pDevice, pDeviceContext)
{
}

CNPC_Kid_1::CNPC_Kid_1(const CNPC_Kid_1& rhs)
	: Super(rhs)
{
}

HRESULT CNPC_Kid_1::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;

	Set_Object_Enum_Tag(OBJECT_ENUM_TAG::NPC_KID_1);

	return S_OK;
}

HRESULT CNPC_Kid_1::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	Set_Name("아이");

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
		Interact_SetDefaultDialogue(220);
	}

	return S_OK;
}

HRESULT CNPC_Kid_1::Awake(const _uint iCurrentLevelID)
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

void CNPC_Kid_1::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CNPC_Kid_1::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
}

void CNPC_Kid_1::Update_Late(const _float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);
}

void CNPC_Kid_1::Ready_Before_Render(const _float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);
}

HRESULT CNPC_Kid_1::Render()
{
	if (FAILED(Super::Render()))
		return E_FAIL;

	return S_OK;
}

void CNPC_Kid_1::OnCollision(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther)
{
	Super::OnCollision(iMyColliderLayer, iOtherLayer, pOther);
}

void CNPC_Kid_1::OnCollision_Enter(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther, const COL_HIT_INFO& tHitInfo)
{
	Super::OnCollision_Enter(iMyColliderLayer, iOtherLayer, pOther, tHitInfo);
}

void CNPC_Kid_1::OnCollision_Exit(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther)
{
	Super::OnCollision_Exit(iMyColliderLayer, iOtherLayer, pOther);
}

void CNPC_Kid_1::OnTrigger_Enter(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther, const COL_HIT_INFO& tHitInfo)
{
	Super::OnTrigger_Enter(iMyColliderLayer, iOtherLayer, pOther, tHitInfo);
}

void CNPC_Kid_1::OnTrigger_Exit(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther)
{
	Super::OnTrigger_Exit(iMyColliderLayer, iOtherLayer, pOther);
}

_bool CNPC_Kid_1::On_Hit(const HIT_DESC& hitDesc)
{
	return true;
}

void CNPC_Kid_1::Try_Attack(const HIT_DESC& hitDesc)
{
}

HRESULT CNPC_Kid_1::Ready_PartObjects()
{
	return S_OK;
}

HRESULT CNPC_Kid_1::Ready_Components(void* pArg)
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

CNPC_Base::NPC_DESC CNPC_Kid_1::Get_PreSetDesc(_uint iLevelId)
{
	CNPC_Base::NPC_DESC npcDesc = {};
	npcDesc.iLevelIndex = iLevelId;
	npcDesc.pTransform_Desc = nullptr;

	npcDesc.wstrBodyModelTag = g_wszNPC_Kid_1_Model_Prototype_Tag;
	npcDesc.wstrPartBodyPrototypeTag = g_wszNPC_Kid_1_Body_Prototype_Tag;
	npcDesc.wstrNPCStateTag = g_wszNPC_Kid_1_State_Tag;

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

void CNPC_Kid_1::QuestEnter()
{
	Super::QuestEnter();
}

void CNPC_Kid_1::QuestExit()
{
	Super::QuestExit();
}

void CNPC_Kid_1::Interact()
{
	Super::Interact();
}

CNPC_Kid_1* CNPC_Kid_1::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CNPC_Kid_1* pInsatnce = new CNPC_Kid_1(pDevice, pDeviceContext);
	if (FAILED(pInsatnce->Initialize_Prototype()))
	{
		MSG_BOX("CNPC_Kid_1::Create, Failed");
		Safe_Release(pInsatnce);
	}

	return pInsatnce;
}

CGameObject* CNPC_Kid_1::Clone(void* pArg)
{
	CNPC_Kid_1* pClone = new CNPC_Kid_1(*this);
	if (FAILED(pClone->Initialize(pArg)))
	{
		MSG_BOX("CNPC_Kid_1::Clone, Failed");
		Safe_Release(pClone);
	}
	return pClone;
}

void CNPC_Kid_1::Free()
{
	Super::Free();
}
