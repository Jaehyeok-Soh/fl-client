#include "pch.h"
#include "NPC_Base.h"

#include "GameInstance.h"

#include "NPC_Body_Base.h"

////////////
// State, Animation, Render
////////////
#include "MonsterControlContext.h"
#include "MonsterActionState.h"
#include "Model.h"
#include "ComputeShader.h"

////////////
// Physics
////////////
#include "PhysicsCCT.h"
#include "PhysicsCollider.h"

////////////
// Effect
////////////
#include "EffectHandler.h"
#include "RenderFx.h"

////////////
// UI
////////////
#include "UIMinimap_Manager.h"
#include "UI_Manager.h"

////////////
// Quest
////////////
#include "QuestManager.h"

////////////
// Dialogue
////////////
#include "DialogueManager.h"

////////////
// NPC
////////////
#include "NPC_Pan.h"
#include "NPC_Tavern.h"
#include "NPC_Villager_1.h"
#include "NPC_Kid_1.h"
#include "NPC_Citizen.h"
#include "NPC_Veteran.h"

CNPC_Base::CNPC_Base(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: Super(pDevice, pDeviceContext)
{
	m_vecPartObjects.resize(Part::END, nullptr);
}

CNPC_Base::CNPC_Base(const CNPC_Base& rhs)
	: Super(rhs)
{
	m_vecPartObjects.resize(Part::END, nullptr);
}

HRESULT CNPC_Base::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;

	Set_Object_Enum_Tag(OBJECT_ENUM_TAG::NPC_DEFAULT);

	return S_OK;
}

HRESULT CNPC_Base::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_PartObjects(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;

	//if (FAILED(Ready_EffectHandler(pArg)))
	//	return E_FAIL;

	NPC_DESC* pDesc = static_cast<NPC_DESC*>(pArg);

	if (pDesc->bHasQuest)
		Ready_Quest(&pDesc->tQuestObjectDesc);

	return S_OK;
}

HRESULT CNPC_Base::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;

	CGameInstance::GetInstance()->Add_Actor_Object(this);
	if (CMonsterActionState* pMonsterState = Get_Component<CMonsterActionState>())
		if (FAILED(pMonsterState->Awake(iCurrentLevelID)))
			return E_FAIL;

	if (FAILED(Get_Component<CMonsterActionState>()->Change_State(0)))
		return E_FAIL;

	if (FAILED(Get_Component<CControlContext>()->Awake(iCurrentLevelID)))
		return E_FAIL;

	Get_Component<CPhysicsCCT>()->Ready_Position();

	{
		UI_PREFAB_DATA tPrefabData = {};
		UI_NPC_TEXT_BUBBLE_PREFAB_DATA Desc = {};
		Desc.pTarget = this;
		Desc.vOffset = Vec3{ 0.f, 1.f, 0.f };
		// 매번 다른 텍스트 말하게 하고 싶으면 말해주세요.
		Desc.wstrContents = L"일어나!";
		tPrefabData.Data = Desc;
		CUI_Manager::GetInstance()->Request_Add_Prefab(iCurrentLevelID, EUIPrefabType::NPC_TEXT_BUBBLE, iCurrentLevelID, &tPrefabData);
	}

	return S_OK;
}

void CNPC_Base::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CNPC_Base::Update(const _float fTimeDelta)
{
	if (CMonsterControlContext* pMonsterControlContext = Get_Component<CMonsterControlContext>())
		pMonsterControlContext->Update_RuntimeDesc(fTimeDelta);

	if (CMonsterActionState* pMonsterState = Get_Component<CMonsterActionState>())
		pMonsterState->Update(fTimeDelta);

	if (m_pEffectHandler)
		m_pEffectHandler->Update(fTimeDelta);

	Super::Update(fTimeDelta);
}

void CNPC_Base::Update_Late(const _float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);
}

void CNPC_Base::Ready_Before_Render(const _float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);

#ifdef _DEBUG
	m_pGameInstance->Push_DebugComponent(Get_Component<CPhysicsCCT>());
#endif // _DEBUG
}

HRESULT CNPC_Base::Render()
{
	if (FAILED(Super::Render()))
		return E_FAIL;

	return S_OK;
}

_int CNPC_Base::Get_AnimationIndex(const wstring& wstrName)
{
	if (CNPC_Body_Base* pBody = Get_Part<CNPC_Body_Base>(Part::BODY))
	{
		if (CModel* pModel = pBody->Get_Component<CModel>())
		{
			return pModel->Get_AnimationIndex(wstrName);
		}
		return -1;
	}

	return -1;
}

_wstring CNPC_Base::Get_AnimationName(_uint iAniIndex)
{
	if (CNPC_Body_Base* pBody = Get_Part<CNPC_Body_Base>(Part::BODY))
	{
		if (CModel* pModel = pBody->Get_Component<CModel>())
		{
			return pModel->Get_AnimationName(iAniIndex);
		}
		return L"";
	}

	return L"";
}

