#include "pch.h"
#include "AnimTool_Manager.h"
#include "GameInstance.h"
#include "Model.h"
#include "AnimObj.h"

// BuilderSystem
#include "BuilderSystem.h"

// Attack overlap
#include "Builder_AttackOverlap.h"
#include "DataDocument_AttackOverlap.h"
#include "DataStruct_AttackOverlap.h"

// EFfect Event
#include "Builder_EffectEvent.h"
#include "DataDocument_EffectEvent.h"
#include "DataStruct_EffectEvent.h"
#include "EffectHandler.h"

// Effect
#include "Builder_Effect.h"
#include "DataDocument_Effect.h"
#include "DataStruct_Effect.h"

// Animation tool module
#include "Event_Overlap_Module.h"
#include "Event_Effect_Module.h"
#include "Event_Sound_Module.h"

IMPLEMENT_SINGLETON(CAnimTool_Manager)

CAnimTool_Manager::CAnimTool_Manager()
{
}

HRESULT CAnimTool_Manager::Initialize_AnimTool(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	m_pDevice = pDevice;
	m_pDeviceContext = pDeviceContext;
	m_pGameInstance = CGameInstance::GetInstance();

	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);
	Safe_AddRef(m_pGameInstance);

	Ready_Builder();
	Ready_BuildFiles();

	m_pOverlapModule = CEvent_Overlap_Module::Create(m_pDevice, m_pDeviceContext);
	m_pEffectModule = CEvent_Effect_Module::Create(m_pDevice, m_pDeviceContext);
	m_pSoundModule = CEvent_Sound_Module::Create();

	Ready_Event();

	return S_OK;
}

void CAnimTool_Manager::Update(const _float& fTimeDelta)
{
	if (m_tAnimControllInfo.bPlay)
		Update_Animation(fTimeDelta);

	Update_Module(fTimeDelta);

	UpdateAnimationInfo();
}

void CAnimTool_Manager::Render()
{
	Render_Module();
}

_int CAnimTool_Manager::Render_ConfirmModal(string strModalId, string message)
{
	_int result = { -1 };

	ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

	if (ImGui::BeginPopupModal(strModalId.c_str(), NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f), message.c_str());

		ImGui::NewLine();
		ImGui::Separator();

		_float buttonsWidth = 50.f;

		if (ImGui::Button("OK", ImVec2(50, 0)))
		{
			result = 1;
			ImGui::CloseCurrentPopup();
		}

		ImGui::SameLine();

		_float windowWidth = ImGui::GetWindowContentRegionMax().x;
		
		ImGui::SetCursorPosX(windowWidth - buttonsWidth);

		if (ImGui::Button("Cancel", ImVec2(buttonsWidth, 0)))
		{
			result = -1;
			ImGui::CloseCurrentPopup(); // 그냥 닫기
		}

		ImGui::EndPopup();
	}

	return result;
}

void CAnimTool_Manager::Open_ConfirmModal(string strModalId)
{
	ImGui::OpenPopup(strModalId.c_str());
}

void CAnimTool_Manager::SetAnimationObject(CAnimObj* pObject, fs::path animModelPath)
{
	if (!pObject)
		return;

	m_tAnimControllInfo.modelPath = animModelPath;

	m_tAnimControllInfo.pCurrentObject = pObject;

	m_tAnimControllInfo.pModel = m_tAnimControllInfo.pCurrentObject->Get_Component<CModel>();
	m_tAnimControllInfo.iCurrentAnimationState = m_tAnimControllInfo.pModel->Get_AnimPlayState();
	m_tAnimControllInfo.vecBones = m_tAnimControllInfo.pModel->Get_Bones();
	m_tAnimControllInfo.vecAnimations = m_tAnimControllInfo.pModel->Get_Animations();

	SetAnimControllInfo();

	SetModuleOwner();
}

