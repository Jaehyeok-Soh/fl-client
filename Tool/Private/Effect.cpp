#include "pch.h"
#include "Effect.h"
#include "DataStruct_Effect.h"
#include "DataDocument_Effect.h"
#include "CEffectObject.h"
#include "DataStruct_EffectEvent.h"
#include "Engine_Utils.h"

#define MAX_EFFECTPART 10

Effect::Effect(EToolObjectType eType, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	:Super(eType, pDevice, pDeviceContext)
{
}

Effect::Effect(const Effect& rhs)
	:Super(rhs)
{
}

HRESULT Effect::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT Effect::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_PartsData(pArg)))
		return E_FAIL;

	return S_OK;
}


HRESULT Effect::Ready_PartsData(void* pArg)
{
	EFFECT_CONTAINERDESC* pDesc = static_cast<EFFECT_CONTAINERDESC*>(pArg);
	if (pDesc == nullptr) return E_FAIL;

	m_eDesc._Effect_SimulationType = pDesc->_Effect_SimulationType;
	auto& ChildDataList = pDesc->_childData;

	m_vecPartObjects.resize(ChildDataList.size(), nullptr);

	_uint index = 0;
	for (DTO::TEFFECT_PartsData& Part : ChildDataList)
	{
		if (index >= MAX_EFFECTPART) break;

		// Transform 데이터 복원
		Vec3 vScale, vPos;
		Quat vQuat;
		Part.vWorldMatrix.Decompose(vScale, vQuat, vPos);

		// 2. Desc 준비
		CEffectObject::Effect_Desc tObjDesc = {};

		// 구조체 전체 복사 (모든 DTO 데이터가 한 번에 복사됨)
		tObjDesc.Data = Part;

		// Transform 설정
		CTransform::TRANSFORM_DESC transformDesc = {};
		transformDesc.ScaleMatrix = Matrix::CreateScale(vScale);
		transformDesc.RotationMatrix = Matrix::CreateFromQuaternion(vQuat);
		transformDesc.TranslationMatrix = Matrix::CreateTranslation(vPos);
		transformDesc.fRotatePerSec = 1.f;
		transformDesc.fMovePerSec = 1.f;

		// 부모 행렬 및 기본 정보 설정
		tObjDesc.pMatParent = &m_matCombinedWorld;
		tObjDesc.pTransform_Desc = &transformDesc;
		tObjDesc.iLevelIndex = pDesc->iLevelIndex;

		// Part 추가 및 이름 설정
		if (FAILED(Add_Part(index, pDesc->iLevelIndex, L"Prototype_GameObject_Effect_Part_Particle", &tObjDesc)))
			return E_FAIL;

		auto pPartObject = Get_Part<CEffectObject>(index);
		if (pPartObject != nullptr)
		{
			pPartObject->Set_Name(Part.EffectPartsName);
		}
		index++;
	}

	return S_OK;
}


HRESULT Effect::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;

	return S_OK;
}

void Effect::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void Effect::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);

	if (m_eDesc._Effect_SimulationType == DTO::E_SIMULATION_SPACE::LOCAL && m_pBoneMatrix != nullptr)
	{

		Matrix matBone = *m_pBoneMatrix;
		Matrix matCustom = XMMatrixIdentity();

		Vector3 vBoneScale;
		Quat vBoneQuat;
		Vector3 vBonePos;

		matBone.Decompose(vBoneScale, vBoneQuat, vBonePos);

		if (Engine_Utils::Has_Flag(m_iBoneFlag, DTO::BONE_SCALE))
			matCustom *= Matrix::CreateScale(vBoneScale);

		if (Engine_Utils::Has_Flag(m_iBoneFlag, DTO::BONE_ROTATAION))
			matCustom *= Matrix::CreateFromQuaternion(vBoneQuat);

		if (Engine_Utils::Has_Flag(m_iBoneFlag, DTO::BONE_POS))
		{
			matCustom.Translation(Vec3(vBonePos.x * 0.01f, vBonePos.y * 0.01f, vBonePos.z * 0.01f));
		}

		Matrix matBoneOwner = *m_pBoneOwnerMatrix;
		Matrix matCustom2 = XMMatrixIdentity();

		Vector3 vBoneScale2;
		Quat vBoneQuat2;
		Vector3 vBonePos2;

		matBoneOwner.Decompose(vBoneScale2, vBoneQuat2, vBonePos2);
		matCustom2 *= Matrix::CreateFromQuaternion(vBoneQuat2);
		matCustom2.Translation(Vec3(vBonePos2.x, vBonePos2.y, vBonePos2.z));
		
		m_matCombinedWorld = m_pOffsetMartix * (matCustom) *(matCustom2);
	}
	else
	{
		m_matCombinedWorld = Get_Component<CTransform>()->Get_WorldMatrix();
	}

	if(m_pGameInstance->Get_CurrentLevelIndex() != (ENUM_TO_UINT(ELevelType::EFFECT)))
		IsEffectFinish();
}

void Effect::Update_Late(const _float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);
}

void Effect::Ready_Before_Render(const _float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);
}

HRESULT Effect::Render()
{
	return Super::Render();
}

_bool Effect::Picking(OUT Vec3& vOut)
{
	return Super::Picking(vOut);
}