void CNPC_Base::OnCollision(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther)
{
}

void CNPC_Base::OnCollision_Enter(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther, const COL_HIT_INFO& tHitInfo)
{
	COLLIDED_DESC collidedDesc{};
	collidedDesc.iCollisionType = COLLISIONEVENT::ON_COLLISION_ENTER;
	collidedDesc.iRequesterLayer = iMyColliderLayer;
	collidedDesc.iOtherLayer = iOtherLayer;
	collidedDesc.pRequester = this;
	collidedDesc.pOther = pOther;
	collidedDesc.tHitInfo = tHitInfo;

	m_pGameInstance->Push_CollidedData(collidedDesc);
}

void CNPC_Base::OnCollision_Exit(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther)
{
}

void CNPC_Base::OnTrigger_Enter(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther, const COL_HIT_INFO& tHitInfo)
{
	COLLIDED_DESC collidedDesc{};
	collidedDesc.iCollisionType = COLLISIONEVENT::ON_TRIGGER_ENTER;
	collidedDesc.iRequesterLayer = iMyColliderLayer;
	collidedDesc.iOtherLayer = iOtherLayer;
	collidedDesc.pRequester = this;
	collidedDesc.pOther = pOther;
	collidedDesc.tHitInfo = tHitInfo;

	m_pGameInstance->Push_CollidedData(collidedDesc);

	if (Is_Interact_Enabled() && iOtherLayer == PHYSICSFILTERGROUP::DETECT_INTERACT)
	{
		if (Is_Interact_Enabled())
			m_pGameInstance->Broadcast<INTERACT_DETECT>(this);
	}
}

void CNPC_Base::OnTrigger_Exit(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther)
{
	if (iOtherLayer == PHYSICSFILTERGROUP::DETECT_INTERACT)
	{
		if (Is_Interact_Enabled())
			m_pGameInstance->Broadcast<INTERACT_LOST>(this);
	}
}

_bool CNPC_Base::On_Hit(const HIT_DESC& hitDesc)
{
	return true;
}

void CNPC_Base::Try_Attack(const HIT_DESC& hitDesc)
{
}

void CNPC_Base::Set_RootMotion_Apply(_bool bApply)
{
	Get_Part<CNPC_Body_Base>(Part::BODY)->Get_Component<CModel>()->Set_CurAnimation_RootApply(bApply);
}

HRESULT CNPC_Base::Ready_BaseStates()
{
	return S_OK;
}

