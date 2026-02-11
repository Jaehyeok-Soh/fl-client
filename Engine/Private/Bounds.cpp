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


	if (!(m_tBounds.pAABB = Create_AABB(vFinalMinMax)))
		return E_FAIL;

	vFinalMinMax[0] *= 1.3f;
	vFinalMinMax[1] *= 1.3f;

	if (!(m_tBounds.pSphere = Create_Sphere(vFinalMinMax)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBounds::Add_SubBounds(const Vec3* pMinMax, span<Matrix> spanInstanceMatrix, _float fRatio)
{
	m_vecSubBounds.resize(spanInstanceMatrix.size());

	Vec3 vAABBMinMax[2]{ (pMinMax[0]) * fRatio , (pMinMax[1]) * fRatio };
	Vec3 vSphereMinMax[2]{ vAABBMinMax[0] *= 1.3f , vAABBMinMax[1] *= 1.3f };

	for (size_t i = 0; i < spanInstanceMatrix.size(); ++i)
	{
		if (!(m_vecSubBounds[i].pAABB = Create_AABB(vAABBMinMax)))
			return E_FAIL;

		if (!(m_vecSubBounds[i].pSphere = Create_Sphere(vSphereMinMax)))
			return E_FAIL;

		m_vecSubBounds[i].pAABB->Update(spanInstanceMatrix[i]);
		m_vecSubBounds[i].pSphere->Update(spanInstanceMatrix[i]);
	}

	return S_OK;
}

HRESULT CBounds::Update_SubBound(const  Vec3* pTotalMinMax , const Matrix& WorldMatrix, _uint iIndex)
{

	if (pTotalMinMax == nullptr) return E_FAIL;
	if (m_vecSubBounds.size() < iIndex) return E_FAIL;

	m_vecSubBounds[iIndex].pAABB->Update(WorldMatrix);
	m_vecSubBounds[iIndex].pSphere->Update(WorldMatrix);



	/* 이미 추가된 Bounds */
	if(FAILED(Update_Bounds(pTotalMinMax)))
		return E_FAIL;
	

	return S_OK;
}

/* Update까지 바로된다 */
HRESULT CBounds::Push_SubBounds(const  Vec3* pTotalMinMax , const   Vec3* pModelMinMax  , const Matrix& WorldMatrix , float fRatio)
{
	if (pTotalMinMax == nullptr) return E_FAIL;
	if (pModelMinMax == nullptr) return E_FAIL;

	Vec3 vAABBMinMax[2]{ (pModelMinMax[0]) * fRatio , (pModelMinMax[1]) * fRatio };
	Vec3 vSphereMinMax[2]{ vAABBMinMax[0] *= 1.3f , vAABBMinMax[1] *= 1.3f };

	MESH_BOUNDS tMeshBounds{ Create_AABB(vAABBMinMax) , Create_Sphere(vSphereMinMax)};
	m_vecSubBounds.push_back(tMeshBounds);
	if (FAILED(CBounds::Update_SubBound(pTotalMinMax, WorldMatrix, static_cast<_uint>(m_vecSubBounds.size() - 1))))
		return E_FAIL;

	return S_OK;
}

HRESULT CBounds::Delete_SubBounds(const Vec3* pTotalMinMax, _uint iDeleteIndex , float fRatio)
{
	if (pTotalMinMax == nullptr) return E_FAIL;
	if (iDeleteIndex > m_vecSubBounds.size()) return E_FAIL;

	m_vecSubBounds.erase(m_vecSubBounds.begin() + iDeleteIndex);

	if (FAILED(Update_Bounds( pTotalMinMax , fRatio)))
		return E_FAIL;

	return S_OK;
}


HRESULT CBounds::Update_Bounds(const Vec3* pMinMax, float fRatio)
{
	Vec3 vFinalMinMax[2]{ (pMinMax[0]) * fRatio , (pMinMax[1]) * fRatio };

	Safe_Release(m_tBounds.pAABB);
	if (!(m_tBounds.pAABB = Create_AABB(vFinalMinMax)))
		return E_FAIL;

	vFinalMinMax[0] *= 1.3f;
	vFinalMinMax[1] *= 1.3f;

	Safe_Release(m_tBounds.pSphere);
	if (!(m_tBounds.pSphere = Create_Sphere(vFinalMinMax)))
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

void CBounds::IntersectWith_Frustrum_SubBounds(BoundingFrustum* pFrustrum, OUT vector<_uint>& vecVisibleIndex)
{
	vecVisibleIndex.clear();

	for (_uint i = 0; i < (_uint)m_vecSubBounds.size(); ++i)
	{
		if (pFrustrum->Contains(*m_vecSubBounds[i].pSphere->Get_Desc()) == ContainmentType::DISJOINT)
			continue;

		if (pFrustrum->Contains(*m_vecSubBounds[i].pAABB->Get_Desc()) == ContainmentType::DISJOINT)
			continue;

		vecVisibleIndex.push_back(i);
	}
}

_bool CBounds::IntersectWithRay_World(OUT Vec3& vOut, OUT _int &iIndex)
{
	if (m_tBounds.pSphere->IntersectWithRay_World(m_pGameInstance, vOut) == false)
		return false;

	if (m_tBounds.pAABB->IntersectWithRay_World(m_pGameInstance, vOut) == false)
		return false;

	for (size_t i = 0; i < m_vecSubBounds.size(); ++i)
	{
		auto& Element = m_vecSubBounds[i];

		if (Element.pSphere->IntersectWithRay_World(m_pGameInstance, vOut) == false)
			continue;

		if (Element.pAABB->IntersectWithRay_World(m_pGameInstance, vOut) == false)
			continue;

		iIndex = static_cast<_int>(i);
		return true;
	}

	iIndex = 0;
	return true;
}

_bool CBounds::IntersectWithRay_Local(OUT Vec3& vOut, OUT _int& iIndex)
{
	if (m_tBounds.pSphere->IntersectWithRay_Local(m_pGameInstance, vOut) == false)
		return false;

	if (m_tBounds.pAABB->IntersectWithRay_Local(m_pGameInstance, vOut) == false)
		return false;

	for (size_t i = 0; i < m_vecSubBounds.size(); ++i)
	{
		auto& Element = m_vecSubBounds[i];

		if (Element.pSphere->IntersectWithRay_World(m_pGameInstance, vOut) == false)
			continue;

		if (Element.pAABB->IntersectWithRay_World(m_pGameInstance, vOut) == false)
			continue;

		iIndex = static_cast<_int>(i);
		return true;
	}

	iIndex = 0;
	return true;
}

BoundingBox* CBounds::Get_WolrdAABB()
{
	if (m_tBounds.pAABB == nullptr)
		return nullptr;
		
	return m_tBounds.pAABB->Get_Desc();
}

CBounding_Sphere* CBounds::Create_Sphere(Vec3* pMinMax)
{
	CBounding_Sphere* pSphere{ nullptr };
	CBounding_Sphere::BOUNDING_SPHERE_DESC desc{};
	desc.pMinMax = pMinMax;
	pSphere = CBounding_Sphere::Create(m_pDevice, m_pDeviceContext, &desc);
	return pSphere;
}

CBounding_AABB* CBounds::Create_AABB(Vec3* pMinMax)
{
	CBounding_AABB* pAABB{ nullptr };
	CBounding_AABB::BOUNDING_AABB_DESC desc{};
	desc.pMinMax = pMinMax;
	pAABB = CBounding_AABB::Create(m_pDevice, m_pDeviceContext, &desc);
	return pAABB;
}

void CBounds::Clear_SubBounds()
{
	for (auto& pElement : m_vecSubBounds)
	{
		Safe_Release(pElement.pAABB);
		Safe_Release(pElement.pSphere);
	}
	m_vecSubBounds.clear();
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
	Clear_SubBounds();
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
	m_tBounds.pAABB->Render(m_pBatch, true);
	//m_tBounds.pSphere->Render(m_pBatch, true);
	for (size_t i = 0; i < m_vecSubBounds.size(); ++i)
	{
		m_vecSubBounds[i].pAABB->Render(m_pBatch, false);
		m_vecSubBounds[i].pSphere->Render(m_pBatch, false);
	}
	m_pBatch->End();
}
#endif