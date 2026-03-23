#include "pch.h"
#include "InteractiveObject.h"
#include "DataStruct_Map.h"
#include "QuestManager.h"
#include "DialogueManager.h"

#include "PhysicsCollider.h"
#include "PhysicsRigidBody.h"

#include "Model.h"
#include "Shader.h"

/* Interactive Object Header */

#include "WeaponPickUp.h"
#include "ChangeLevelObject.h"

/* -------------------------  */
#include "GameInstance.h"

CInteractiveObject::CInteractiveObject(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CInteractiveObject::CInteractiveObject(const CInteractiveObject& rhs)
	: CGameObject(rhs)
{
}


HRESULT CInteractiveObject::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CInteractiveObject::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	CInteractiveObject::INTERACTIVEOBJECT_DESC* pDesc{static_cast<CInteractiveObject::INTERACTIVEOBJECT_DESC*>(pArg)};


	if (FAILED(Ready_Component(pDesc)))
		return E_FAIL;

	if (FAILED(Ready_PhysicsCollider(pDesc)))
		return E_FAIL;

	if (FAILED(Ready_PhysicsRigidBody(pDesc)))
		return E_FAIL;

	if (FAILED(Ready_Quest(pDesc->vecQuestDesc)))
		return E_FAIL;

	// 상호작용
	{
		Set_Interact_Disable();
		Set_Interact_DefaultDisable();
	}

	return S_OK;
}