HRESULT CNPC_Base::Ready_PartObjects(void* pArg)
{
	NPC_DESC* pDesc = static_cast<NPC_DESC*>(pArg);

	if(!pDesc->wstrBodyModelTag.empty())
	{
		CNPC_Body_Base::NPCBODY_DESC bodyDesc = {};
		bodyDesc.pMatParent = &Get_Component<CTransform>()->Get_WorldMatrix();
		bodyDesc.iLevelIndex = pDesc->iLevelIndex;
		bodyDesc.wstrModelPrototypeTag = pDesc->wstrBodyModelTag;
		bodyDesc.spanBoneNames = pDesc->spanBoneNames;
		if (FAILED(Add_Part(Part::BODY, ENUM_TO_UINT(ELevelType::STATIC), pDesc->wstrPartBodyPrototypeTag, &bodyDesc)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CNPC_Base::Ready_Components(void* pArgs)
{
	NPC_DESC* pDesc = static_cast<NPC_DESC*>(pArgs);

	if (FAILED(Ready_CCT(pArgs)))
		return E_FAIL;

	return S_OK;
}

HRESULT CNPC_Base::Ready_EffectHandler(void* pArgs)
{
	NPC_DESC* pDesc = static_cast<NPC_DESC*>(pArgs);

	wstring NameTag = pDesc->wstrBodyModelTag;
	Engine_Utils::Replace(NameTag, L"Prototype_Component_Model_", L"");

	if (FAILED(Add_Component<CEffectHandler>(/*Static*/0, L"Prototype_Component_EffectHandler_" + NameTag, nullptr)))
		return E_FAIL;

	m_pEffectHandler = Get_Component<CEffectHandler>();
	return S_OK;
}

HRESULT CNPC_Base::Ready_CCT(void* pArgs)
{
	NPC_DESC* pDesc = static_cast<NPC_DESC*>(pArgs);
	pDesc->tCCTDesc.pOwner = this;
	pDesc->tCCTDesc.pOwnerMatrix = &Get_Component<CTransform>()->Get_WorldMatrix();

	if (FAILED(Add_Component<CPhysicsCCT>(ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_Component_Physics_CCT", &pDesc->tCCTDesc)))
		return E_FAIL;

	{
		// CCT, PhysicsCollider 세트
		PHYSICSCOLLIDER_DESC cloneDesc{};
		cloneDesc.eFilterLayer = pDesc->tCCTDesc.eFilterLayer;
		cloneDesc.iFilterMask = pDesc->tCCTDesc.iFilterMask;
		cloneDesc.bSetOnlyFilter = true;

		if (FAILED(Add_Component<CPhysicsCollider>(ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_Component_Physics_Collider", &cloneDesc)))
			return E_FAIL;
	}

	return S_OK;
}

void CNPC_Base::Ready_Quest(vector<DTO::QUEST_CHAPTERDESC>* pQuestDesc)
{
	for (auto desc : *pQuestDesc)
		CQuestManager::GetInstance()->Register_QuestObject(desc, this);
}

void CNPC_Base::QuestEnter()
{
	if (Is_Quest_Enabled() && m_eQuestEvent == DTO::QUESTEVENT::NPC_TALK)
	{
		auto chapterDesc = CQuestManager::GetInstance()->Get_QuestChapterInfo();

		auto iter = std::find(chapterDesc.eTargetType.begin(), chapterDesc.eTargetType.end(), m_eObject_Enum_Tag);

		if (iter != chapterDesc.eTargetType.end() && chapterDesc.tQuestDesc.iEnterDialogueId != -1)
		{
			CDialogueManager::GetInstance()->Start_Dialogue(chapterDesc.tQuestDesc.iEnterDialogueId);

			CallQuestEvent(m_eObject_Enum_Tag, 1);
		}
	}

	Set_Interact_Enable();
}

void CNPC_Base::QuestExit()
{
	if (Is_Quest_Enabled() && m_eQuestEvent == DTO::QUESTEVENT::NPC_TALK)
	{
		auto chapterDesc = CQuestManager::GetInstance()->Get_QuestChapterInfo();

		auto iter = std::find(chapterDesc.eTargetType.begin(), chapterDesc.eTargetType.end(), m_eObject_Enum_Tag);

		if (iter != chapterDesc.eTargetType.end() && chapterDesc.tQuestDesc.iExitDialogueId != -1)
		{
			CDialogueManager::GetInstance()->Start_Dialogue(chapterDesc.tQuestDesc.iExitDialogueId);

			CallQuestEvent(m_eObject_Enum_Tag, 1);
		}
	}

	if (Is_Interact_DefaultEnabled() == false)
		Set_Interact_Disable();
}

void CNPC_Base::Interact()
{
	if (Is_Quest_Enabled() && m_eQuestEvent == DTO::QUESTEVENT::NPC_TALK)
	{
		auto chapterDesc = CQuestManager::GetInstance()->Get_QuestChapterInfo();

		auto iter = std::find(chapterDesc.eTargetType.begin(), chapterDesc.eTargetType.end(), m_eObject_Enum_Tag);

		if (iter != chapterDesc.eTargetType.end() && chapterDesc.tQuestDesc.iInteractDialogueId != -1)
			CDialogueManager::GetInstance()->Start_Dialogue(chapterDesc.tQuestDesc.iInteractDialogueId);
	}
	else
	{
		CDialogueManager::GetInstance()->Start_Dialogue(m_iDefaultDialogueId);
	}

	CallQuestEvent(m_eObject_Enum_Tag, 1);
}

HRESULT CNPC_Base::Create_NPC(BATCH_NPC_DESC* pDesc, _uint iFindPrototypeLevelType, _uint iAddLevelType, CTransform::TRANSFORM_DESC* pTransformDesc)
{
	CGameObject* pResult{ nullptr };

	_uint	iFindPrototypeIndex = ENUM_TO_UINT(ELevelType::STATIC);
	wstring wstrAddLayerName{};
	wstring wstrFindPrototypeName{};

	CNPC_Base::NPC_DESC npcDesc = {};
	npcDesc.iLevelIndex = iAddLevelType;
	npcDesc.pTransform_Desc = pTransformDesc;

	npcDesc.bHasQuest = pDesc->bHasQuest;
	npcDesc.tQuestObjectDesc = pDesc->tQuestObjectDesc;

	wstrAddLayerName = g_wszNPCeLayer;

	void* pArg{nullptr};
	switch (pDesc->eBatchNPCType)
	{
	case Engine::EObjectEnumTag::NPC_DEFAULT:
		break;
	case Engine::EObjectEnumTag::NPC_PAN:
	{
		npcDesc = CNPC_Pan::Get_PreSetDesc(npcDesc.iLevelIndex);
		npcDesc.iLevelIndex = iAddLevelType;
		npcDesc.pTransform_Desc = pTransformDesc;
		
		npcDesc.bHasQuest = pDesc->bHasQuest;
		npcDesc.tQuestObjectDesc = pDesc->tQuestObjectDesc;

		npcDesc.bHasQuest = pDesc->bHasQuest;
		npcDesc.tQuestObjectDesc = pDesc->tQuestObjectDesc;

		wstrFindPrototypeName = g_wszNPC_Pan_Prototype_Tag;
		wstrAddLayerName = g_wszNPCeLayer;
		pArg = &npcDesc;
	}
		break;
	case Engine::EObjectEnumTag::NPC_BERENICA:
		break;
	case Engine::EObjectEnumTag::NPC_TAVERN:
	{
		npcDesc = CNPC_Tavern::Get_PreSetDesc(npcDesc.iLevelIndex);
		npcDesc.iLevelIndex = iAddLevelType;
		npcDesc.pTransform_Desc = pTransformDesc;

		npcDesc.bHasQuest = pDesc->bHasQuest;
		npcDesc.tQuestObjectDesc = pDesc->tQuestObjectDesc;

		npcDesc.bHasQuest = pDesc->bHasQuest;
		npcDesc.tQuestObjectDesc = pDesc->tQuestObjectDesc;

		wstrFindPrototypeName = g_wszNPC_Tavern_Prototype_Tag;
		wstrAddLayerName = g_wszNPCeLayer;
		pArg = &npcDesc;

	}
		break;
	case Engine::EObjectEnumTag::NPC_VILLAGER_1:
	{
		npcDesc = CNPC_Villager_1::Get_PreSetDesc(npcDesc.iLevelIndex);
		npcDesc.iLevelIndex = iAddLevelType;
		npcDesc.pTransform_Desc = pTransformDesc;

		npcDesc.bHasQuest = pDesc->bHasQuest;
		npcDesc.tQuestObjectDesc = pDesc->tQuestObjectDesc;

		npcDesc.bHasQuest = pDesc->bHasQuest;
		npcDesc.tQuestObjectDesc = pDesc->tQuestObjectDesc;

		wstrFindPrototypeName = g_wszNPC_Villager_1_Prototype_Tag;
		wstrAddLayerName = g_wszNPCeLayer;
		pArg = &npcDesc;
	}
	break;
	case Engine::EObjectEnumTag::NPC_KID_1:
	{
		npcDesc = CNPC_Kid_1::Get_PreSetDesc(npcDesc.iLevelIndex);
		npcDesc.iLevelIndex = iAddLevelType;
		npcDesc.pTransform_Desc = pTransformDesc;

		npcDesc.bHasQuest = pDesc->bHasQuest;
		npcDesc.tQuestObjectDesc = pDesc->tQuestObjectDesc;

		npcDesc.bHasQuest = pDesc->bHasQuest;
		npcDesc.tQuestObjectDesc = pDesc->tQuestObjectDesc;

		wstrFindPrototypeName = g_wszNPC_Kid_1_Prototype_Tag;
		wstrAddLayerName = g_wszNPCeLayer;
		pArg = &npcDesc;
	}
	break;
	case Engine::EObjectEnumTag::NPC_CITIZEN:
	{
		static CNPC_Citizen::NPC_CITIZEN_DESC tDesc{};
		static_cast<CNPC_Citizen::NPC_DESC&>(tDesc) = CNPC_Citizen::Get_PreSetDesc(npcDesc.iLevelIndex);
		tDesc.pTransform_Desc = pTransformDesc;
		tDesc.tCitizenData				= pDesc->tNpcCitizenData;
		tDesc.wstrPartBodyPrototypeTag	= g_wszNPC_Citizen_Body_Prototype_Tag;
		wstrFindPrototypeName			= g_wszNPC_Citizen_Prototype_Tag;
		wstrAddLayerName				= g_wszNPCeLayer;

		pArg = &tDesc;
	}
	break;
	case Engine::EObjectEnumTag::NPC_VETERAN:
	{
		npcDesc = CNPC_Veteran::Get_PreSetDesc(npcDesc.iLevelIndex);
		npcDesc.iLevelIndex = iAddLevelType;
		npcDesc.pTransform_Desc = pTransformDesc;

		npcDesc.bHasQuest = pDesc->bHasQuest;
		npcDesc.tQuestObjectDesc = pDesc->tQuestObjectDesc;

		npcDesc.bHasQuest = pDesc->bHasQuest;
		npcDesc.tQuestObjectDesc = pDesc->tQuestObjectDesc;

		wstrFindPrototypeName = g_wszNPC_Veteran_Prototype_Tag;
		wstrAddLayerName = g_wszNPCeLayer;
		pArg = &npcDesc;
	}
	break;
	default:
		return E_FAIL;
	}

	if (!(pResult = CGameInstance::GetInstance()->Add_GameObject(iFindPrototypeIndex, wstrFindPrototypeName, iAddLevelType, wstrAddLayerName,pArg)))
		return E_FAIL;

	return S_OK;
}

void CNPC_Base::Free()
{
	Super::Free();
}
