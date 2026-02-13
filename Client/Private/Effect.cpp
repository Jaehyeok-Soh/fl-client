#include "pch.h"
#include "Effect.h"
#include "DataStruct_Effect.h"
#include "DataDocument_Effect.h"
#include "EffectObject.h"
#include "Engine_Utils.h"

#define MAX_EFFECTPART 10

Effect::Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	:CContainerObject(pDevice, pDeviceContext)
{
	m_vecPartObjects.resize(MAX_EFFECTPART, nullptr);
}

Effect::Effect(const Effect& rhs)
	:CContainerObject(rhs)
{
	m_vecPartObjects.resize(MAX_EFFECTPART, nullptr);
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

	m_eSimulationSpace = pDesc->_Effect_SimulationType;
	auto& ChildDataList = pDesc->_childData; 

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
		tObjDesc.pMatParent = &(Get_Component<CTransform>()->Get_WorldMatrix());
		tObjDesc.pTransform_Desc = &transformDesc;
		tObjDesc.iLevelIndex = pDesc->iLevelIndex;

		// Part 추가 및 이름 설정
		if (FAILED(Add_Part(index, pDesc->iLevelIndex, L"Prototype_GameObject_Effect_Parts", &tObjDesc)))
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

	if (m_eSimulationSpace == DTO::E_SIMULATION_SPACE::LOCAL && m_pParentsWorldMatrix != nullptr)
	{
		Update_CombinedWorldMatrix(m_pParentsWorldMatrix);
	}
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
	return false;
}

void Effect::Update_CombinedWorldMatrix(const Matrix* pMatParent)
{
	m_matCombinedWorld = Get_Component<CTransform>()->Get_WorldMatrix() * (*pMatParent);
}


void Effect::Set_Dead(const wstring& wstrLayerTag)
{
}

HRESULT Effect::Spawn_FromPool(void* pArg)
{
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

Effect* Effect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	Effect* pInstance = new Effect(pDevice, pDeviceContext);

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
