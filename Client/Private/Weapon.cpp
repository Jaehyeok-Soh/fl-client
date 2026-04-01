#include "pch.h"
#include "Client_Defines.h"
#include "Weapon.h"

#include "Collider.h"
#include "Shader.h"
#include "Model.h"
#include "ComputeShader.h"

#include "GameInstance.h"

#include "PlayerImguiValues.h"

CWeapon::CWeapon(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, Weapon_Type eWeapon)
	: Super(pDevice, pDeviceContext)
	, m_eWaeponType(eWeapon)
{
}

CWeapon::CWeapon(const CWeapon& rhs)
	: Super(rhs)
	, m_eWaeponType(rhs.m_eWaeponType)
	, m_tColorDesc(rhs.m_tColorDesc)
	, m_eAnimState(rhs.m_eAnimState)
	, m_matHandOffsetMatrix(rhs.m_matHandOffsetMatrix)
	, m_matHoldOffsetMatrix(rhs.m_matHoldOffsetMatrix)
	, m_bPlayOnceYet(rhs.m_bPlayOnceYet)
{
}

HRESULT CWeapon::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;

	m_matHandOffsetMatrix = Matrix::Identity;
	m_matHoldOffsetMatrix = Matrix::Identity;

	return S_OK;
}

HRESULT CWeapon::Initialize(void* pArg)
{
	if (!pArg)
		return E_FAIL;

	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	WEAPON_DESC* pDesc = static_cast<WEAPON_DESC*>(pArg);
	m_pMatHandSocket = pDesc->pMatHandSocket;
	m_pMatSocket = pDesc->pMatSocket;

	m_eModleType = pDesc->eModel;
	m_eAnimState = pDesc->eAnimState;
	m_eState = pDesc->eState;

	m_FDescFlags = pDesc->FDescFlag;
	m_bMainWeapon = pDesc->bMianWeapon;

	m_matHandOffsetMatrix = pDesc->matHandOffsetMatrix;
	m_matHoldOffsetMatrix = pDesc->matHoldOffsetMatrix;
	m_matConOffsetMatrix = pDesc->matConOffsetMatrix;

	if (FAILED(Ready_Components(pDesc)))
		return E_FAIL;

	switch (m_eModleType)
	{
	case Weapon_ModelType::STATIC:
		break;

	case Weapon_ModelType::ANIM:
		if (FAILED(Ready_ComputeShaders()))
			return E_FAIL;

		Get_Component<CModel>()->Change_Animation(m_pAnimECS, pDesc->iStartAnimIdx, false,true,true);

		break;
	}

	if (!m_bMainWeapon)
		Set_WeaponState(State::NONE);

	//Get_Component<CTransform>()->Set_Scale(0.1f, 0.1f, 0.1f);
	//Get_Component<CTransform>()->Rotation(0.f, ::XMConvertToRadians(90.f), 0.f);
	//Get_Component<CTransform>()->Set_Info(TRANSFORM_INFO_STATE::POS, ::XMVectorSet(0.8f, 0.f, 0.f, 1.f));

	Set_RenderInfoFlag(OF_Outline, true);
	Set_RenderInfoFlag(OF_Rim, true);

	if (Is_Dissolve())
	{
		if (FAILED(Ready_DissolveEffect_Setting(pDesc)))
			return E_FAIL;
	}

	// CascadeBuffer Shader에 연결
	if (FAILED(m_pGameInstance->Set_CascadeShadowConstantBuffer(Get_Component<CShader>())))
		return E_FAIL;

	return S_OK;
}

HRESULT CWeapon::Awake(const _uint iCurrentLevelIndex)
{
	if (FAILED(Super::Awake(iCurrentLevelIndex)))
		return E_FAIL;

	return S_OK;
}

void CWeapon::Update_Priority(_float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);	
	//_matrix matSocket = ::XMLoadFloat4x4(m_pMatSocket);
	//// Right, Up, Look 노멀라이즈로 스케일 죽이기
	//// 예시코드에서 Socket에 적용된 스케일때문에 외부 Mesh를 붙였을때 이상하게 출력
	//// 이렇게 직접 컨트롤이 가능하다는 예시
	//for (_uint i = 0; i < 3; ++i)
	//	matSocket.r[i] = ::XMVector3Normalize(matSocket.r[i]);
	//_matrix matParent = matSocket * ::XMLoadFloat4x4(m_pMatParent);
	//Super::Update_CombinedWorldMatrix(matParent);


	// state에 따른 combineworld 업데이트
	switch (m_eState)
	{
	case State::HOLD:
	{

	}
	break;
	}
}

