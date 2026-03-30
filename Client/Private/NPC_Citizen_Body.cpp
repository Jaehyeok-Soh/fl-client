#include "pch.h"
#include "NPC_Citizen_Body.h"
#include "Model.h"
#include "Shader.h"
#include "PhysicsCCT.h"
#include "ComputeShader.h"
#include "NPC_Citizen.h"
#include "GameInstance.h"

CNPC_Citizen_Body::CNPC_Citizen_Body(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CNPC_Body_Base(pDevice, pDeviceContext)
	, m_strLoopAninName{}
	, m_pBoneMeshCS{nullptr}
	, m_pBoneCombineCS{ nullptr }
	, m_pAnimBlendCS{nullptr}
	, m_pAnimECS{nullptr}
	, m_pAnimMix{nullptr}
	, m_vecShaderPass{}
	, m_tRGBColorDesc{}
	, m_pCBCitizenFaceData{nullptr}
{ 
}	 

CNPC_Citizen_Body::CNPC_Citizen_Body(const CNPC_Citizen_Body& rhs)
	: CNPC_Body_Base(rhs)
	, m_strLoopAninName{ rhs.m_strLoopAninName }
	, m_pBoneMeshCS{ rhs.m_pBoneMeshCS }
	, m_pBoneCombineCS{ rhs.m_pBoneCombineCS }
	, m_pAnimBlendCS{ rhs.m_pAnimBlendCS }
	, m_pAnimECS{ rhs.m_pAnimECS }
	, m_pAnimMix{ rhs.m_pAnimMix }
	, m_vecShaderPass{rhs.m_vecShaderPass }
	, m_tRGBColorDesc{rhs.m_tRGBColorDesc}
	, m_pCBCitizenFaceData{rhs.m_pCBCitizenFaceData }
{
}

HRESULT CNPC_Citizen_Body::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CNPC_Citizen_Body::Initialize(void* pArg)
{
	/* Transform 생성 */
	if (FAILED(CGameObject::Initialize(pArg)))
		return E_FAIL;

	CNPC_Citizen_Body::NPC_CITIZEN_BODY* pDesc = static_cast<CNPC_Citizen_Body::NPC_CITIZEN_BODY*>(pArg);

	m_strLoopAninName = pDesc->strLoopAnimName;

	m_tRGBColorDesc = pDesc->tRGBColorData;

	m_pMatParent = pDesc->pMatParent;

	if (FAILED(Ready_Component(pDesc)))
		return E_FAIL;

	if (FAILED(CNPC_Body_Base::Ready_ComputeShader()))
		return E_FAIL;

	if (FAILED(this->Ready_Animation(pDesc)))
		return E_FAIL;

	if (FAILED(this->Ready_ShaderPass(pDesc)))
		return E_FAIL;

	if (FAILED(this->Ready_FaceData(pDesc)))
		return E_FAIL;

	m_pGameInstance->Bind_DissolveTexture(Get_Component<CShader>());

	return S_OK;
}

HRESULT CNPC_Citizen_Body::Ready_Component(NPC_CITIZEN_BODY* pDesc)
{
	static Matrix CitizenPreMatrix = Matrix::CreateScale(0.01f,0.01f,0.01f) * Matrix::CreateRotationX(XMConvertToRadians(90.f));
	/* Body Model 생성 */
	/*  Citizen Model */

	static const wstring& wstrCitizenTag = L"NPC_Citizen/";

	const wstring& wstrModelFolderPath = pDesc->wstrModelPrototypeTag;
	const wstring& wstrModelFoloderFullPath = wstrCitizenTag + wstrModelFolderPath;

	const wstring& wstrModelPrototypeTag = g_wszModel_Prototype_Tag + wstrModelFolderPath ;	/* FolderPath */

	CModel::MODEL_ORIGIN_DESC tDesc{};
	tDesc.eType = EModelType::ANIM;
	tDesc.iPrototypeLevelIndex = ENUM_TO_UINT(ELevelType::STATIC);
	tDesc.pMatPreTransform = &CitizenPreMatrix;
	tDesc.wstrModelFolderName = wstrModelFoloderFullPath;
	tDesc.FStageBone = CModel::STAGEING_BONE::SB_ALLBONE;

	/* Anim Model CHannel Data */
	CModel::DATA_ANIMCHANNEL tAnimChannelData{};
	tAnimChannelData.bMixAni = false;
	tAnimChannelData.bRootAni = false;
	tAnimChannelData.iRootBoneIndex = 3;

	//tDesc.vecStageBoneIndices = { 3 };
	tDesc.pAniChannelData = &tAnimChannelData;

	CBase* pResult{ nullptr };
	if (nullptr == (pResult = m_pGameInstance->Find_Prototype(tDesc.iPrototypeLevelIndex, wstrModelPrototypeTag)))
	{
		/* 찾았는데 없다면 */
		if (FAILED(m_pGameInstance->Add_Prototype(tDesc.iPrototypeLevelIndex, wstrModelPrototypeTag, CModel::Create(m_pDevice, m_pDeviceContext, &tDesc))))
			return E_FAIL;
	}

	/* Model Add */
	if (FAILED(Add_Component<CModel>(tDesc.iPrototypeLevelIndex, wstrModelPrototypeTag, nullptr)))
		return E_FAIL;



	/* Shader Add */
	if (FAILED(CGameObject::Add_Component<CShader>(ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_Component_Shader_VtxAnimMesh", nullptr)))
		return E_FAIL;



	return S_OK;
}

HRESULT CNPC_Citizen_Body::Ready_Animation(NPC_CITIZEN_BODY* pDesc)
{
	m_pBoneMeshCS = static_cast<CComputeShader*>(Get_Script_Component(TEXT("ComputeShader_BoneMesh")));
	m_pBoneCombineCS = static_cast<CComputeShader*>(Get_Script_Component(TEXT("ComputeShader_BoneCombine")));
	m_pAnimECS = static_cast<CComputeShader*>(Get_Script_Component(TEXT("ComputeShader_AnimE")));
	m_pAnimBlendCS = static_cast<CComputeShader*>(Get_Script_Component(TEXT("ComputeShader_AnimB")));
	m_pAnimMix = static_cast<CComputeShader*>(Get_Script_Component(TEXT("ComputeShader_AnimMix")));

	CModel* pModel = Get_Component<CModel>();
	if (pModel == nullptr) return E_FAIL;


	if (pDesc->iLoopAnimIndex == -1)
	{
		m_iAnimIndex = pModel->Get_AnimationIndex(Engine_Utils::ToWString(m_strLoopAninName));
		if (FAILED(pModel->Change_Animation(m_pAnimECS, m_iAnimIndex , false, true , true)))
			return E_FAIL;
	}
	else
	{
		if (FAILED(pModel->Change_Animation(m_pAnimECS, m_iAnimIndex , false, true, true)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CNPC_Citizen_Body::Ready_ShaderPass(NPC_CITIZEN_BODY* pDesc)
{
	CModel* pModel = Get_Component<CModel>();
	_uint iMtlCount = pModel->Get_MaterialCount();
	m_vecShaderPass.resize(iMtlCount);

	for (_uint i = 0; i < pModel->Get_MaterialCount(); ++i)
	{
		EAnimShaderPass ePass{ EAnimShaderPass::Default };
		wstring wstrMtlName = pModel->Get_MaterialName(i);
		if (wstrMtlName.find(L"Eye") != std::wstring::npos)
			ePass = EAnimShaderPass::CitizenEye;
		else if (wstrMtlName.find(L"Mouth") != std::wstring::npos)
			ePass = EAnimShaderPass::CitizenMouth;
		else if (wstrMtlName.find(L"Cloth") != std::wstring::npos)
			ePass = EAnimShaderPass::CitizenCloth;
		else if (wstrMtlName.find(L"Body") != std::wstring::npos)
			ePass = EAnimShaderPass::CitizenBody;
		m_vecShaderPass[i] = ePass;
	}
	return S_OK;
}

HRESULT CNPC_Citizen_Body::Ready_FaceData(NPC_CITIZEN_BODY* pDesc)
{
	for (_uint i = 0; i < (_uint)DTO::CITIZEN_ATLAS_TYPE::END; ++i)
	{
		auto& tAtlasData = pDesc->arrayAtlasDatas[i];
		m_tCBCitizenFaceData.SetFaceUV((DTO::CITIZEN_ATLAS_TYPE)i, &tAtlasData);
	}


	CShader* pShader = Get_Component<CShader>();
	if (pShader == nullptr) return E_FAIL;

	ID3DX11EffectConstantBuffer* pCB = pShader->Get_ConstantBuffer("CB_CitizentFaceData");
	if (!pCB->IsValid())
		return E_FAIL;

	m_pCBCitizenFaceData = pCB;


	return S_OK;
}

HRESULT CNPC_Citizen_Body::Awake(const _uint iCurrentLevelIndex)
{
	if (FAILED(Super::Awake(iCurrentLevelIndex)))
		return E_FAIL;


	if (FAILED(Ready_DissolveEffect_Setting()))
		return E_FAIL;


	/* 애니매이션 버그 고치기 프로젝트 */




	return S_OK;
}

void CNPC_Citizen_Body::Update_Priority(_float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CNPC_Citizen_Body::Update(_float fTimeDelta)
{
	Super::Update(fTimeDelta);

	m_tDissolveDesc.Update(fTimeDelta);
}

void CNPC_Citizen_Body::Update_Late(_float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);
}

void CNPC_Citizen_Body::Ready_Before_Render(_float fTimeDelta)
{
	//Super::Ready_Before_Render(fTimeDelta);


	CModel* pModel = Get_Component<CModel>();
	if (pModel)
		pModel->Update_Animation( m_pBoneCombineCS , m_pAnimECS, fTimeDelta ,m_pParentObject->Get_Component<CTransform>() ,
			m_pParentObject->Get_Component<CPhysicsCCT>(), m_pAnimBlendCS);

	Super::Update_CombinedWorldMatrix(*m_pMatParent);


	m_pGameInstance->Push_RenderObject(RENDER_CATEGORY::NONEBLEND, this);
}

void CNPC_Citizen_Body::OnCollision(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther)
{
}

void CNPC_Citizen_Body::OnCollision_Enter(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther, const COL_HIT_INFO& tHitInfo)
{
}

void CNPC_Citizen_Body::OnCollision_Exit(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther)
{
}

void CNPC_Citizen_Body::OnTrigger_Enter(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther, const COL_HIT_INFO& tHitInfo)
{
}

void CNPC_Citizen_Body::OnTrigger_Exit(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther)
{
}

_bool CNPC_Citizen_Body::On_Hit(const HIT_DESC& hitDesc)
{
	return false;
}

HRESULT CNPC_Citizen_Body::Render()
{
	//if (FAILED(Super::Render()))
	//	return E_FAIL;

	CShader* pShader	= Get_Component<CShader>();
	CModel* pModel		= Get_Component<CModel>();
	_uint iMeshCount	= pModel->Get_MeshCount();
	

	pShader->Bind_ObjectInfoData(m_tObjectInfoDesc);
	pShader->Bind_TransformData(m_matCombinedWorld);
	pShader->Bind_RGBColorData(m_tRGBColorDesc);
	pShader->Bind_DissolveEffectData(m_tDissolveDesc.ShaderData);


	m_pCBCitizenFaceData->SetRawValue(&m_tCBCitizenFaceData , 0 , sizeof(m_tCBCitizenFaceData) );

	for (_uint i = 0; i < iMeshCount; ++i)
	{
		pShader->Set_Pass(ENUM_TO_UINT(m_vecShaderPass[i]));
		pModel->Bind_Material(pShader, i);
		pModel->Bind_Bones(pShader, i, m_pBoneMeshCS, m_pBoneCombineCS);
		pShader->Apply();
		pModel->Render(i);
	}

	// 디졸브 값 초기화
	SHADER_DISSOLVE_EFFECT_DESC Desc = {};
	pShader->Bind_DissolveEffectData(Desc);

	return S_OK;
}

HRESULT CNPC_Citizen_Body::Ready_DissolveEffect_Setting()
{
	using DS = DissolveEffectDesc;
	m_tDissolveDesc.Reset();
	m_tDissolveDesc.Add_DissolveFlag(DS::BIT_SPAWN_START,/* DS::BIT_USE_ALPHA_FADE, */DS::BIT_USE_DISSOLVE_MAP);
	m_tDissolveDesc.Set_Dissolve_Setting(3.f,1.f);
	m_tDissolveDesc.Set_Spawn_Setting(1.f, 1.f);
	m_tDissolveDesc.Set_ObjectType(DS::DISSOLVE_OBJECTTYPE::TYPE_NPC);

	// 스폰 시간 & 디졸브 시간
	m_tDissolveDesc.ShaderData.fDissolveEdgeColor = SimpleMath::Vector3(1.f,1.f,1.f);
	m_tDissolveDesc.ShaderData.fDissolveEdgeWidth = 0.1f;

	return S_OK;
}

void CNPC_Citizen_Body::DissolveStarts()
{
	using DS = DissolveEffectDesc;
	m_tDissolveDesc.Reset();
	m_tDissolveDesc.Add_DissolveFlag(DS::BIT_DISSOLVE_START, DS::BIT_USE_EDGE,/* DS::BIT_USE_ALPHA_FADE, */DS::BIT_USE_DISSOLVE_MAP);
}

HRESULT CNPC_Citizen_Body::Change_Animation(_uint iAnimIndex)
{
	CModel* pModel = Get_Component<CModel>();
	if (!pModel) return E_FAIL;

	return pModel->Change_Animation(m_pAnimECS, iAnimIndex, false, true);
}

CNPC_Citizen_Body* CNPC_Citizen_Body::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CNPC_Citizen_Body* pBody = new CNPC_Citizen_Body(pDevice, pContext);

	if (FAILED(pBody->Initialize_Prototype()))
	{
		Safe_Release(pBody);
		MSG_BOX("NPC Body Citizen is Failed to Create");
		return nullptr;
	}

	return pBody;
}

CGameObject* CNPC_Citizen_Body::Clone(void* pArg)
{
	CNPC_Citizen_Body* pBody = new CNPC_Citizen_Body(*this);

	if (FAILED(pBody->Initialize(pArg)))
	{
		Safe_Release(pBody);
		MSG_BOX("NPC Body Citizen is Failed to Clone");
		return nullptr;
	}

	return pBody;
}

void CNPC_Citizen_Body::Free()
{
	Super::Free();

	m_pCBCitizenFaceData = nullptr;
}

