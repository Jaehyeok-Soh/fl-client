#include "pch.h"
#include "Effect_WarningCircle.h"
#include "EffectObject.h"
#include "GameInstance.h"

CEffect_WarningCircle::CEffect_WarningCircle(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	:Super(pDevice, pDeviceContext)
{

}

CEffect_WarningCircle::CEffect_WarningCircle(const CEffect_WarningCircle& rhs)
	:Super(rhs)
{

}

HRESULT CEffect_WarningCircle::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CEffect_WarningCircle::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CEffect_WarningCircle::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;

	return S_OK;
}

void CEffect_WarningCircle::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CEffect_WarningCircle::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
}

void CEffect_WarningCircle::Update_Late(const _float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);
}

void CEffect_WarningCircle::Ready_Before_Render(const _float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);
}

HRESULT CEffect_WarningCircle::Render()
{
	Super::Render();

	return S_OK;
}

void CEffect_WarningCircle::Spawn_PositionCalculate(void* pArg)
{
	EFFECT_WARNING_DESC* pDesc = static_cast<EFFECT_WARNING_DESC*>(pArg);
	if (pDesc == nullptr) return;

	Matrix WorldMatrix = WorldMatrix_Calculate(pDesc);
	Get_Component<CTransform>()->Set_WorldMatrix(WorldMatrix);

	// 타이머 및 자식들 초기화
	for (auto effectObject : m_vecPartObjects)
	{
		if (effectObject != nullptr)
			effectObject->Spawn_FromPool(pArg);
	}
}

Matrix CEffect_WarningCircle::WorldMatrix_Calculate(const EFFECT_WARNING_DESC* pArg)
{
	using namespace DirectX::SimpleMath;

	Quaternion rotation = Quaternion::CreateFromYawPitchRoll(pArg->VFX_Rotation);
	return (Matrix::CreateScale(pArg->VFX_Scale) * Matrix::CreateFromQuaternion(rotation) * Matrix::CreateTranslation(pArg->VFX_Target_Position));
}

HRESULT CEffect_WarningCircle::Spawn_FromPool(void* pArg)
{
	if (nullptr == pArg) return E_FAIL;

	if (FAILED(Super::Spawn_FromPool(pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CEffect_WarningCircle::Enable_VFX(void* pArg)
{
	m_bIsEffectFinish = false;

	EFFECT_WARNING_DESC* pDesc = static_cast<EFFECT_WARNING_DESC*>(pArg);
	if (pDesc == nullptr) return E_FAIL;

	Matrix WorldMatrix = WorldMatrix_Calculate(pDesc);
	Get_Component<CTransform>()->Set_WorldMatrix(WorldMatrix);

	m_eDesc._Effect_SimulationType = (DTO::E_SIMULATION_SPACE)pDesc->iSimulationType;

	// 타이머 및 자식들 초기화
	for (auto effectObject : m_vecPartObjects)
	{
		if (effectObject != nullptr)
			static_cast<CEffectObject*>(effectObject)->Enable_VFX(pArg);
	}

	return S_OK;
}

HRESULT CEffect_WarningCircle::Disable_VFX()
{
	m_bIsEffectFinish = false;

	for (auto effectObject : m_vecPartObjects)
	{
		if (effectObject != nullptr)
			static_cast<CEffectObject*>(effectObject)->Disable_VFX();
	}

	return S_OK;
}

CEffect_WarningCircle* CEffect_WarningCircle::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CEffect_WarningCircle* pInstance = new CEffect_WarningCircle(pDevice, pDeviceContext);

	if (pInstance == nullptr)
	{
		MSG_BOX("Create to Fail : CEffect_WarningCircle");
		Safe_Release(pInstance);
		return nullptr;
	}

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Initialize to Fail : CEffect_WarningCircle");
		return nullptr;
	}

	return pInstance;
}

CGameObject* CEffect_WarningCircle::Clone(void* pArg)
{
	CEffect_WarningCircle* pClone = new CEffect_WarningCircle(*this);
	if (FAILED(pClone->Initialize(pArg)))
	{
		MSG_BOX("CEffect_WarningCircle::Clone, Failed");
		Safe_Release(pClone);
	}
	return pClone;
}

void CEffect_WarningCircle::Free()
{
	Super::Free();
}