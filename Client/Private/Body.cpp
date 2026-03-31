#include "pch.h"
#include "Client_Defines.h"
#include "Body.h"

#include "Player.h"

#include "Bone.h"

// components
#include "ActionState.h"
#include "Shader.h"
#include "Collider.h"
#include "Model.h"
#include "PhysicsCCT.h"
#include "EffectHandler.h"
#include "ComputeShader.h"
#include "RenderFx.h"

#include "CameraAnchorResolver.h"
#include "GameInstance.h"
#include "Boss_Lianhuo_Body.h"

CBody::CBody(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: Super(pDevice, pDeviceContext)
{
}

CBody::CBody(const CBody& rhs)
	: Super(rhs)
{
}

HRESULT CBody::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBody::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	Set_Name("Eun_bi_Parts");

	BODY_DESC* pDesc = static_cast<BODY_DESC*>(pArg);
	if (FAILED(Ready_Components(pDesc)))
		return E_FAIL;

	if (FAILED(Ready_ComputeShader()))
		return E_FAIL;

	m_iHead_Index = Get_Component<CModel>()->Get_BoneIndex("head");
	m_iNeck_Index = Get_Component<CModel>()->Get_BoneIndex("neck_01");
	m_iSpine1_Index = Get_Component<CModel>()->Get_BoneIndex("spine_01");
	m_iCamPos_Index = Get_Component<CModel>()->Get_BoneIndex("camera_point");
	m_iCamSocket_Index = Get_Component<CModel>()->Get_BoneIndex("camera_socket");

	// model 초기값
	{
		CModel* pMyModel = Get_Component<CModel>();

		// 전반적인 초기 셋팅
		{
			pMyModel->Set_Animation_SpeedOffset_All(1.2f);
		}

		// face mix 셋팅
		{
			vector<CModel::DATA_ANIMIX> vecMix = { {113,false,1.f} }; // mixing 정보
			_uint iFaceAnimIdx = pMyModel->Get_AnimationIndex(L"Animation_PlayerMoon_Face_Angry");

			pMyModel->Set_MixAnim_ResetSize(0); 								// mix 값 초기화
			pMyModel->Set_MixAnim(true);										// mix를 키고
			pMyModel->Set_MixAnim_ResetSize(2);									// mix size 설정 (0 : face , 1 : Lower body)
			pMyModel->Make_MixRatio(iFaceAnimIdx, vecMix, m_pBoneAnimMixCS);	// mixing 정보 할당
			pMyModel->Set_MixAnim_AnimIndex(0, iFaceAnimIdx);					// model에 mix 애니메이션 설정
		}

		// additive mix 셋팅
		{
			vector<CModel::DATA_ANIMIX> vecAnimMix = { {4,true,1.f},{414,true,1.f} }; // mixing 정보
			_uint iAnimMiddle = pMyModel->Get_AnimationIndex(L"Animation_PlayerMoon_Shotgun_Aim_MM");

			pMyModel->Set_AdditiveRef_AnimIdx(99);									// middle aim을 ref 애니메이션으로 잡는다
			pMyModel->Make_MixRatio(iAnimMiddle - 1, vecAnimMix, m_pAdditiveMixCS);	// aim down
			pMyModel->Make_MixRatio(iAnimMiddle, vecAnimMix, m_pAdditiveMixCS);		// aim middle
			pMyModel->Make_MixRatio(iAnimMiddle + 1, vecAnimMix, m_pAdditiveMixCS); // aim up
		}

		// bone move 셋팅
		{
			CS_CB_MU_BONEMOVE tCB = {};
			tCB.iMovingIdx = 85;

			pMyModel->Set_MoveBoneCS(tCB); // aim up
		}

		// motion offset
		{
			pMyModel->Set_Animtion_MotionOffset(Get_AnimationIndex(L"Animation_PlayerMoon_Run_Loop_Acc"), 0.6f);
			pMyModel->Set_Animtion_MotionOffset(Get_AnimationIndex(L"Animation_PlayerMoon_Dodge_InAir"), 1.5f);
			pMyModel->Set_Animtion_MotionOffset(Get_AnimationIndex(L"Animation_PlayerMoon_Dodge_To_Run"), 1.2f);
			pMyModel->Set_Animtion_MotionOffset(Get_AnimationIndex(L"Animation_PlayerMoon_Dualblade_RunAttack_01"), 1.7f);
			pMyModel->Set_Animtion_MotionOffset(Get_AnimationIndex(L"Animation_PlayerMoon_Dualblade_RunAttack_02"), 1.8f);
			pMyModel->Set_Animtion_MotionOffset(Get_AnimationIndex(L"Animation_PlayerMoon_FirstJump_InplaceStart"), 2.5f);
			pMyModel->Set_Animtion_MotionOffset(Get_AnimationIndex(L"Animation_PlayerMoon_BulletJump_Start"), 2.f);
		}


		// animation speed 조절
		{
			pMyModel->Get_Animation(Get_AnimationIndex(L"Animation_PlayerMoon_Crouch_Loop"))->Set_AnimationSpeed(1.1f);

			pMyModel->Get_Animation(Get_AnimationIndex(L"Animation_PlayerMoon_Dualblade_RunAttack_01"))->Set_AnimationSpeed(1.9f);
			pMyModel->Get_Animation(Get_AnimationIndex(L"Animation_PlayerMoon_Dualblade_RunAttack_02"))->Set_AnimationSpeed(1.3f);
			pMyModel->Get_Animation(Get_AnimationIndex(L"Animation_PlayerMoon_Dualblade_RunAttack_03"))->Set_AnimationSpeed(1.3f);
			pMyModel->Get_Animation(Get_AnimationIndex(L"Animation_PlayerMoon_Dualblade_RunAttack_04"))->Set_AnimationSpeed(1.3f);
			pMyModel->Get_Animation(Get_AnimationIndex(L"Animation_PlayerMoon_Dualblade_SlideAttack"))->Set_AnimationSpeed(1.5f);
			pMyModel->Get_Animation(Get_AnimationIndex(L"Animation_PlayerMoon_Dualblade_FallAttack_End"))->Set_AnimationSpeed(1.3f);
			pMyModel->Get_Animation(Get_AnimationIndex(L"Animation_PlayerMoon_Dualblade_HeavyAttack_Start"))->Set_AnimationSpeed(1.3f);
			pMyModel->Get_Animation(Get_AnimationIndex(L"Animation_PlayerMoon_Dualblade_HeavyAttack_End"))->Set_AnimationSpeed(1.3f);

			pMyModel->Get_Animation(Get_AnimationIndex(L"Animation_PlayerMoon_Sword_RunAttack_01"))->Set_AnimationSpeed(1.9f);
			pMyModel->Get_Animation(Get_AnimationIndex(L"Animation_PlayerMoon_Sword_RunAttack_02"))->Set_AnimationSpeed(1.3f);
			pMyModel->Get_Animation(Get_AnimationIndex(L"Animation_PlayerMoon_Sword_RunAttack_03"))->Set_AnimationSpeed(1.3f);
			pMyModel->Get_Animation(Get_AnimationIndex(L"Animation_PlayerMoon_Sword_RunAttack_04"))->Set_AnimationSpeed(1.3f);
			pMyModel->Get_Animation(Get_AnimationIndex(L"Animation_PlayerMoon_Sword_SlideAttack"))->Set_AnimationSpeed(1.3f);
			pMyModel->Get_Animation(Get_AnimationIndex(L"Animation_PlayerMoon_Sword_FallAttack_End"))->Set_AnimationSpeed(1.3f);
			pMyModel->Get_Animation(Get_AnimationIndex(L"Animation_PlayerMoon_Sword_HeavyAttack_Start"))->Set_AnimationSpeed(1.3f);
			pMyModel->Get_Animation(Get_AnimationIndex(L"Animation_PlayerMoon_Sword_HeavyAttack_End"))->Set_AnimationSpeed(1.3f);
			pMyModel->Get_Animation(Get_AnimationIndex(L"Animation_PlayerMoon_HitSP_Lianhuo_Skill07_Start"))->Set_AnimationSpeed(1.0f);
		}

		//// root motion 셋팅
		//{
		//	pMyModel->Get_Animation(Get_AnimationIndex(L"Animation_PlayerMoon_Turn_L45"))->Set_ApplyRootMotion(false);
		//}

	}

	// CascadeBuffer Shader에 연결
	if (FAILED(m_pGameInstance->Set_CascadeShadowConstantBuffer(Get_Component<CShader>())))
		return E_FAIL;

	Set_RenderInfoFlag(OF_Outline, true);
	Set_RenderInfoFlag(OF_Rim, true);
	return S_OK;
}

