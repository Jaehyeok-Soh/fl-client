#include "pch.h"
#include "AnimTool_Manager.h"
#include "GameInstance.h"
#include "Model.h"
#include "AnimObj.h"

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

	return S_OK;
}

void CAnimTool_Manager::Update(const _float fTimeDelta)
{
	if (m_tAnimControllInfo.bPlay)
		Update_Animation(fTimeDelta);

	UpdateAnimationInfo();
}

void CAnimTool_Manager::SetAnimationObject(CAnimObj* pObject)
{
	if (!pObject)
		return;

	m_tAnimControllInfo.pCurrentObject = pObject;

	m_tAnimControllInfo.pModel = m_tAnimControllInfo.pCurrentObject->Get_Component<CModel>();
	m_tAnimControllInfo.iCurrentAnimationState = m_tAnimControllInfo.pModel->Get_AnimPlayState();
	m_tAnimControllInfo.vecBones = m_tAnimControllInfo.pModel->Get_Bones();
	m_tAnimControllInfo.vecAnimations = m_tAnimControllInfo.pModel->Get_Animations();

	SetAnimControllInfo();
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

void CAnimTool_Manager::Update_Animation(const _float fTimeDelta)
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

void CAnimTool_Manager::UpdateAnimationInfo()
{
	if (!ValidCheck())
		return;

	m_tAnimControllInfo.fTrackPosition = m_tAnimControllInfo.pModel->Get_AnimTrackPosition();
	m_tAnimControllInfo.fTickPerSecond = m_tAnimControllInfo.pModel->Get_AnimTickPerSecond();
}

void CAnimTool_Manager::Free()
{
	Super::Free();

	Safe_Release(m_pDevice);
	Safe_Release(m_pDeviceContext);

	Safe_Release(m_pGameInstance);
}