void CAnimTool_Manager::SetAnimControllInfo()
{
	if (!ValidCheck())
		return;

	m_tAnimControllInfo.iTotalAnimCount = m_tAnimControllInfo.pModel->Get_AnimationCount();
	m_tAnimControllInfo.iCurrentAnimIndex = m_tAnimControllInfo.pModel->Get_CurrentAnimationIndex();
	m_tAnimControllInfo.fDuration = (_uint)m_tAnimControllInfo.pModel->Get_AnimDurationTime();
	m_tAnimControllInfo.fTrackPosition = 0;
	m_tAnimControllInfo.fTickPerSecond = m_tAnimControllInfo.pModel->Get_AnimTickPerSecond();
	m_tAnimControllInfo.pModel->Set_AnimationPlayRate(m_tAnimControllInfo.iCurrentAnimIndex, 1.f);
	m_tAnimControllInfo.fPlayRate = 1.f;
	m_tAnimControllInfo.pModel->Set_LoopState(m_tAnimControllInfo.bLoop);

	m_tAnimControllInfo.vecBones.clear();
	m_tAnimControllInfo.vecBones = m_tAnimControllInfo.pModel->Get_Bones();

	m_tAnimControllInfo.vecAnimations.clear();
	m_tAnimControllInfo.vecAnimations = m_tAnimControllInfo.pModel->Get_Animations();

	SetAnimationInfo();
	SetBoneInfo();
}

void CAnimTool_Manager::SetAnimationInfo()
{
	if (!ValidCheck())
		return;

	m_tAnimControllInfo.vecAnimInfo.clear();

	for (auto& anim : m_tAnimControllInfo.vecAnimations)
	{

		ANIMINFO info{};
		info.wstrAnimName = wstring(anim->Get_Name());
		info.strAnimName = Engine_Utils::ToString(info.wstrAnimName);
		info.iIndex = m_tAnimControllInfo.pModel->Get_AnimationIndex(info.wstrAnimName);
		info.pModelAnimation = anim;
		info.fDuration = anim->Get_DurationTime();

		m_tAnimControllInfo.vecAnimInfo.push_back(info);
	}
}

void CAnimTool_Manager::SetBoneInfo()
{
	if (!ValidCheck())
		return;

	m_tAnimControllInfo.vecBoneInfo.clear();

	for (auto& bone : m_tAnimControllInfo.vecBones)
	{
		BONEINFO info{};
		info.iIndex = bone->Get_Index();
		info.iParentIndex = bone->Get_ParentIndex();
		info.strBoneName = bone->Get_Name();
		info.wstrBoneName = Engine_Utils::ToWString(info.strBoneName);
		info.pBone = bone;
		info.matTransform = bone->Get_Transform();

		m_tAnimControllInfo.vecBoneInfo.push_back(info);
	}
}

_bool CAnimTool_Manager::ValidCheck()
{
	return m_tAnimControllInfo.pCurrentObject != nullptr;
}

void CAnimTool_Manager::Update_Animation(const _float& fTimeDelta)
{
	if (!ValidCheck())
		return;

	CComputeShader* pBonCS = static_cast<CComputeShader*>(m_tAnimControllInfo.pCurrentObject->Get_Script_Component(TEXT("ComputeShader_BoneCombine")));
	CComputeShader* pAnimECS = static_cast<CComputeShader*>(m_tAnimControllInfo.pCurrentObject->Get_Script_Component(TEXT("ComputeShader_AnimE")));
	CComputeShader* pAnimBCS = static_cast<CComputeShader*>(m_tAnimControllInfo.pCurrentObject->Get_Script_Component(TEXT("ComputeShader_AnimB")));
	CComputeShader* pAnimMixCS = static_cast<CComputeShader*>(m_tAnimControllInfo.pCurrentObject->Get_Script_Component(TEXT("ComputeShader_AnimMix")));
	CComputeShader* pAnimAdditiveCS = static_cast<CComputeShader*>(m_tAnimControllInfo.pCurrentObject->Get_Script_Component(TEXT("ComputeShader_AnimAdditiveMix")));

	m_tAnimControllInfo.pModel->Update_Animation(pBonCS,
		pAnimECS,
		fTimeDelta,
		m_tAnimControllInfo.pCurrentObject->Get_Component<CTransform>(),
		m_tAnimControllInfo.pCurrentObject->Get_Component<CPhysicsCCT>(),
		pAnimBCS,
		pAnimMixCS,
		pAnimAdditiveCS);

	m_tAnimControllInfo.pModel->Emit_Notifies(EAnimNotifyPhase::Late);
	m_tAnimControllInfo.pModel->Emit_Notifies(EAnimNotifyPhase::PreRender);

	if (m_tAnimControllInfo.pModel->Is_AnimFinished() || m_tAnimControllInfo.pModel->Is_LoopAnimDone())
	{
		m_tAnimControllInfo.pCurrentObject->Get_Component<CPhysicsCCT>()->SetFootPosition(Vec3(0.f, 0.f, 0.f));
		m_tAnimControllInfo.pCurrentObject->Get_Component<CTransform>()->Set_Info(TRANSFORM_INFO_STATE::POS, Vec3(0.f, 0.f, 0.f));
	}
}