HRESULT CBody::Awake(const _uint iCurrentLevelIndex)
{
	if (FAILED(Super::Awake(iCurrentLevelIndex)))
		return E_FAIL;

	// 하체 믹스는 끄기 위함
	CModel* pMyModel = Get_Component<CModel>();
	pMyModel->Set_MixAnim_AnimIndex(1, -1);
	Get_Component<CShader>()->Set_Pass(3);
	return S_OK;
}

void CBody::Update_Priority(_float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CBody::Update(_float fTimeDelta)
{
	Super::Update(fTimeDelta);

	//CComputeShader* pGetBoneCS = static_cast<CComputeShader*>(Get_Script_Component(TEXT("ComputeShader_GetBone")));

	Get_Component<CModel>()->Update_Animation(m_pBoneCombineCS, m_pBoneAnimEvaluateCS, fTimeDelta,
		Get_Parent()->Get_Component<CTransform>(), Get_Parent()->Get_Component<CPhysicsCCT>(), m_pBoneAnimBlendCS, m_pBoneAnimMixCS, m_pAdditiveMixCS,nullptr, m_pBoneMoveCS);
	Get_Component<CRenderFx>()->Update(fTimeDelta);

	// GhostTrail
	Get_Component<CModel>()->Update_GhostTrail(fTimeDelta);
}


void CBody::Update_Late(_float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);
	
	Get_Component<CEffectHandler>()->Update(fTimeDelta);
	Get_Component<CModel>()->Emit_Notifies(EAnimNotifyPhase::Late);
}