void CWeapon::Update(_float fTimeDelta)
{
	Super::Update(fTimeDelta);

	// scale이 죽었을때 다시 살리기 위함
	//if (CCollider* pCollider = Get_Component<CCollider>())
	//	pCollider->Update(Matrix::CreateScale(100.f, 100.f, 100.f) * m_matCombinedWorld);

	switch (m_eModleType)
	{
	case Weapon_ModelType::STATIC:
		break;

	case Weapon_ModelType::ANIM:
		Play_Anim(fTimeDelta);
		break;
	}
}

void CWeapon::Update_Late(_float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);

	if (m_bPlayOnceYet == false)
	{
		Set_Weapon_PlayState(ENUM_TO_UINT(AnimState::STOP));
	}
}

void CWeapon::Ready_Before_Render(_float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);

	// none일때는 그리지 않음
	if (m_eState != State::NONE)
	{
		m_pGameInstance->Push_RenderObject(RENDER_CATEGORY::NONEBLEND, this);
		m_pGameInstance->Push_RenderObject(RENDER_CATEGORY::SHADOW_DYNAMIC, this);
	}

	// state에 따른 combineworld 업데이트
	switch (m_eState)
	{
	case State::HOLD:
	{
		if (Is_Dissolve())
			m_tDissolveDesc.Update(fTimeDelta);

		Super::Update_CombinedWorldMatrix(m_matHoldOffsetMatrix * (*m_pMatSocket) * (*m_pMatParent));
		Update_HoldingPos();
	}
		break;

	case State::HAND:
		Super::Update_CombinedWorldMatrix(m_matHandOffsetMatrix *(*m_pMatHandSocket) * (*m_pMatParent));
		break;

	case State::HAND_ONLY_POS:
	{
		Matrix matfinalMat = Matrix::Identity;
		matfinalMat.Translation((*m_pMatHandSocket).Translation());
		Super::Update_CombinedWorldMatrix(m_matHandOffsetMatrix * matfinalMat * (*m_pMatParent));
	}
		break;
	case State::HAND_ONLY_POS_SCALE:
	{
		Vec3 vPos{ Vec3::Zero };
		Vec3 vScale{ Vec3::One };
		Quat qRot{ Quat::Identity };
		Matrix* pMat = const_cast<Matrix*>(m_pMatHandSocket);
		pMat->Decompose(vScale, qRot, vPos);
		Matrix matfinalMat = Matrix::CreateScale(vScale);
		matfinalMat *= Matrix::CreateTranslation(vPos);
		Super::Update_CombinedWorldMatrix(m_matHandOffsetMatrix * matfinalMat * (*m_pMatParent));
	}
		break;

	case State::CONDEMN:
	{
		Super::Update_CombinedWorldMatrix(m_matConOffsetMatrix * (*m_pMatHandSocket) * (*m_pMatParent));
	}
	break;

	default:
		int a = 0;
	}

#ifdef _DEBUG
	m_pGameInstance->Push_DebugComponent(Get_Component<CCollider>());
#endif
}

void CWeapon::OnCollision(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther)
{
	Get_Parent()->OnCollision(iMyColliderLayer, iOtherLayer, pOther);
}

void CWeapon::OnCollision_Enter(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther, const COL_HIT_INFO& tHitInfo)
{
	Get_Parent()->OnCollision_Enter(iMyColliderLayer, iOtherLayer, pOther, tHitInfo);
}

void CWeapon::OnCollision_Exit(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther)
{
	Get_Parent()->OnCollision_Exit(iMyColliderLayer, iOtherLayer, pOther);
}

void CWeapon::OnTrigger_Enter(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther, const COL_HIT_INFO& tHitInfo)
{
	Get_Parent()->OnTrigger_Enter(iMyColliderLayer, iOtherLayer, pOther, tHitInfo);
}

