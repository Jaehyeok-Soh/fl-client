#include "Engine_pch.h"
#include "Bounds.h"
#include "Bounding_Sphere.h"
#include "Bounding_AABB.h"
#include "GameInstance.h"

CBounds::CBounds(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: Super()
	, m_pDevice(pDevice)
	, m_pDeviceContext(pDeviceContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);
}

CBounds::CBounds(const CBounds& rhs)
	: Super(rhs)
	, m_pDevice(rhs.m_pDevice)
	, m_pDeviceContext(rhs.m_pDeviceContext)
#ifdef _DEBUG
	, m_pBatch(rhs.m_pBatch)
	, m_pEffect(rhs.m_pEffect)
	, m_pInputLayout(rhs.m_pInputLayout)
#endif
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);
#ifdef _DEBUG
	Safe_AddRef(m_pInputLayout);
#endif
}

HRESULT CBounds::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;
#ifdef _DEBUG
	m_pBatch = new PrimitiveBatch<VertexPositionColor>(m_pDeviceContext);
	m_pEffect = new BasicEffect(m_pDevice);
	m_pEffect->SetVertexColorEnabled(true);

	const void* pShaderInput = { nullptr };
	size_t iShaderInputLenght = {};
	m_pEffect->GetVertexShaderBytecode(&pShaderInput, &iShaderInputLenght);

	if (FAILED(m_pDevice->CreateInputLayout(
		VertexPositionColor::InputElements
		, VertexPositionColor::InputElementCount
		, pShaderInput
		, iShaderInputLenght
		, &m_pInputLayout)))
		return E_FAIL;
#endif
	return S_OK;
}

HRESULT CBounds::Initialize(void* pArg)
{
	if (pArg == nullptr)
		return E_FAIL;

	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	BOUND_COMP_DESC* pDesc = static_cast<BOUND_COMP_DESC*>(pArg);
	if (pDesc->pMinMax == nullptr)
		return E_FAIL;

	Vec3 vFinalMinMax[2]{ (pDesc->pMinMax[0]) * pDesc->fRatio ,
						(pDesc->pMinMax[1]) * pDesc->fRatio };

	{
		CBounding_AABB::BOUNDING_AABB_DESC desc{};
		desc.pMinMax = vFinalMinMax;
		m_tBounds.pAABB = CBounding_AABB::Create(m_pDevice, m_pDeviceContext, &desc);
	}
	if (m_tBounds.pAABB == nullptr)
		return E_FAIL;

	{
		vFinalMinMax[0] *= 1.3f;
		vFinalMinMax[1] *= 1.3f;
		CBounding_Sphere::BOUNDING_SPHERE_DESC desc{};
		desc.pMinMax = vFinalMinMax;
		m_tBounds.pSphere = CBounding_Sphere::Create(m_pDevice, m_pDeviceContext, &desc);
	}
	if (m_tBounds.pSphere == nullptr)
		return E_FAIL;

	return S_OK;
}

void CBounds::Update_BoundingDesc(const Matrix& matWorld)
{
	if (m_tBounds.pAABB != nullptr && m_tBounds.pSphere != nullptr)
	{
		m_tBounds.pAABB->Update(matWorld);
		m_tBounds.pSphere->Update(matWorld);
	}
}

_bool CBounds::IntersectWith_Frustrum(BoundingFrustum* pFrustrum)
{
	if (pFrustrum->Contains(*m_tBounds.pSphere->Get_Desc()) == ContainmentType::DISJOINT)
		return false;

	if (pFrustrum->Contains(*m_tBounds.pAABB->Get_Desc()) == ContainmentType::DISJOINT)
		return false;

	return true;
}

_bool CBounds::IntersectWithRay_World(OUT Vec3& vOut)
{
	if (m_tBounds.pSphere->IntersectWithRay_World(m_pGameInstance, vOut) == false)
		return false;

	if (m_tBounds.pAABB->IntersectWithRay_World(m_pGameInstance, vOut) == false)
		return false;

	return true;
}

_bool CBounds::IntersectWithRay_Local(OUT Vec3& vOut)
{
	if (m_tBounds.pSphere->IntersectWithRay_Local(m_pGameInstance, vOut) == false)
		return false;

	if (m_tBounds.pAABB->IntersectWithRay_Local(m_pGameInstance, vOut) == false)
		return false;

	return true;
}

BoundingBox* CBounds::Get_WolrdAABB()
{
	if (m_tBounds.pAABB == nullptr)
		return nullptr;
		
	return m_tBounds.pAABB->Get_Desc();
}


CBounds* CBounds::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CBounds* pInstance = new CBounds(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CBounds::Create, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CBounds::Clone(void* pArg)
{
	CBounds* pInstance = new CBounds(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CBounds::Clone, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBounds::Free()
{
#ifdef _DEBUG
	if (IsClone() == false)
	{
		Safe_Delete(m_pBatch);
		Safe_Delete(m_pEffect);
	}
	Safe_Release(m_pInputLayout);
#endif
	Safe_Release(m_tBounds.pSphere);
	Safe_Release(m_tBounds.pAABB);
	Safe_Release(m_pDevice);
	Safe_Release(m_pDeviceContext);
	Super::Free();
}

#ifdef _DEBUG
void CBounds::Render()
{
	m_pDeviceContext->CSSetShader(nullptr, nullptr, 0);
	m_pDeviceContext->GSSetShader(nullptr, nullptr, 0);
	m_pEffect->SetWorld(Matrix::Identity);
	m_pEffect->SetView(m_pGameInstance->Get_ViewMatrix());
	m_pEffect->SetProjection(m_pGameInstance->Get_ProjMatrix());

	m_pEffect->Apply(m_pDeviceContext);
	m_pDeviceContext->IASetInputLayout(m_pInputLayout);

	m_pBatch->Begin();
	m_tBounds.pAABB->Render(m_pBatch, false);
	m_pBatch->End();
}
#endif