void CBody::Ready_Before_Render(_float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);
	Super::Update_CombinedWorldMatrix(m_pMatParent);
	CModel *pModel = Get_Component<CModel>();
	pModel->Emit_Notifies(EAnimNotifyPhase::PreRender);
	m_pGameInstance->Push_RenderObject(RENDER_CATEGORY::NONEBLEND, this);
	m_pGameInstance->Push_RenderObject(RENDER_CATEGORY::SHADOW_DYNAMIC, this);
	if (pModel->Has_GhostTrailSnapshots() || pModel->Is_ActiveGhostTrail())
	{
		m_pGameInstance->Push_RenderObject(RENDER_CATEGORY::GHOST_TRAIL, this);
	}
	//CComputeShader* pGetBoneCS = static_cast<CComputeShader*>(Get_Script_Component(TEXT("ComputeShader_GetBone")));
	//Get_Component<CModel>()->Get_BoneMatrix(pGetBoneCS);

#ifdef _DEBUG
	m_pGameInstance->Push_DebugComponent(Get_Component<CCollider>());
#endif 
}

void CBody::OnCollision(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther)
{
	Get_Parent()->OnCollision(iMyColliderLayer, iOtherLayer, pOther);
}

void CBody::OnCollision_Enter(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther, const COL_HIT_INFO& tHitInfo)
{
	Get_Parent()->OnCollision_Enter(iMyColliderLayer, iOtherLayer, pOther, tHitInfo);
}

void CBody::OnCollision_Exit(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther)
{
	Get_Parent()->OnCollision_Exit(iMyColliderLayer, iOtherLayer, pOther);
}

void CBody::OnTrigger_Enter(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther, const COL_HIT_INFO& tHitInfo)
{
	Get_Parent()->OnTrigger_Enter(iMyColliderLayer, iOtherLayer, pOther, tHitInfo);
}

void CBody::OnTrigger_Exit(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther)
{
	Get_Parent()->OnTrigger_Exit(iMyColliderLayer, iOtherLayer, pOther);
}