void CWeapon::OnTrigger_Exit(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther)
{
	Get_Parent()->OnTrigger_Exit(iMyColliderLayer, iOtherLayer, pOther);
}

HRESULT CWeapon::Render()
{
	if (FAILED(Super::Render()))
		return E_FAIL;

	HRESULT hr = {};
	switch (m_eModleType)
	{
	case Weapon_ModelType::STATIC:
		Render_StaticWeap();
		break;

	case Weapon_ModelType::ANIM:
		Render_AnimWeap();
		break;
	}

	return S_OK;
}

HRESULT CWeapon::Render_Shadow()
{
	CShader* pShader = Get_Component<CShader>();
	_uint iPrevPass = pShader->Get_CurrentPass();

	_uint iShadowPass = 0;
	switch (m_eModleType)
	{
	case Weapon_ModelType::STATIC:
		iShadowPass = 15;
		break;

	case Weapon_ModelType::ANIM:
		iShadowPass = 4;
		break;
	}

	// Set Shadow Pass
	pShader->Set_Pass(iShadowPass);
	CModel* pModel = Get_Component<CModel>();
	_uint iMeshCount = pModel->Get_MeshCount();
	pShader->Bind_TransformData(m_matCombinedWorld);
	for (_uint i = 0; i < iMeshCount; ++i)
	{
		if(m_eModleType == Weapon_ModelType::ANIM)
			pModel->Bind_Bones(pShader, i, m_pBoneMeshCS, m_pBoneCombineCS);
		pShader->Apply();
		pModel->Render(i);
	}

	pShader->Set_Pass(iPrevPass);
	return S_OK;
}

void CWeapon::Change_WeaponAnim(_uint iAnimIdx, _bool bLoop, _bool bForce, _bool bBlend)
{
	// 우선 weapon은 blend 안 한다 생각하고 진행 : todo blend 필요하다면 바꿔야함
	Get_Component<CModel>()->Change_Animation(m_pAnimECS, iAnimIdx, bBlend, bLoop, bForce);
}

void CWeapon::Set_HandSocket()
{
	if (m_eState == State::HAND)
		return;

	m_eState = State::HAND;
}

void CWeapon::Set_DefaultSocket()
{
	if (m_eState == State::NONE)
		return;

	m_eState = State::NONE;
}

void CWeapon::Set_WeaponState(State eState)
{
	if (m_eState != eState)
	{
		m_eState = eState;

		(m_eState == State::NONE) ? Set_Active(false) : Set_Active(true);

		switch (m_eState)
		{
		case State::HOLD:
		{
			if (Is_Dissolve())
			{
				DissolveStart();
			}

		}
		break;

		default:
			m_tDissolveDesc.Reset();
		}
	}
}

void CWeapon::Set_WeaponState(_uint iState)
{
	State eNewState = static_cast<State>(iState);

	if (m_eState != eNewState)
	{
		m_eState = eNewState;

		(m_eState == State::NONE) ? Set_Active(false) : Set_Active(true);

		switch (m_eState)
		{
		case State::HOLD:
		{
			if (Is_Dissolve())
			{
				DissolveStart();
			}

		}
		break;

		default:
			m_tDissolveDesc.Reset();
		}
	}
}

_int CWeapon::Get_AnimationIndex(const wstring& wstrName)
{
	if (CModel* pModel = Get_Component<CModel>())
	{
		return pModel->Get_AnimationIndex(wstrName);
	}
	return -1;
}

