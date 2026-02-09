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
#include "ComputeShader.h"

#include "GameInstance.h"

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
	//m_iLeftHand_Index = Get_Component<CModel>()->Get_BoneIndex("hand_l");
	//m_iRightHand_Index = Get_Component<CModel>()->Get_BoneIndex("hand_r");
	//m_iLeftFoot_Index = Get_Component<CModel>()->Get_BoneIndex("LeftFoot");
	//m_iRightFoot_Index = Get_Component<CModel>()->Get_BoneIndex("RightFoot");
	//m_iSpine_Index = Get_Component<CModel>()->Get_BoneIndex("Spine");
	//m_iEffectMouseSocket_Index = Get_Component<CModel>()->Get_BoneIndex("EFF_Mouth01");
	//m_iSwordSocket_Index = Get_Component<CModel>()->Get_BoneIndex("Attach_NinjaSword");
	//m_iLeftShoulderSocket_Index = Get_Component<CModel>()->Get_BoneIndex("LeftShoulder");
	//m_iRightShoulderSocket_Index = Get_Component<CModel>()->Get_BoneIndex("RightShoulder");
	//m_iRightHandWeaponSocket_Index = Get_Component<CModel>()->Get_BoneIndex("MeleeWeaponRHand");
	return S_OK;
}

HRESULT CBody::Awake(const _uint iCurrentLevelIndex)
{
	if (FAILED(Super::Awake(iCurrentLevelIndex)))
		return E_FAIL;

	return S_OK;
}

void CBody::Update_Priority(_float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CBody::Update(_float fTimeDelta)
{
	Super::Update(fTimeDelta);

	CComputeShader* pBonCS = static_cast<CComputeShader*>(Get_Script_Component(TEXT("ComputeShader_BoneCombine")));
	CComputeShader* pAnimECS = static_cast<CComputeShader*>(Get_Script_Component(TEXT("ComputeShader_AnimE")));
	CComputeShader* pAnimBCS = static_cast<CComputeShader*>(Get_Script_Component(TEXT("ComputeShader_AnimB")));

	Get_Component<CModel>()->Update_Animation(pBonCS, pAnimECS, pAnimBCS,
		fTimeDelta, Get_Parent()->Get_Component<CTransform>(), Get_Parent()->Get_Component<CPhysicsCCT>());

	if(CCollider* pCollider = Get_Component<CCollider>())
		pCollider->Update(m_matCombinedWorld);
}

void CBody::Update_Late(_float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);
}

void CBody::Ready_Before_Render(_float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);
	m_pGameInstance->Push_RenderObject(RENDER_CATEGORY::NONEBLEND, this);
	Super::Update_CombinedWorldMatrix(m_pMatParent);
#ifdef _DEBUG
	m_pGameInstance->Push_DebugComponent(Get_Component<CCollider>());
#endif 

}

void CBody::OnCollision(_uint iMyColliderLayer, CCollider* pOther)
{
	Get_Parent()->OnCollision(iMyColliderLayer, pOther);
}

void CBody::OnCollision_Enter(_uint iMyColliderLayer, CCollider* pOther)
{
	Get_Parent()->OnCollision_Enter(iMyColliderLayer, pOther);
}

void CBody::OnCollision_Exit(_uint iMyColliderLayer, CCollider* pOther)
{
	Get_Parent()->OnCollision_Exit(iMyColliderLayer, pOther);
}

_bool CBody::On_Hit(_uint iCollideMyLayer, ATTACK_DESC* pDesc, CGameObject* pOther)
{
	return Get_Parent()->On_Hit(iCollideMyLayer, pDesc, pOther);
}

HRESULT CBody::Render()
{
	if (FAILED(Super::Render()))
		return E_FAIL;

	CShader* pShader = Get_Component<CShader>();
	CModel* pModel = Get_Component<CModel>();
	_uint iMeshCount = pModel->Get_MeshCount();

	pShader->Bind_TransformData(m_matCombinedWorld);
	for (_uint i = 0; i < iMeshCount; ++i)
	{
		pModel->Bind_Material(pShader, i);
		pModel->Bind_Bones(pShader, i);
		pShader->Apply();
		pModel->Render(i);
	}

	return S_OK;
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
	if (FAILED(Add_Component<CModel>(0/*static*/, pDesc->wstrModelPrototypeName, pDesc)))
		return E_FAIL;

	if (FAILED(Add_Component<CShader>(0/*static*/, L"Prototype_Component_Shader_VtxAnimMesh", pDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBody::Bind_ShaderResources()
{
	return S_OK;
}

HRESULT CBody::Ready_ComputeShader()
{
	_uint iBoneNums = Get_Component<CModel>()->Get_BoneCount();
	// ========   Compute Shader : Bone  ========
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

		if (FAILED(Add_Script_Component(L"ComputeShader_BoneCombine", L"Prototype_Component_Shader_BondCombine", &ShaderDesc)))
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

		if (FAILED(Add_Script_Component(L"ComputeShader_AnimE", L"Prototype_Component_Shader_AnimEv", &ShaderDesc)))
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

		if (FAILED(Add_Script_Component(L"ComputeShader_AnimB", L"Prototype_Component_Shader_AnimB", &ShaderDesc)))
			return E_FAIL;
	}


	if (FAILED(Get_Component<CModel>()->Ready_CSs(static_cast<CComputeShader*>(Get_Script_Component(L"ComputeShader_BoneCombine")),
		static_cast<CComputeShader*>(Get_Script_Component(L"ComputeShader_AnimE")))))
		return E_FAIL;

	return S_OK;
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