_bool CBody::On_Hit(const HIT_DESC& hitDesc)
{
	return Get_Parent()->On_Hit(hitDesc);
}

HRESULT CBody::Render()
{
	if (FAILED(Super::Render()))
		return E_FAIL;

	CShader* pShader = Get_Component<CShader>();
	CRenderFx* pRenderFx = Get_Component<CRenderFx>();
	CModel* pModel = Get_Component<CModel>();
	_uint iMeshCount = pModel->Get_MeshCount();

	pRenderFx->Bind_Resources(pShader);
	pShader->Bind_ObjectInfoData(m_tObjectInfoDesc);
	pShader->Bind_TransformData(m_matCombinedWorld);
	for (_uint i = 0; i < iMeshCount; ++i)
	{
		pModel->Bind_Material(pShader, i);
		pModel->Bind_Bones(pShader, i, m_pBoneMeshCS, m_pBoneCombineCS);
		pShader->Apply();
		pModel->Render(i);
	}

	return S_OK;
}

HRESULT CBody::Render_Shadow()
{
	if (FAILED(Super::Render()))
		return E_FAIL;

	CShader* pShader = Get_Component<CShader>();
	_uint iPrevPass = pShader->Get_CurrentPass();

	constexpr _uint iShadowPass = 4;

	// Set Shadow Pass
	pShader->Set_Pass(iShadowPass);
	CModel* pModel = Get_Component<CModel>();
	_uint iMeshCount = pModel->Get_MeshCount();
	pShader->Bind_TransformData(m_matCombinedWorld);
	for (_uint i = 0; i < iMeshCount; ++i)
	{
		pModel->Bind_Bones(pShader, i, m_pBoneMeshCS, m_pBoneCombineCS);
		pShader->Apply();
		pModel->Render(i);
	}

	pShader->Set_Pass(iPrevPass);
	return S_OK;
}

HRESULT CBody::Render_GhostTrail()
{
	Get_Component<CModel>()->Capture_Ghsot(m_pBoneCombineCS, m_matCombinedWorld);
	CShader* pShader = Get_Component<CShader>();
	return Get_Component<CModel>()->Render_GhostTrail(
		pShader,
		m_pBoneMeshCS,
		m_pBoneCombineCS,
		/*  */10);
}

_bool CBody::Resolve_CameraAnchor(Engine::ECameraAnchorResolve eResolve, const string& strAnchorTag, const Matrix& matOwnerWorld, OUT Engine::CAMERA_ANCHOR_RESULT& outResult)
{
	outResult = {};

	CBone* pAnchorBone = Find_CameraAnchorBone(eResolve, strAnchorTag);
	if (pAnchorBone == nullptr)
		return false;

	Matrix matAnchorWorld =
		pAnchorBone->Get_CombinedTransformMatrix() * matOwnerWorld;

	outResult.vPos = matAnchorWorld.Translation();
	outResult.vRight = matAnchorWorld.Right();
	outResult.vUp = matAnchorWorld.Up();
	outResult.vLook = matAnchorWorld.Backward();

	CCameraAnchorResolver::Normalize_AnchorResult(outResult);
	return true;
}

const Matrix* CBody::Get_SocketMatrix(const _char* szBoneName)
{
	if (CBone* pReturn = Get_Component<CModel>()->Get_Bone(szBoneName))
	{
		return &pReturn->Get_CombinedTransformMatrix();
	}

	return nullptr;
}

const Matrix* CBody::Get_SocketMatrix(_uint iIndex)
{
	if (CBone* pReturn = Get_Component<CModel>()->Get_Bone(iIndex))
	{
		return &pReturn->Get_CombinedTransformMatrix();
	}

	return nullptr;
}

const Matrix* CBody::Get_PosMatrix(_uint iIndex)
{
	if (CBone* pReturn = Get_Component<CModel>()->Get_Bone(iIndex))
	{
		return &pReturn->Get_BindPoseTransformMatrix();
	}

	return nullptr;
}

