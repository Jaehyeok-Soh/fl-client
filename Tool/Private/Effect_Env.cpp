#include "pch.h"
#include "Effect_Env.h"
#include "CEffectObject.h"
#include "GameInstance.h"

CEffect_Env::CEffect_Env(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	:Super(EToolObjectType::MESHEFFECT, pDevice, pDeviceContext)
{

}

CEffect_Env::CEffect_Env(const CEffect_Env& rhs)
	:Super(rhs)
{

}

HRESULT CEffect_Env::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CEffect_Env::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CEffect_Env::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;

	return S_OK;
}

void CEffect_Env::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CEffect_Env::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
}

void CEffect_Env::Update_Late(const _float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);
}

void CEffect_Env::Ready_Before_Render(const _float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);
}

HRESULT CEffect_Env::Render()
{
	Super::Render();

	return S_OK;
}

void CEffect_Env::Update_CombinedWorldMatrix()
{
	using namespace DirectX::SimpleMath;

	Quaternion rotation = Quaternion::CreateFromYawPitchRoll(m_tEnvDesc.VFX_Rotation);

	m_pOffsetMartix = Matrix::CreateScale(m_tEnvDesc.VFX_Scale) * Matrix::CreateFromQuaternion(rotation) * Matrix::CreateTranslation(m_tEnvDesc.VFX_Target_Position);

	if (m_eDesc._Effect_SimulationType == DTO::E_SIMULATION_SPACE::LOCAL)
	{
		if (m_pBoneMatrix != nullptr)
		{
			Update_Bone_Attached_Matrix();
		}
		
		else if (m_pBoneOwnerMatrix != nullptr)
		{
			m_matCombinedWorld = m_pOffsetMartix * (*m_pBoneOwnerMatrix);
		}
	}

	else
	{
		// WORLD일 때는 계산된 Offset 자체가 월드 행렬이 됨
		m_matCombinedWorld = m_pOffsetMartix;
	}
}

void CEffect_Env::Spawn_PartsSetting(void* pArg)
{
	EFFECT_ENV_DESC* pDesc = static_cast<EFFECT_ENV_DESC*>(pArg);
	if (pDesc == nullptr) return;
	if (pDesc->VFX_PartsDescList.size() == 0) return;
	
	_uint PartSize = (_uint)m_vecPartObjects.size();

	for (auto& Parts : pDesc->VFX_PartsDescList)
	{
		if (Parts.iPartsIndex >= PartSize) continue;

		CEffectObject* pObject = static_cast<CEffectObject*>(m_vecPartObjects[Parts.iPartsIndex]);
		pObject->Overwrite_FromEnv(Parts);
	}
}

void CEffect_Env::Spawn_PositionCalculate(void* pArg)
{
	EFFECT_ENV_DESC* pDesc = static_cast<EFFECT_ENV_DESC*>(pArg);
	if (pDesc == nullptr) return;

	Matrix WorldMatrix = WorldMatrix_Calculate(pDesc);
	Get_Component<CTransform>()->Set_WorldMatrix(WorldMatrix);

	pDesc->VFX_Color = {};

	// 타이머 및 자식들 초기화
	for (auto effectObject : m_vecPartObjects)
	{
		if (effectObject != nullptr)
			effectObject->Spawn_FromPool(pArg);
	}
}

Matrix CEffect_Env::WorldMatrix_Calculate(const EFFECT_ENV_DESC* pArg)
{
	using namespace DirectX::SimpleMath;

	Quaternion rotation = Quaternion::CreateFromYawPitchRoll(pArg->VFX_Rotation);
	return (Matrix::CreateScale(pArg->VFX_Scale) * Matrix::CreateFromQuaternion(rotation) * Matrix::CreateTranslation(pArg->VFX_Target_Position));
}

HRESULT CEffect_Env::Spawn_FromPool(void* pArg)
{
	if (nullptr == pArg) return E_FAIL;

	if (FAILED(Super::Spawn_FromPool(pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CEffect_Env::Enable_VFX(void* pArg)
{
	if (nullptr == pArg) return E_FAIL;

	m_bIsEffectFinish = false;

	EFFECT_ENV_DESC* pDesc = static_cast<EFFECT_ENV_DESC*>(pArg);
	m_tEnvDesc = *pDesc;

	m_eDesc._Effect_SimulationType = (DTO::E_SIMULATION_SPACE)pDesc->iSimulationType;
	m_tEnvDesc.VFX_COLORTYPE = pDesc->VFX_COLORTYPE;

	if (m_tEnvDesc.VFX_COLORTYPE == EFFECT_ENV_DESC::E_VFX_COLORMODE::COLOR_CHANGE)
		m_tEnvDesc.VFX_Color = pDesc->VFX_Color;

	// 부모 행렬 포인터 연결
	if (pDesc->pTransformMatrix)
		m_pBoneOwnerMatrix = *pDesc->pTransformMatrix;

	if (pDesc->pTargetBoneMatrix)
		m_pBoneMatrix = *pDesc->pTargetBoneMatrix;

	m_iBoneFlag = pDesc->iBoneFlag;

	Update_CombinedWorldMatrix();

	for (auto effectObject : m_vecPartObjects)
	{
		if (effectObject != nullptr)
			static_cast<CEffectObject*>(effectObject)->Enable_VFX(pArg);
	}

	Spawn_PartsSetting(pArg);

	return S_OK;
}

HRESULT CEffect_Env::Disable_VFX()
{
	m_bIsEffectFinish = false;

	for (auto effectObject : m_vecPartObjects)
	{
		if (effectObject != nullptr)
			static_cast<CEffectObject*>(effectObject)->Disable_VFX();
	}

	return S_OK;
}

CEffect_Env* CEffect_Env::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CEffect_Env* pInstance = new CEffect_Env(pDevice, pDeviceContext);

	if (pInstance == nullptr)
	{
		MSG_BOX("Create to Fail : CEffect_Env");
		Safe_Release(pInstance);
		return nullptr;
	}

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Initialize to Fail : CEffect_Env");
		return nullptr;
	}

	return pInstance;
}

CGameObject* CEffect_Env::Clone(void* pArg)
{
	CEffect_Env* pClone = new CEffect_Env(*this);
	if (FAILED(pClone->Initialize(pArg)))
	{
		MSG_BOX("CEffect_Env::Clone, Failed");
		Safe_Release(pClone);
	}
	return pClone;
}

void CEffect_Env::Free()
{
	Super::Free();
}