_bool Effect::Export_Data(DTO::ECategory eCategory, CDataDocumentBase* pDocument)
{
	if (eCategory != DTO::ECategory::EFFECT)
		return false;

	CDataDocument_Effect* pEffectDoc = static_cast<CDataDocument_Effect*>(pDocument);

	// 부모(Container) 데이터 생성 및 기본 정보 기입
	DTO::TEFFECT_ContainerData tContainerData;
	tContainerData.strTag = Get_Name();
	tContainerData.EffectContainerName = tContainerData.strTag;
	tContainerData.vWorldMatrix = Get_Component<CTransform>()->Get_WorldMatrix();
	tContainerData._Effect_SimulationType = ENUM_TO_UINT(m_eDesc._Effect_SimulationType);

	// 자식(Parts) 데이터 수집
	for (auto& pPart : m_vecPartObjects)
	{
		CEffectObject* pEffectPart = dynamic_cast<CEffectObject*>(pPart);
		if (nullptr == pEffectPart) continue;

		DTO::TEFFECT_PartsData tPartData = pEffectPart->Get_EffectDesc();

		// =========== 정보 보정 (실시간 갱신이 필요한 값들) ===========
		tPartData.EffectPartsName = pEffectPart->Get_Name();
		tPartData.ParentsName = tContainerData.EffectContainerName;

		// 실시간 툴 조작으로 변한 Transform 행렬 업데이트
		tPartData.vWorldMatrix = pEffectPart->Get_Component<CTransform>()->Get_WorldMatrix();

		// 수집된 파츠 데이터를 컨테이너 리스트에 추가
		tContainerData._ChildData.push_back(tPartData);
	}

	// 부모 Document에 최종 컨테이너 추가 및 직렬화 준비
	return SUCCEEDED(pEffectDoc->Try_Add(tContainerData));
}

void Effect::Draw_ImGui()
{
	Super::Draw_ImGui();
}

void Effect::Update_CombinedWorldMatrix(const Matrix* pBoneMatrix)
{
	if(pBoneMatrix != nullptr)
		m_matCombinedWorld = m_pOffsetMartix * (*pBoneMatrix);
}


void Effect::Set_Dead(const wstring& wstrLayerTag)
{
	m_pGameInstance->Request_DeleteGameObject(0, wstrLayerTag, this);
}

void Effect::IsEffectFinish()
{
	_uint FinishCount = 0;

	for (auto Effect : m_vecPartObjects)
	{
		if (Effect)
			FinishCount += static_cast<CEffectObject*>(Effect)->IsEffectfinish();
	}

	if (FinishCount == m_vecPartObjects.size())
	{
		Set_Dead(L"Effect_Layer");
	}
}

HRESULT Effect::Spawn_FromPool(void* pArg)
{
	if (nullptr == pArg) return E_FAIL;

	Matrix matTargetWorld = XMMatrixIdentity();
	Get_Component<CTransform>()->Set_WorldMatrix(matTargetWorld);

	// Engine에서 던진 범용 Desc로 캐스팅
	EFFECT_SPAWN_DESC* pEngineDesc = static_cast<EFFECT_SPAWN_DESC*>(pArg);

	// Engine 데이터를 기반으로 Client의 데이터 갱신
	m_eDesc._Effect_SimulationType = (DTO::E_SIMULATION_SPACE)pEngineDesc->iSimulationType;
	m_pBoneMatrix = *pEngineDesc->pTargetBoneMatrix;
	m_pBoneOwnerMatrix = *pEngineDesc->pTransformMatrix;
	m_iBoneFlag = pEngineDesc->iFlag;

	if (m_eDesc._Effect_SimulationType == DTO::E_SIMULATION_SPACE::WORLD)
		Get_Component<CTransform>()->Set_WorldMatrix(pEngineDesc->matWorld);

	else if ((m_eDesc._Effect_SimulationType == DTO::E_SIMULATION_SPACE::LOCAL))
		m_pOffsetMartix = pEngineDesc->matWorld;

	// 타이머 및 자식들 초기화
	for (auto effectObject : m_vecPartObjects)
	{
		if (effectObject != nullptr)
			effectObject->Spawn_FromPool(pArg);
	}

	return S_OK;
}

HRESULT Effect::Despawn_FromPool()
{
	for (auto effectObject : m_vecPartObjects)
	{
		if (effectObject != nullptr)
			effectObject->Despawn_FromPool();
	}

	return S_OK;
}

Effect* Effect::Create(EToolObjectType eType, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	Effect* pInstance = new Effect(eType, pDevice, pDeviceContext);

	if (pInstance == nullptr)
	{
		MSG_BOX("Create to Fail : Effect");
		Safe_Release(pInstance);
		return nullptr;
	}

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Initialize to Fail : Effect");
		return nullptr;
	}

	return pInstance;
}

CGameObject* Effect::Clone(void* pArg)
{
	Effect* pClone = new Effect(*this);
	if (FAILED(pClone->Initialize(pArg)))
	{
		MSG_BOX("Effect::Clone, Failed");
		Safe_Release(pClone);
	}
	return pClone;
}

void Effect::Free()
{
	Super::Free();
}