CBone* CBody::Get_CamBone()
{
	if (CBone* pCam = Get_Component<CModel>()->Get_Bone(m_iCamPos_Index))
		return pCam;

	return nullptr;
}

CBone* CBody::Get_CamSocketBone()
{
	if (CBone* pCam = Get_Component<CModel>()->Get_Bone(m_iCamSocket_Index))
		return pCam;

	return nullptr;
}

CBone* CBody::Get_HeadBone()
{
	if (CBone* pHead = Get_Component<CModel>()->Get_Bone(m_iHead_Index))
		return pHead;

	return nullptr;
}

CBone* CBody::Get_Spine1Bone()
{
	if (CBone* pSpine = Get_Component<CModel>()->Get_Bone(m_iSpine1_Index))
		return pSpine;

	return nullptr;
}

CBone* CBody::Get_WeaponSocket()
{
	//289
	if (CBone* pHead = Get_Component<CModel>()->Get_Bone(289)) //285 ~ 289
		return pHead;

	return nullptr;
}

CBone* CBody::Get_RightHandSocket()
{
	if (CBone* pHead = Get_Component<CModel>()->Get_Bone(414))
		return pHead;

	return nullptr;
}

CBone* CBody::Get_NeckBone()
{
	if (CBone* pNeck = Get_Component<CModel>()->Get_Bone(m_iNeck_Index))
		return pNeck;

	return nullptr;
}

//
//CBone* CBody::Get_SpineBone()
//{
//	if (CBone* pSpine = Get_Component<CModel>()->Get_Bone(m_iSpine_Index))
//		return pSpine;
//
//	return nullptr;
//}
//
//
//CBone* CBody::Get_SwordSocket()
//{
//	if (CBone* pSwordSocket = Get_Component<CModel>()->Get_Bone(m_iSwordSocket_Index))
//		return pSwordSocket;
//
//	return nullptr;
//}
//
//CBone* CBody::Get_RightHandWeaponSocket()
//{
//	if (CBone* pRightHandWeaponSocket = Get_Component<CModel>()->Get_Bone(m_iRightHandWeaponSocket_Index))
//		return pRightHandWeaponSocket;
//
//	return nullptr;
//}
//
//CBone* CBody::Get_LeftFootSocket()
//{
//	if (CBone* pLeftFootSocket = Get_Component<CModel>()->Get_Bone(m_iLeftFoot_Index))
//		return pLeftFootSocket;
//
//	return nullptr;
//}
//
//CBone* CBody::Get_RightFootSocket()
//{
//	if (CBone* pRightFootSocket = Get_Component<CModel>()->Get_Bone(m_iRightFoot_Index))
//		return pRightFootSocket;
//
//	return nullptr;
//}
//
//CBone* CBody::Get_LeftShoulderSocket()
//{
//	if (CBone* pLeftShoulderSocket = Get_Component<CModel>()->Get_Bone(m_iLeftShoulderSocket_Index))
//		return pLeftShoulderSocket;
//
//	return nullptr;
//}
//
//CBone* CBody::Get_RightShoulderSocket()
//{
//	if(CBone * pRightShoulderSocket = Get_Component<CModel>()->Get_Bone(m_iRightShoulderSocket_Index))
//		return pRightShoulderSocket;
//
//	return nullptr;
//}
//
//CBone* CBody::Get_EffectMouseSocket()
//{
//	if (CBone* pEffectMouseSocket = Get_Component<CModel>()->Get_Bone(m_iEffectMouseSocket_Index))
//		return pEffectMouseSocket;
//
//	return nullptr;
//}
//
//CBone* CBody::Get_LeftHandSocket()
//{
//	if (CBone* pLeftHandSocket = Get_Component<CModel>()->Get_Bone(m_iLeftHand_Index))
//		return pLeftHandSocket;
//
//	return nullptr;
//}
//
//CBone* CBody::Get_RightHandSocket()
//{
//	if (CBone* pRightHandSocket = Get_Component<CModel>()->Get_Bone(m_iRightHand_Index))
//		return pRightHandSocket;
//
//	return nullptr;
//}