HRESULT CInteractiveObject::Ready_Component(INTERACTIVEOBJECT_DESC* pDesc)
{
	/* Model이 뒤죽박죽이라... 그냥 해당 Scene에서 생성하고 있는지없는지 판단해야할듯 */

	if (pDesc->wstrModelPath.empty()) return E_FAIL;

	/* Model 추가 */

	path pathModel = path(pDesc->wstrModelPath);
	wstring wstrModelName = { pathModel.filename().stem().wstring()};
	wstring wstrModelPrototypeTag = g_wszModel_Prototype_Tag + wstrModelName;

	CModel::MODEL_ORIGIN_DESC tOriginDesc{};
	tOriginDesc.eType = pDesc->eModelType;
	tOriginDesc.iPrototypeLevelIndex	= pDesc->iLevelIndex;
	tOriginDesc.wstrModelFolderName		= pathModel.wstring();	/* 부모 폴더 경로 넣어주기 */

	CGameObject* pResult{nullptr};
	if (FAILED(m_pGameInstance->Add_Prototype(pDesc->iLevelIndex, wstrModelPrototypeTag,CModel::Create(m_pDevice,m_pDeviceContext,&tOriginDesc))))
		return E_FAIL;

	m_pGameInstance->RegisterPhysicsMesh(tOriginDesc.iPrototypeLevelIndex, wstrModelPrototypeTag);
	CModel::MODEL_COPY_DESC tModelCopyDesc{};
	if (FAILED(Add_Component<CModel>(pDesc->iLevelIndex, wstrModelPrototypeTag, &tModelCopyDesc)))
		return E_FAIL;


	/* Shader 추가 */
	if (pDesc->eModelType == EModelType::ANIM || pDesc->eModelType == EModelType::BONE)
	{
		/* 나머지는 Animation 매쉬 */
		if (FAILED(Add_Component<CShader>(ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_Component_Shader_VtxAnimMesh", nullptr)))
			return E_FAIL;
	}
	else
	{
		/* 단순 매쉬 */
		if (FAILED(Add_Component<CShader>(ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_Component_Shader_VtxMesh", nullptr)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CInteractiveObject::Ready_Quest(const vector<DTO::QUEST_CHAPTERDESC>& vecQusetDesc)
{
	if (vecQusetDesc.empty())
	{
		/* Quest가 없으면 Defautl 상호작용 false */
		Set_Interact_DefaultDisable();
		Set_Interact_Disable();
		return S_OK;
	}

	/* 퀘스트 등록 */
	for (auto& Desc : vecQusetDesc)
	{
		CQuestManager::GetInstance()->Register_QuestObject(Desc,this);
	}


	Set_Interact_DefaultEnable();
	Set_Interact_Enable();

	return S_OK;
}

HRESULT CInteractiveObject::Ready_PhysicsCollider(INTERACTIVEOBJECT_DESC* pDesc)
{
	PHYSICSCOLLIDER_DESC pcDesc{};
	wstring wstrModelName = path(pDesc->wstrModelPath).filename().stem().wstring();

	pcDesc.wstrModelPrototypeTag = L"Prototype_Component_Model_" + wstrModelName;
	pcDesc.bIsConvex = false;

	CPhysicsCollider* pCollider = CPhysicsCollider::Create(m_pDevice, m_pDeviceContext, &pcDesc);
	if (pCollider)
	{
		if (FAILED(m_pGameInstance->Add_Prototype(pDesc->iLevelIndex, L"Prototype_Component_Physics_Collider_" + wstrModelName, pCollider)))
			Safe_Release(pCollider);
	}

	PHYSICSCOLLIDER_DESC cloneDesc{};

	cloneDesc.eFilterLayer = pDesc->flagFilterLayer;
	cloneDesc.iFilterMask = pDesc->flagFilterMaks;

	cloneDesc.bSetOnlyFilter = true;

	if (FAILED(Add_Component<CPhysicsCollider>(pDesc->iLevelIndex, L"Prototype_Component_Physics_Collider_" + wstrModelName, &cloneDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CInteractiveObject::Ready_PhysicsRigidBody(INTERACTIVEOBJECT_DESC* pDesc)
{
	PHYSICSRIGIDBODY_DESC desc{};
	desc.eType = EPhysicsActorType::STATIC;
	desc.detection = EPhysicsCollisionDetection::DISCRETE;
	desc.fDensity = 10.f;
	desc.bUseGravity = false;
	desc.bIsKinematic = false;
	desc.fLinearDamping = 0.f;
	desc.fAngularDamping = 0.f;
	Matrix WorldMaitrx = Get_Component<CTransform>()->Get_WorldMatrix();
	desc.pOwnerMatrices.push_back(WorldMaitrx);
	PHYSICS_SRT tSRT{};
	WorldMaitrx.Decompose(tSRT.vScale,tSRT.vQuat,tSRT.vPosition);
	desc.vecSRT.push_back(tSRT);


	if (FAILED(Add_Component<CPhysicsRigidBody>(0, L"Prototype_Component_Physics_RigidBody", &desc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CInteractiveObject::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;

	CPhysicsRigidBody* pRigidBody = Get_Component<CPhysicsRigidBody>();
	if (pRigidBody)
		pRigidBody->Awake();

	return S_OK;
}

void CInteractiveObject::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CInteractiveObject::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);

}

void CInteractiveObject::Update_Late(const _float fTimeelta)
{
	Super::Update_Late(fTimeelta);

}


void CInteractiveObject::Ready_Before_Render(const _float fTimeDelta)
{
	/* InteractiveObject는 특별하게 해준다 */
	m_pGameInstance->Push_RenderObject(RENDER_CATEGORY::COMPUTELIGHT_BLEND, this);
}

HRESULT CInteractiveObject::Render()
{
	if (FAILED(Super::Render()))
		return E_FAIL;

	return S_OK;
}

void CInteractiveObject::OnTrigger_Enter(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther, const COL_HIT_INFO& tHitInfo)
{
	if (Is_Interact_Enabled() && iOtherLayer == PHYSICSFILTERGROUP::DETECT_INTERACT)
	{
		if (Is_Interact_Enabled())
			m_pGameInstance->Broadcast<INTERACT_DETECT>(this);
	}
}

void CInteractiveObject::OnTrigger_Exit(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther)
{
	if (Is_Interact_Enabled() && iOtherLayer == PHYSICSFILTERGROUP::DETECT_INTERACT)
	{
		if (Is_Interact_Enabled())
			m_pGameInstance->Broadcast<INTERACT_LOST>(this);
	}
}

void CInteractiveObject::Free()
{
	Super::Free();

}

HRESULT CInteractiveObject::Create_InteractiveObject(const Engine::BATCH_INTERACTIVEOBJECT_DESC* pToolDesc, _uint iAddLevelIndex, const wstring& wstrModelPath, CTransform::TRANSFORM_DESC* pTsDesc)
{
	if (!pToolDesc) return E_FAIL;

	EObjectEnumTag::Enum eType = pToolDesc->eBatchInteractiveObejctType;

	CTransform::TRANSFORM_DESC tTransformDesc{ pTsDesc==nullptr ? CTransform::TRANSFORM_DESC() : *pTsDesc };

	CInteractiveObject::INTERACTIVEOBJECT_DESC tInteractiveDesc{};
	tInteractiveDesc.pTransform_Desc = &tTransformDesc;
	tInteractiveDesc.iLevelIndex = iAddLevelIndex;
	tInteractiveDesc.wstrModelPath = wstrModelPath;
	tInteractiveDesc.vecQuestDesc = pToolDesc->vecQuestDesc;


	void* pArg{nullptr};
	wstring wstrProtototypeTag{};

	switch (eType)
	{
	case Engine::EObjectEnumTag::OBJECT_INTERACT_WEAPONPICKUP:
	{
		wstrProtototypeTag = g_wszWeaponPickUp_Prototype_Tag;

		/* Weapon PickUp */
		static CWeaponPickUp::WEAPONPICKUP_DESC tWeaponDesc{};
		static_cast<CInteractiveObject::INTERACTIVEOBJECT_DESC&>(tWeaponDesc) = tInteractiveDesc;

		tWeaponDesc.eModelType = EModelType::STATIC;			/*  무기들은 단순 모델 Type */
		tWeaponDesc.strWeaponType = pToolDesc->strWeaponType;
		tWeaponDesc.isTutorialEvent = pToolDesc->isTutorialEvent;
		pArg = &tWeaponDesc;
	}
	break;
	case Engine::EObjectEnumTag::OBJECT_INTERACT_CHANGELEVEL:
	{
		wstrProtototypeTag = g_wszChangeLevelObject_Prototype_Tag;

		/* Level Change */
		static CChangeLevelObject::INTERACT_LEVELCHANGE_DESC tChangeLevelDesc{};
		static_cast<CInteractiveObject::INTERACTIVEOBJECT_DESC&>(tChangeLevelDesc) = tInteractiveDesc;

		tChangeLevelDesc.eModelType = EModelType::STATIC;
		tChangeLevelDesc.eChangeLevelType = StringToClientleveltype(pToolDesc->strChangeLevelTypeName);

		pArg = &tChangeLevelDesc;
	}
	break;
	default:	return E_FAIL;
	}

	CGameObject* pResult{nullptr};
	pResult = CGameInstance::GetInstance()->Add_GameObject(ENUM_TO_UINT(ELevelType::STATIC), wstrProtototypeTag, iAddLevelIndex , g_wszInteractiveObjectLayer ,pArg);
	if (pResult == nullptr)	return E_FAIL;

	return S_OK;
}



#pragma region Interact Interface 관련

void CInteractiveObject::Interact()
{
	if (Is_Quest_Enabled() && m_eQuestEvent == DTO::QUESTEVENT::OBJECT_INTERACT)
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
#pragma endregion

#pragma region Quest Interface 관련

void CInteractiveObject::QuestEnter()
{
	if (Is_Quest_Enabled() && m_eQuestEvent == DTO::QUESTEVENT::OBJECT_INTERACT)
	{
		auto chapterDesc = CQuestManager::GetInstance()->Get_QuestChapterInfo();

		auto iter = std::find(chapterDesc.eTargetType.begin(), chapterDesc.eTargetType.end(), m_eObject_Enum_Tag);

		if (iter != chapterDesc.eTargetType.end() && chapterDesc.tQuestDesc.iInteractDialogueId != -1)
		{
			CDialogueManager::GetInstance()->Start_Dialogue(chapterDesc.tQuestDesc.iInteractDialogueId);

			CallQuestEvent(m_eObject_Enum_Tag, 1);
		}
	}

	Set_Interact_Enable();
}

void CInteractiveObject::QuestExit()
{
	if (Is_Quest_Enabled() && m_eQuestEvent == DTO::QUESTEVENT::OBJECT_INTERACT)
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

#pragma endregion