void CAnimTool_Manager::ChangeAnimation(_uint iIndex)
{
	if (!ValidCheck())
		return;

	CComputeShader* pAnimECS = static_cast<CComputeShader*>(m_tAnimControllInfo.pCurrentObject->Get_Script_Component(TEXT("ComputeShader_AnimE")));

	m_tAnimControllInfo.pModel->Change_Animation(pAnimECS, m_tAnimControllInfo.iCurrentAnimIndex, true, m_tAnimControllInfo.bLoop, false);

	m_tAnimControllInfo.fDuration = (_uint)m_tAnimControllInfo.pModel->Get_AnimDurationTime();
	m_tAnimControllInfo.fTickPerSecond = m_tAnimControllInfo.pModel->Get_AnimTickPerSecond();
	m_tAnimControllInfo.fPlayRate = 1.f;

	// 위치 0,0,0으로 맞춤
	{
		m_tAnimControllInfo.pCurrentObject->Get_Component<CPhysicsCCT>()->SetFootPosition(Vec3(0.f, 0.f, 0.f));
		m_tAnimControllInfo.pCurrentObject->Get_Component<CTransform>()->Set_Info(TRANSFORM_INFO_STATE::POS, Vec3(0.f, 0.f, 0.f));
	}
}

void CAnimTool_Manager::Set_RootBone(_int iBoneIdx)
{
	m_tAnimControllInfo.pModel->Set_RootBone(iBoneIdx);
	m_tAnimControllInfo.pModel->Set_ApplyRootMotionAll(true);
}

void CAnimTool_Manager::Set_RootOffset(_uint iAnimIdx, _float fOffset)
{
	m_tAnimControllInfo.pModel->Set_Animtion_MotionOffset(iAnimIdx, fOffset);
}

void CAnimTool_Manager::UpdateAnimationInfo()
{
	if (!ValidCheck())
		return;

	m_tAnimControllInfo.fTrackPosition = (_uint)m_tAnimControllInfo.pModel->Get_AnimTrackPosition();
	m_tAnimControllInfo.fTickPerSecond = m_tAnimControllInfo.pModel->Get_AnimTickPerSecond();
}

void CAnimTool_Manager::Update_Module(const _float& fTimeDelta)
{
	if (!ValidCheck())
		return;

	// 충돌체
	m_pOverlapModule->Update(fTimeDelta);

	// 이펙트
	if (m_pEffectModule)
		m_pEffectModule->Update(fTimeDelta);

	// 사운드
}

void CAnimTool_Manager::Render_Module()
{
	if (!ValidCheck())
		return;

	// 충돌체
	m_pOverlapModule->Render();

	// 이펙트
	if (m_pEffectModule)
		m_pEffectModule->Render();

	// 사운드
}

void CAnimTool_Manager::SetModuleOwner()
{
	if (!ValidCheck())
		return;

	// 충돌체
	m_pOverlapModule->SetOwner(m_tAnimControllInfo.pCurrentObject);

	// 이펙트
	 m_pEffectModule->SetOwner(m_tAnimControllInfo.pCurrentObject);

	// 사운드
	 m_pSoundModule->Set_Owner(m_tAnimControllInfo.pCurrentObject);
}