HRESULT CBody::Ready_Components(BODY_DESC* pDesc)
{
	if (FAILED(Add_Component<CModel>(0/*static*/, pDesc->wstrModelPrototypeName, nullptr)))
		return E_FAIL;

	if (FAILED(Add_Component<CShader>(0/*static*/, L"Prototype_Component_Shader_VtxAnimMesh", nullptr)))
		return E_FAIL;

	// RenderFx
	{
		CRenderFx::RENDER_FX_COPY_DESC desc{};
		desc.vEmissiveColor = Vec3{ 0.55f, 0.82f, 1.00f };
		desc.fEmissiveDefaultIntensity = 0.8f;
		desc.fShakeAmpX = 0.015f;
		desc.fShakeAmpY = 0.030f;
		desc.fShakeFreq = 9.0f;
		desc.fShakePhase = 0.0f;
		if (FAILED(Add_Component<CRenderFx>(0, L"Prototype_Component_RenderFx", &desc)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CBody::Bind_ShaderResources()
{
	return S_OK;
}

HRESULT CBody::Ready_ComputeShader()
{
	_uint iBoneNums = Get_Component<CModel>()->Get_BoneCount();
	_uint iGetBoneNums = Get_Component<CModel>()->Get_StageBoneCount();
	// ========   Compute Shader : BoneMesh  ========
	{
		CComputeShader::ComShaderCopyDesc ShaderDesc = {};
		ShaderDesc.Output_SRVBuffer_Name = "BONEFNIMAL_TRANSFORMS_SRV";

		ShaderDesc.InputBufferNum	= 2;
		ShaderDesc.bMakeSB			= false;
		//// 입력 버퍼
		//ShaderDesc.Input_StructBuffer.sBufferName = "IMMU_BONEDATA";
		//ShaderDesc.Input_StructBuffer.iElementSize = sizeof(CS_IMMU_BONE);
		//ShaderDesc.Input_StructBuffer.iNumElements = iBoneNums;

		// 출력 버퍼
		ShaderDesc.OutPut_StructBuffer.sBufferName	= "BONEFNIMAL_TRANSFORMS";
		ShaderDesc.OutPut_StructBuffer.iElementSize = sizeof(CS_OUT_BONE);
		ShaderDesc.OutPut_StructBuffer.iNumElements = iBoneNums;

		if (FAILED(Add_Script_Component(L"ComputeShader_BoneMesh", L"Prototype_Component_Shader_BoneMesh", &ShaderDesc, CAST_VOID_PP(&m_pBoneMeshCS))))
			return E_FAIL;
	}

	// ========   Compute Shader : BoneCombine  ========
	{
		CComputeShader::ComShaderCopyDesc ShaderDesc = {};
		ShaderDesc.Output_SRVBuffer_Name = "BONECOMBINED_TRANSFORMS_SRV";

		ShaderDesc.InputBufferNum = 3;
		// 입력 버퍼
		ShaderDesc.Input_StructBuffer.sBufferName = "IMMU_BONEDATA";
		ShaderDesc.Input_StructBuffer.iElementSize = sizeof(CS_IMMU_BONE);
		ShaderDesc.Input_StructBuffer.iNumElements = iBoneNums;

		// 출력 버퍼
		ShaderDesc.OutPut_StructBuffer.sBufferName = "BONECOMBINED_TRANSFORMS";
		ShaderDesc.OutPut_StructBuffer.iElementSize = sizeof(CS_OUT_BONE);
		ShaderDesc.OutPut_StructBuffer.iNumElements = iBoneNums;

		if (FAILED(Add_Script_Component(L"ComputeShader_BoneCombine", L"Prototype_Component_Shader_BondCombine", &ShaderDesc, CAST_VOID_PP(&m_pBoneCombineCS))))
			return E_FAIL;
	}

	// ========   Compute Shader : AnimE  ========
	{
		CComputeShader::ComShaderCopyDesc ShaderDesc = {};
		ShaderDesc.Output_SRVBuffer_Name = "CHANNEL_OUTPUT_SRV";

		ShaderDesc.InputBufferNum = 2;
		ShaderDesc.bMakeSB = false;
		//// 입력 버퍼
		//ShaderDesc.Input_StructBuffer.sBufferName = "IMMU_EFFECT_PARTICLE";
		//ShaderDesc.Input_StructBuffer.iElementSize = sizeof(EFFECT_PARTICLE_IMMU_ELEMENT);
		//ShaderDesc.Input_StructBuffer.iNumElements = m_tEffectDesc._Effect_MaxParticle;

		// 출력 버퍼
		ShaderDesc.OutPut_StructBuffer.sBufferName = "CHANNEL_OUTPUT";
		ShaderDesc.OutPut_StructBuffer.iElementSize = sizeof(CS_SRT);
		ShaderDesc.OutPut_StructBuffer.iNumElements = iBoneNums;

		if (FAILED(Add_Script_Component(L"ComputeShader_AnimE", L"Prototype_Component_Shader_AnimEv", &ShaderDesc, CAST_VOID_PP(&m_pBoneAnimEvaluateCS))))
			return E_FAIL;
	}

	// ========   Compute Shader : AnimB  ========
	{
		CComputeShader::ComShaderCopyDesc ShaderDesc = {};
		ShaderDesc.Output_SRVBuffer_Name = "BLEND_OUTPUT_SRV";

		ShaderDesc.InputBufferNum = 2;
		ShaderDesc.bMakeSB = false;
		//// 입력 버퍼
		//ShaderDesc.Input_StructBuffer.sBufferName = "IMMU_EFFECT_PARTICLE";
		//ShaderDesc.Input_StructBuffer.iElementSize = sizeof(EFFECT_PARTICLE_IMMU_ELEMENT);
		//ShaderDesc.Input_StructBuffer.iNumElements = iBoneNums;

		// 출력 버퍼
		ShaderDesc.OutPut_StructBuffer.sBufferName = "BLEND_OUTPUT";
		ShaderDesc.OutPut_StructBuffer.iElementSize = sizeof(CS_SRT);
		ShaderDesc.OutPut_StructBuffer.iNumElements = iBoneNums;

		if (FAILED(Add_Script_Component(L"ComputeShader_AnimB", L"Prototype_Component_Shader_AnimB", &ShaderDesc, CAST_VOID_PP(&m_pBoneAnimBlendCS))))
			return E_FAIL;
	}

	// ========   Compute Shader : AnimMix  ========
	{
		CComputeShader::ComShaderCopyDesc ShaderDesc = {};
		ShaderDesc.Output_SRVBuffer_Name = "CHANNEL_OUTPUT_SRV";

		ShaderDesc.InputBufferNum = 4;
		ShaderDesc.bMakeSB = false;
		//// 입력 버퍼
		//ShaderDesc.Input_StructBuffer.sBufferName = "IMMU_EFFECT_PARTICLE";
		//ShaderDesc.Input_StructBuffer.iElementSize = sizeof(EFFECT_PARTICLE_IMMU_ELEMENT);
		//ShaderDesc.Input_StructBuffer.iNumElements = iBoneNums;

		// 출력 버퍼
		ShaderDesc.OutPut_StructBuffer.sBufferName = "CHANNEL_OUTPUT";
		ShaderDesc.OutPut_StructBuffer.iElementSize = sizeof(CS_SRT);
		ShaderDesc.OutPut_StructBuffer.iNumElements = iBoneNums;

		if (FAILED(Add_Script_Component(L"ComputeShader_AnimMix", L"Prototype_Component_Shader_AnimMix", &ShaderDesc, CAST_VOID_PP(&m_pBoneAnimMixCS))))
			return E_FAIL;
	}

	// ========   Compute Shader : AdditiveMix  ========
	{
		CComputeShader::ComShaderCopyDesc ShaderDesc = {};
		ShaderDesc.Output_SRVBuffer_Name = "CHANNEL_OUTPUT_SRV";

		ShaderDesc.InputBufferNum = 6;
		ShaderDesc.bMakeSB = false;
		//// 입력 버퍼
		//ShaderDesc.Input_StructBuffer.sBufferName = "IMMU_EFFECT_PARTICLE";
		//ShaderDesc.Input_StructBuffer.iElementSize = sizeof(EFFECT_PARTICLE_IMMU_ELEMENT);
		//ShaderDesc.Input_StructBuffer.iNumElements = iBoneNums;

		// 출력 버퍼
		ShaderDesc.OutPut_StructBuffer.sBufferName = "CHANNEL_OUTPUT";
		ShaderDesc.OutPut_StructBuffer.iElementSize = sizeof(CS_SRT);
		ShaderDesc.OutPut_StructBuffer.iNumElements = iBoneNums;

		if (FAILED(Add_Script_Component(L"ComputeShader_AnimAdditiveMix", L"Prototype_Component_Shader_AnimAdditiveMix", &ShaderDesc, CAST_VOID_PP(&m_pAdditiveMixCS))))
			return E_FAIL;
	}

	// ========   Compute Shader : BoneMove  ========
	{
		CComputeShader::ComShaderCopyDesc ShaderDesc = {};
		ShaderDesc.Output_SRVBuffer_Name = "FINAL_SRT_SRV";

		ShaderDesc.InputBufferNum = 0;
		ShaderDesc.bMakeSB = false;

		// 출력 버퍼
		ShaderDesc.OutPut_StructBuffer.sBufferName = "FINAL_SRT";
		ShaderDesc.OutPut_StructBuffer.iElementSize = sizeof(CS_SRT);
		ShaderDesc.OutPut_StructBuffer.iNumElements = iBoneNums;

		if (FAILED(Add_Script_Component(L"ComputeShader_BoneMove", L"Prototype_Component_Shader_BoneMove", &ShaderDesc, CAST_VOID_PP(&m_pBoneMoveCS))))
			return E_FAIL;
	}

	// ========   Compute Shader : GetBone  ========
	{
		//CComputeShader::ComShaderCopyDesc ShaderDesc = {};
		//ShaderDesc.Output_SRVBuffer_Name = "SELECTED_COMBINETRANSFORMS_SRV";

		//ShaderDesc.InputBufferNum = 2;
		//ShaderDesc.bMakeSB = true;
		//// 입력 버퍼
		//ShaderDesc.Input_StructBuffer.sBufferName	= "IMMU_BONEINDIECS";
		//ShaderDesc.Input_StructBuffer.iElementSize	= sizeof(CS_MU_BONEIDX);
		//ShaderDesc.Input_StructBuffer.iNumElements	= iGetBoneNums; // 내가 저장하고 싶은 본 개수

		//// 출력 버퍼
		//ShaderDesc.OutPut_StructBuffer.sBufferName = "SELECTED_COMBINETRANSFORMS";
		//ShaderDesc.OutPut_StructBuffer.iElementSize = sizeof(CS_OUT_BONE);
		//ShaderDesc.OutPut_StructBuffer.iNumElements = iGetBoneNums;

		//if (FAILED(Add_Script_Component(L"ComputeShader_GetBone", L"Prototype_Component_Shader_GetBone", &ShaderDesc)))
		//	return E_FAIL;
	}

	if (FAILED(Get_Component<CModel>()->Ready_ComputeShaders(m_pBoneMeshCS, m_pBoneCombineCS, m_pBoneAnimEvaluateCS, m_pBoneAnimBlendCS, m_pBoneAnimMixCS, m_pAdditiveMixCS)))
		return E_FAIL;

	return S_OK;
}

CBone* CBody::Find_CameraAnchorBone(ECameraAnchorResolve eResolve, const string& strAnchorTag)
{
	switch (eResolve)
	{
	case Engine::ECameraAnchorResolve::CAM_SOCKET:
	{
		return Get_CamSocketBone();
	}

	case Engine::ECameraAnchorResolve::BONE:
	{
		if (strAnchorTag.empty())
			return nullptr;

		return Get_Component<CModel>()->Get_Bone(strAnchorTag.c_str());
	}
	}

	return nullptr;
}

CBody* CBody::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CBody* pInstance = new CBody(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CBody::Create, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CBody::Clone(void* pArg)
{
	CBody* pInstance = new CBody(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CBody::Clone, Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBody::Free()
{
	Super::Free();
}