HRESULT CWeapon::Ready_Components(WEAPON_DESC* pDesc)
{
	if (FAILED(Add_Component<CModel>(0/*static*/, pDesc->wstrModelPrototypeName, nullptr)))
		return E_FAIL;

	// model과 desc 정보 다를때를 위한 방어
	if (Get_Component<CModel>()->Get_Type() == EModelType::STATIC)
		m_eModleType = Weapon_ModelType::STATIC;

	if (m_eModleType == Weapon_ModelType::STATIC)
	{
		if (FAILED(Add_Component<CShader>(0/*static*/, L"Prototype_Component_Shader_VtxMesh", nullptr)))
			return E_FAIL;
	}
	else
	{
		if (FAILED(Add_Component<CShader>(0/*static*/, L"Prototype_Component_Shader_VtxAnimMesh", nullptr)))
			return E_FAIL;
	}


	if (Engine_Utils::Has_Flag(m_FDescFlags, WeaponDescFlag::WF_RGBMappingOn))
	{
		_uint iPass = 0;
		if (m_eModleType == Weapon_ModelType::STATIC)
			iPass = ENUM_TO_UINT(EMapObjectShaderPass::RGBMapping);
		else
			iPass = 2;

		Get_Component<CShader>()->Set_Pass(iPass);
		m_tColorDesc.vColorR = pDesc->vColorR;
		m_tColorDesc.vColorG = pDesc->vColorG;
		m_tColorDesc.vColorB = pDesc->vColorB;
	}
	return S_OK;
}

HRESULT CWeapon::Bind_ShaderResources()
{
	return S_OK;
}

HRESULT CWeapon::Ready_ComputeShaders()
{
	_uint iBoneNums = Get_Component<CModel>()->Get_BoneCount();
	_uint iGetBoneNums = Get_Component<CModel>()->Get_StageBoneCount();
	// ========   Compute Shader : BoneMesh  ========
	{
		CComputeShader::ComShaderCopyDesc ShaderDesc = {};
		ShaderDesc.Output_SRVBuffer_Name = "BONEFNIMAL_TRANSFORMS_SRV";

		ShaderDesc.InputBufferNum = 2;
		ShaderDesc.bMakeSB = false;
		//// 입력 버퍼
		//ShaderDesc.Input_StructBuffer.sBufferName = "IMMU_BONEDATA";
		//ShaderDesc.Input_StructBuffer.iElementSize = sizeof(CS_IMMU_BONE);
		//ShaderDesc.Input_StructBuffer.iNumElements = iBoneNums;

		// 출력 버퍼
		ShaderDesc.OutPut_StructBuffer.sBufferName = "BONEFNIMAL_TRANSFORMS";
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

		if (FAILED(Add_Script_Component(L"ComputeShader_AnimE", L"Prototype_Component_Shader_AnimEv", &ShaderDesc, CAST_VOID_PP(&m_pAnimECS))))
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

		if (FAILED(Add_Script_Component(L"ComputeShader_AnimB", L"Prototype_Component_Shader_AnimB", &ShaderDesc, CAST_VOID_PP(&m_pAnimBlendECS))))
			return E_FAIL;
	}

	if (FAILED(Get_Component<CModel>()->Ready_ComputeShaders(m_pBoneMeshCS, m_pBoneCombineCS, m_pAnimECS, m_pAnimBlendECS)))
		return E_FAIL;

	return S_OK;
}

HRESULT CWeapon::Ready_DissolveEffect_Setting(WEAPON_DESC* pDesc)
{
	using DS = DissolveEffectDesc;
	m_tDissolveDesc.Reset();

	m_tDissolveDesc.Add_DissolveFlag(DS::BIT_SPAWN_START,DS::BIT_USE_DISSOLVE_MAP, DS::BIT_USE_EDGE);
	m_tDissolveDesc.Set_Spawn_Setting(pDesc->vDissolveValues.x, pDesc->vDissolveValues.y);

	m_tDissolveDesc.Set_ObjectType(DS::DISSOLVE_OBJECTTYPE::TYPE_SWORD);

	// 스폰 시간 & 디졸브 시간
	m_tDissolveDesc.ShaderData.fDissolveEdgeColor = pDesc->vDissolveColor;
	m_tDissolveDesc.ShaderData.fDissolveEdgeWidth = pDesc->vDissolveValues.z;


	// 디졸브 셰이더 
	m_pGameInstance->Bind_DissolveTexture(Get_Component<CShader>());

	return S_OK;
}