HRESULT CAnimTool_Manager::Ready_Builder()
{
	m_pBuilderSystem = CBuilderSystem::Create();
	if (m_pBuilderSystem == nullptr)  return E_FAIL;

	if (FAILED(m_pBuilderSystem->Ready_Builder(DTO::ECategory::OVERLAP_SCRIPT, CBuilder_AttackOverlap::Create(m_pDevice, m_pDeviceContext, ENUM_TO_UINT(ELevelType::ANIMATION)))))
		return E_FAIL;

	if (FAILED(m_pBuilderSystem->Ready_Builder(DTO::ECategory::EFFECTEVENT, CBuilder_EffectEvent::Create(m_pDevice, m_pDeviceContext, ENUM_TO_UINT(ELevelType::ANIMATION)))))
		return E_FAIL;

	if (FAILED(m_pBuilderSystem->Ready_Builder(DTO::ECategory::EFFECT, CBuilder_Effect::Create(m_pDevice, m_pDeviceContext, ENUM_TO_UINT(ELevelType::ANIMATION)))))
		return E_FAIL;

	return S_OK;
}

HRESULT CAnimTool_Manager::Ready_BuildFiles()
{
#pragma region EFFECT
	DTO::ECategory eCategory = DTO::ECategory::EFFECT;
	if (FAILED(m_pGameInstance->Regist_Document<CDataDocument_Effect>((_uint)ELevelType::ANIMATION, eCategory)))
		return E_FAIL;

	std::filesystem::path strEffectFolderPath = L"../../Resources/Data/EffectData/";

	if (std::filesystem::exists(strEffectFolderPath))
	{
		for (const auto& entry : std::filesystem::recursive_directory_iterator(strEffectFolderPath))
		{
			if (std::filesystem::is_regular_file(entry.path()))
			{
				// 확장자가 .json인 것만 골라내기
				if (entry.path().extension() == ".json")
				{
					if (FAILED(m_pGameInstance->Load_File_Json((_uint)ELevelType::ANIMATION, eCategory, entry.path())))
						return E_FAIL;

					if (FAILED(Build_File((_uint)ELevelType::ANIMATION, eCategory, entry.path().stem().string())))
						return E_FAIL;

					m_vecEffectTags.push_back(entry.path().stem().string());
				}
			}
		}
	}
#pragma endregion

	return S_OK;
}

HRESULT CAnimTool_Manager::Build_File(_uint iLevelID, DTO::ECategory eCateGory, string strFileKey)
{
	return m_pBuilderSystem->Build_File(iLevelID, eCateGory, strFileKey);
}

HRESULT CAnimTool_Manager::Ready_Event()
{
	m_EventHandles[CLevel_Animation::Event::LOAD_OVERLAP_SCRIPT] =
		m_pGameInstance->Subscribe<LoadAttackOverlap>(this, &CAnimTool_Manager::Set_AttackOverlap);

	m_EventHandles[CLevel_Animation::Event::LOAD_EFFECT_SCRIPT] =
		m_pGameInstance->Subscribe<LoadEffectEvent>(this, &CAnimTool_Manager::Set_EffectEvent);

	return S_OK;
}

HRESULT CAnimTool_Manager::Load_AttackOverlap(fs::path path)
{
	ELevelType eLevelType = ELevelType::ANIMATION;
	DTO::ECategory eCategory = DTO::ECategory::OVERLAP_SCRIPT;
	_uint iLevelID = ENUM_TO_UINT(eLevelType);

	if (FAILED(m_pGameInstance->Regist_Document<CDataDocument_AttackOverlap>(iLevelID, eCategory)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Load_File_Json(iLevelID, eCategory, path)))
		return E_FAIL;

	m_pBuilderSystem->Build_File(ENUM_TO_UINT(ELevelType::ANIMATION), DTO::ECategory::OVERLAP_SCRIPT, path.filename().stem().string());

	return S_OK;
}

