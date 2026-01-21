#include "Bounding_AABB.h"
#include "Bounding_OBB.h"
#include "Bounding_Sphere.h"
#include "GameObject.h"
#include "GameInstance.h"
#include "Collider.h"

_uint CCollider::s_iNextID = { 0 };

CCollider::CCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, EColliderType eType)
	: Super()
	, m_pDevice(pDevice)
	, m_pDeviceContext(pDeviceContext)
	, m_eType(eType)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);
}

CCollider::CCollider(const CCollider& rhs)
	: Super(rhs)
	, m_iID(s_iNextID++)
	, m_pDevice(rhs.m_pDevice)
	, m_pDeviceContext(rhs.m_pDeviceContext)
	, m_eType(rhs.m_eType)
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

HRESULT CCollider::Initialize_Prototype()
{
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

HRESULT CCollider::Initialize(void* pArg)
{
	if (!pArg)
		return E_FAIL;

	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	COLLIDER_DESC* pDesc = static_cast<COLLIDER_DESC*>(pArg);
	if (!pDesc->pBoundingDesc)
		return E_FAIL;

	m_iLayer = pDesc->iLayer;
	if (FAILED(Create_Bounding(pDesc->pBoundingDesc)))
		return E_FAIL;

	return S_OK;
}

void CCollider::Update(_fmatrix matWorld)
{
	m_pBounding->Update(matWorld);
}

void CCollider::OnCollision_Enter(CCollider* pOther)
{
	m_pOwner->OnCollision_Enter(m_iLayer, pOther);
}

void CCollider::OnCollision(CCollider* pOther)
{
	m_pOwner->OnCollision(m_iLayer, pOther);
}

void CCollider::OnCollision_Exit(CCollider* pOther)
{
	m_pOwner->OnCollision_Exit(m_iLayer, pOther);
}

_bool CCollider::Intersect(CCollider* pOther)
{
	m_bIsColl = m_pBounding->Intersect_Bounding(pOther->Get_Type(), pOther->Get_Bounding());
	return m_bIsColl;
}

_bool CCollider::IntersectWithRay_World(OUT _float4& vOut)
{
	return m_pBounding->IntersectWithRay_World(m_pGameInstance, vOut);
}

_bool CCollider::IntersectWithRay_Local(OUT _float4& vOut)
{
	return m_pBounding->IntersectWithRay_Local(m_pGameInstance, vOut);
}

_bool CCollider::IntersectWithRay_World(CRay* pRay, OUT _float4& vOut)
{
	return m_pBounding->IntersectWithRay_World(pRay, vOut);
}

_bool CCollider::IntersectWithRay_Local(CRay* pRay, OUT _float4& vOut)
{
	return m_pBounding->IntersectWithRay_Local(pRay, vOut);
}

HRESULT CCollider::Create_Bounding(CBounding::BOUNDING_DESC* pBoundingDesc)
{
	HRESULT hr = { S_OK };
	switch (m_eType)
	{
	case Engine::EColliderType::SPHERE:
	{
		m_pBounding = CBounding_Sphere::Create(m_pDevice, m_pDeviceContext, pBoundingDesc);
	} break;
	case Engine::EColliderType::AABB:
	{
		m_pBounding = CBounding_AABB::Create(m_pDevice, m_pDeviceContext, pBoundingDesc);
	} break;
	case Engine::EColliderType::OBB:
	{
		m_pBounding = CBounding_OBB::Create(m_pDevice, m_pDeviceContext, pBoundingDesc);
	} break;
	}

	if (!m_pBounding) hr = E_FAIL;

	return hr;
}

#ifdef _DEBUG
void CCollider::Render()
{
	if (Is_Active() == false)
		return;
	m_pDeviceContext->GSSetShader(nullptr, nullptr, 0);
	m_pEffect->SetWorld(XMMatrixIdentity());
	m_pEffect->SetView(::XMLoadFloat4x4(&m_pGameInstance->Get_ViewMatrix()));
	m_pEffect->SetProjection(::XMLoadFloat4x4(&m_pGameInstance->Get_ProjMatrix()));
	
	m_pEffect->Apply(m_pDeviceContext);
	m_pDeviceContext->IASetInputLayout(m_pInputLayout);
	
	m_pBatch->Begin();
	m_pBounding->Render(m_pBatch, m_bIsColl);
	m_pBatch->End();
}
#endif

CCollider* CCollider::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, EColliderType eType)
{
	CCollider* pInstance = new CCollider(pDevice, pDeviceContext, eType);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CCollider::Create, Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CCollider::Clone(void* pArg)
{
	CCollider* pClone = new CCollider(*this);

	if (FAILED(pClone->Initialize(pArg)))
	{
		MSG_BOX("CCollider::Clone, Failed");
		Safe_Release(pClone);
	}
	return pClone;
}

void CCollider::Free()
{
#ifdef _DEBUG
	if (IsClone() == false)
	{
		Safe_Delete(m_pBatch);
		Safe_Delete(m_pEffect);
	}
	Safe_Release(m_pInputLayout);
#endif
	Safe_Release(m_pBounding);
	Safe_Release(m_pDeviceContext);
	Safe_Release(m_pDevice);
	Super::Free();
}