HRESULT CWeapon::Render_StaticWeap()
{
	CShader* pShader = Get_Component<CShader>();
	CModel* pModel = Get_Component<CModel>();
	_uint iMeshCount = pModel->Get_MeshCount();

	// rgb mapping
	if (Engine_Utils::Has_Flag(m_FDescFlags, WeaponDescFlag::WF_RGBMappingOn))
	{
		pShader->Bind_RGBColorData(m_tColorDesc);
	}

	// 디졸브 data 넘겨주기
	if(Is_Dissolve())
		pShader->Bind_DissolveEffectData(m_tDissolveDesc.ShaderData);

	pShader->Bind_ObjectInfoData(m_tObjectInfoDesc);
	pShader->Bind_TransformData(m_matCombinedWorld);
	for (_uint i = 0; i < iMeshCount; ++i)
	{
		pModel->Bind_Material(pShader, i);
		pModel->Bind_MaterialInstance(pShader, i);
		pShader->Apply();
		pModel->Render(i);
	}

	// 디졸브 바인딩 값 초기화
	if (Is_Dissolve())
	{
		SHADER_DISSOLVE_EFFECT_DESC Desc = {};
		pShader->Bind_DissolveEffectData(Desc);
	}


	return S_OK;
}

HRESULT CWeapon::Render_AnimWeap()
{
	CShader*			pShader			= Get_Component<CShader>();
	CModel*				pModel			= Get_Component<CModel>();
	_uint				iMeshCount		= pModel->Get_MeshCount();

	if (Engine_Utils::Has_Flag(m_FDescFlags, WeaponDescFlag::WF_RGBMappingOn))
	{
		pShader->Bind_RGBColorData(m_tColorDesc);
	}

	// 디졸브 data 넘겨주기
	if (Is_Dissolve())
	{
		pShader->Bind_DissolveEffectData(m_tDissolveDesc.ShaderData);
	}

	pShader->Bind_ObjectInfoData(m_tObjectInfoDesc);
	pShader->Bind_TransformData(m_matCombinedWorld);
	for (_uint i = 0; i < iMeshCount; ++i)
	{
		pModel->Bind_Material(pShader, i);
		pModel->Bind_Bones(pShader, i, m_pBoneMeshCS, m_pBoneCombineCS);
		pShader->Apply();
		pModel->Render(i);
	}

	// 디졸브 바인딩 값 초기화
	if (Is_Dissolve())
	{
		SHADER_DISSOLVE_EFFECT_DESC Desc = {};
		pShader->Bind_DissolveEffectData(Desc);
	}

	return S_OK;
}

void CWeapon::DissolveStart()
{
	using DS = DissolveEffectDesc;

	//if (m_tDissolveDesc.Is_Finished())
	{
		m_tDissolveDesc.Reset();

		// reset시 flag 초기화 함 -> start 할때마다 넣어줘야함
		m_tDissolveDesc.Add_DissolveFlag(DS::BIT_SPAWN_START, DS::BIT_USE_DISSOLVE_MAP, DS::BIT_USE_EDGE);
	}
}

_bool CWeapon::Is_Dissolve()
{
	return Engine_Utils::Has_Flag(m_FDescFlags, WeaponDescFlag::WF_Dissolve);
}

void CWeapon::Play_Anim(const _float fTimeDelta)
{
	if (m_eState != State::NONE)
	{
		switch (m_eAnimState)
		{
		case AnimState::PLAY:
			Get_Component<CModel>()->Update_Animation(m_pBoneCombineCS, m_pAnimECS, fTimeDelta,nullptr,nullptr, m_pAnimBlendECS);
			break;

		case AnimState::STOP:
			Get_Component<CModel>()->Update_Animation(m_pBoneCombineCS, m_pAnimECS, 0.f, nullptr, nullptr, m_pAnimBlendECS);
			break;

		case AnimState::PLAY_ONCE:
			Get_Component<CModel>()->Update_Animation(m_pBoneCombineCS, m_pAnimECS, 0.01f, nullptr, nullptr, m_pAnimBlendECS);
			m_bPlayOnceYet = false;
			break;
		}
	}
}

void CWeapon::Update_HoldingPos()
{
	// 현재 com transform 기준으로 부터 약간 뒤로, 약간 위로

	Vec3 vLook = m_matCombinedWorld.Backward();
	Vec3 vUp = m_matCombinedWorld.Left();
	Vec3 vPos = m_matCombinedWorld.Translation();

	Vec3 vMove = (vLook * 0.4f) + (vUp * 0.2f);

	m_matCombinedWorld.Translation(vPos + vMove);
}

void CWeapon::Free()
{
	Super::Free();
}