HRESULT CAnimTool_Manager::Load_EffectEvent(fs::path path)
{
	ELevelType eLevelType = ELevelType::ANIMATION;
	DTO::ECategory eCategory = DTO::ECategory::EFFECTEVENT;
	_uint iLevelID = ENUM_TO_UINT(eLevelType);

	if (FAILED(m_pGameInstance->Regist_Document<CDataDocument_EffectEvent>(iLevelID, eCategory)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Load_File_Json(iLevelID, eCategory, path)))
		return E_FAIL;

	m_pBuilderSystem->Build_File(ENUM_TO_UINT(ELevelType::ANIMATION), DTO::ECategory::EFFECTEVENT, path.filename().stem().string());

	return S_OK;
}

void CAnimTool_Manager::Set_AttackOverlap(CPhysicsAttackOverlap* pAttackOverlap)
{
	m_pOverlapModule->SetAttackOverlap(pAttackOverlap, m_tAnimControllInfo.pCurrentObject);
	
	if (m_pOverlapModule)
		m_tEventInfo.vecAttackEvents = m_pOverlapModule->GetEvents();
}

void CAnimTool_Manager::Set_EffectEvent(CEffectHandler* pEffectEvent)
{
	if (m_pEffectModule)
	{
		m_pEffectModule->SetEFfectEvent(pEffectEvent, m_tAnimControllInfo.pCurrentObject);

		unordered_map<string, vector<DTO::EFFECTEVENT>> EventList = {};
		EventList = m_pEffectModule->GetEvents();

		for (auto& Event : EventList)
		{
			for (auto& E : Event.second)
			{
				m_tEventInfo.vecVFXEvents.push_back(E);
			}
		}
		//m_tEventInfo.vecVFXEvents = m_pEffectModule->GetEvents();
		int count = (int)m_tEventInfo.vecVFXEvents.size();
	}
}

HRESULT CAnimTool_Manager::Save_AttackOverlap(fs::path path, string strAnimTag, _int iPool)
{
	ELevelType eLevelType = ELevelType::ANIMATION;
	DTO::ECategory eCategory = DTO::ECategory::OVERLAP_SCRIPT;
	_uint iLevelID = ENUM_TO_UINT(eLevelType);

	if (eCategory != DTO::ECategory::OVERLAP_SCRIPT)
		return E_FAIL;

	if (FAILED(m_pGameInstance->Regist_Document<CDataDocument_AttackOverlap>(iLevelID, eCategory)))
		return E_FAIL;

	CDataDocumentBase* pDocument = m_pGameInstance->Ensure_Document(iLevelID, eCategory, path);
	if (pDocument == nullptr)
		return E_FAIL;

	CDataDocument_AttackOverlap* pAttackOverlapDoc = static_cast<CDataDocument_AttackOverlap*>(pDocument);

	if (pAttackOverlapDoc == nullptr)
		return E_FAIL;

	for (auto& event : m_tEventInfo.vecAttackEvents)
		event.strAnimTag = Engine_Utils::ToString(m_tAnimControllInfo.pModel->Get_AnimationName(event.iAnimIndex));

	DTO::ATTACKOVERLAP_DESC tData{};
	tData.strTag = m_tAnimControllInfo.modelPath.stem().string();
	tData.iNumPool = iPool;
	tData.attackEvents = m_tEventInfo.vecAttackEvents;

	if (FAILED(pAttackOverlapDoc->Try_Add(tData)))
		return E_FAIL;

	m_pGameInstance->Save_File_Json(iLevelID, DTO::ECategory::OVERLAP_SCRIPT, path);
	return S_OK;
}

