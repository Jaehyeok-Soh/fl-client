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

// Animation tool module
#include "Event_Overlap_Module.h"

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

	m_pOverlapModule = CEvent_Overlap_Module::Create(m_pDevice, m_pDeviceContext);

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
	m_tAnimControllInfo.fDuration = m_tAnimControllInfo.pModel->Get_AnimDurationTime();
	m_tAnimControllInfo.fTrackPosition = 0.f;
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

	m_tAnimControllInfo.pModel->Update_Animation(pBonCS,
		pAnimECS,
		fTimeDelta,
		m_tAnimControllInfo.pCurrentObject->Get_Component<CTransform>(),
		m_tAnimControllInfo.pCurrentObject->Get_Component<CPhysicsCCT>(),
		pAnimBCS,
		nullptr);

	if (m_tAnimControllInfo.pModel->Is_AnimFinished())
		m_tAnimControllInfo.pCurrentObject->Get_Component<CTransform>()->Set_Info(TRANSFORM_INFO_STATE::POS, Vec3(0.f, 0.f, 0.f));
}

void CAnimTool_Manager::ChangeAnimation(_uint iIndex)
{
	if (!ValidCheck())
		return;

	CComputeShader* pAnimECS = static_cast<CComputeShader*>(m_tAnimControllInfo.pCurrentObject->Get_Script_Component(TEXT("ComputeShader_AnimE")));

	m_tAnimControllInfo.pModel->Change_Animation(pAnimECS, m_tAnimControllInfo.iCurrentAnimIndex, true, m_tAnimControllInfo.bLoop, false);

	m_tAnimControllInfo.fDuration = m_tAnimControllInfo.pModel->Get_AnimDurationTime();
	m_tAnimControllInfo.fTickPerSecond = m_tAnimControllInfo.pModel->Get_AnimTickPerSecond();
	m_tAnimControllInfo.fPlayRate = 1.f;
}

void CAnimTool_Manager::Set_RootBone(_int iBoneIdx)
{
	m_tAnimControllInfo.pModel->Set_RootBone(iBoneIdx);
}

void CAnimTool_Manager::UpdateAnimationInfo()
{
	if (!ValidCheck())
		return;

	m_tAnimControllInfo.fTrackPosition = m_tAnimControllInfo.pModel->Get_AnimTrackPosition();
	m_tAnimControllInfo.fTickPerSecond = m_tAnimControllInfo.pModel->Get_AnimTickPerSecond();
}

void CAnimTool_Manager::Update_Module(const _float& fTimeDelta)
{
	if (!ValidCheck())
		return;

	// 충돌체
	m_pOverlapModule->Update(fTimeDelta);

	// 이펙트

	// 사운드
}

void CAnimTool_Manager::Render_Module()
{
	if (!ValidCheck())
		return;

	// 충돌체
	m_pOverlapModule->Render();

	// 이펙트

	// 사운드
}

void CAnimTool_Manager::SetModuleOwner()
{
	if (!ValidCheck())
		return;

	// 충돌체
	m_pOverlapModule->SetOwner(m_tAnimControllInfo.pCurrentObject);

	// 이펙트

	// 사운드
}

HRESULT CAnimTool_Manager::Ready_Builder()
{
	m_pBuilderSystem = CBuilderSystem::Create();
	if (m_pBuilderSystem == nullptr)  return E_FAIL;

	if (FAILED(m_pBuilderSystem->Ready_Builder(DTO::ECategory::OVERLAP_SCRIPT, CBuilder_AttackOverlap::Create(m_pDevice, m_pDeviceContext, ENUM_TO_UINT(ELevelType::ANIMATION)))))
		return E_FAIL;

	return S_OK;
}

HRESULT CAnimTool_Manager::Ready_Event()
{
	m_EventHandles[CLevel_Animation::Event::LOAD_OVERLAP_SCRIPT] =
		m_pGameInstance->Subscribe<LoadAttackOverlap>(this, &CAnimTool_Manager::Set_AttackOverlap);

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

void CAnimTool_Manager::Set_AttackOverlap(CPhysicsAttackOverlap* pAttackOverlap)
{
	m_pOverlapModule->SetAttackOverlap(pAttackOverlap, m_tAnimControllInfo.pCurrentObject);
	
	if (m_pOverlapModule)
		m_tEventInfo.vecAttackEvents = m_pOverlapModule->GetEvents();
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
}

HRESULT CAnimTool_Manager::Release_Event()
{
	m_pGameInstance->Unsubscribe<LoadAttackOverlap>(m_EventHandles[CLevel_Animation::Event::LOAD_OVERLAP_SCRIPT]);

	return S_OK;
}

void CAnimTool_Manager::Awake_AttackOverlap()
{
	m_pOverlapModule->Awake();
}

void CAnimTool_Manager::Modify_AttackOverlap(_uint eventIdx, DTO::ATTACKEVENT event)
{
	m_pOverlapModule->Modify_AttackOverlap(eventIdx, event);
}

void CAnimTool_Manager::Modify_AttackOverlap(vector<DTO::ATTACKEVENT> events)
{
	m_pOverlapModule->Modify_AttackOverlap(events);
}

void CAnimTool_Manager::Free()
{
	Safe_Release(m_pOverlapModule);

	Release_Event();

	Safe_Release(m_pBuilderSystem);

	Safe_Release(m_pDevice);
	Safe_Release(m_pDeviceContext);
	
	Super::Free();

	Safe_Release(m_pGameInstance);
}