HRESULT CAnimTool_Manager::Save_EffectEvent(fs::path path, string strAnimTag, _int iPool)
{
	ELevelType eLevelType = ELevelType::ANIMATION;
	DTO::ECategory eCategory = DTO::ECategory::EFFECTEVENT;
	_uint iLevelID = ENUM_TO_UINT(eLevelType);

	if (FAILED(m_pGameInstance->Regist_Document<CDataDocument_EffectEvent>(iLevelID, eCategory)))
		return E_FAIL;

	CDataDocumentBase* pDocument = m_pGameInstance->Ensure_Document(iLevelID, eCategory, path);
	CDataDocument_EffectEvent* pEffectEventDoc = static_cast<CDataDocument_EffectEvent*>(pDocument);

	if (pEffectEventDoc == nullptr) return E_FAIL;

	DTO::EFFECT_EVENT_INFO_DESC tData{};
	tData.strOwnerTag = m_tAnimControllInfo.modelPath.stem().string();
	tData.iNumPool = iPool;

	for (auto& event : m_tEventInfo.vecVFXEvents)
	{
		event.strAnimTag = Engine_Utils::ToString(m_tAnimControllInfo.pModel->Get_AnimationName(event.iAnimIndex));
	}
	tData.vecEffectEvents = m_tEventInfo.vecVFXEvents;

	if (FAILED(pEffectEventDoc->Try_Add(tData)))
		return E_FAIL;

	m_pGameInstance->Save_File_Json(iLevelID, eCategory, path);
	return S_OK;
}

HRESULT CAnimTool_Manager::Release_Event()
{
	m_pGameInstance->Unsubscribe<LoadAttackOverlap>(m_EventHandles[CLevel_Animation::Event::LOAD_OVERLAP_SCRIPT]);
	m_pGameInstance->Unsubscribe<LoadEffectEvent>(m_EventHandles[CLevel_Animation::Event::LOAD_EFFECT_SCRIPT]);

	return S_OK;
}

void CAnimTool_Manager::Awake_AttackOverlap()
{
	m_pOverlapModule->Awake();
	m_pEffectModule->Awake();
}

void CAnimTool_Manager::Modify_AttackOverlap(_uint eventIdx, DTO::ATTACKEVENT event)
{
	m_pOverlapModule->Modify_AttackOverlap(eventIdx, event);
	m_pOverlapModule->SetOwner(m_tAnimControllInfo.pCurrentObject);
}

void CAnimTool_Manager::Modify_AttackOverlap(vector<DTO::ATTACKEVENT> events)
{
	m_pOverlapModule->Modify_AttackOverlap(events);
	m_pOverlapModule->SetOwner(m_tAnimControllInfo.pCurrentObject);
}

void CAnimTool_Manager::Modify_EffectEvent(vector<DTO::EFFECTEVENT> events)
{
	m_tEventInfo.vecVFXEvents = events;

	if (m_pEffectModule)
	{
		// Add Event를 눌러서 생성했는데 컴포넌트가 없는 상태라면
		if (m_pEffectModule->GetHandler() == nullptr)
		{
			CEffectHandler::ANIM_EFFECT_HANDLER_DESC tDesc{};
			tDesc.strOwnerTag = m_tAnimControllInfo.modelPath.stem().string();

			CEffectHandler* pNewHandler = CEffectHandler::Create(&tDesc);
			if (pNewHandler)
			{
				m_pEffectModule->SetEFfectEvent(pNewHandler, m_tAnimControllInfo.pCurrentObject);
				//Safe_Release(pNewHandler);
			}
		}
	}
	m_pEffectModule->Modify_EFfectEvent(events);
}

void CAnimTool_Manager::Modify_SoundEvent(vector<DTO::SOUNDEVENT> events)
{
	m_tEventInfo.vecSoundEvents = std::move(events);

	if (!ValidCheck())
		return;

	m_pSoundModule->Set_Owner(m_tAnimControllInfo.pCurrentObject);
	m_pSoundModule->Rebuild(events);
}

HRESULT CAnimTool_Manager::EffectEvent_GizmoObjectSetting()
{


	return E_NOTIMPL;
}


void CAnimTool_Manager::Free()
{
	Safe_Release(m_pOverlapModule);
	Safe_Release(m_pEffectModule);
	Safe_Release(m_pSoundModule);

	Release_Event();

	Safe_Release(m_pBuilderSystem);

	Safe_Release(m_pDevice);
	Safe_Release(m_pDeviceContext);
	
	Super::Free();

	Safe_Release(m_pGameInstance);
}
