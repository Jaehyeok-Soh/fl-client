#include "Engine_pch.h"
#include "InstanceStaticMesh.h"

CInstanceStaticMesh::CInstanceStaticMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CVIBuffer(pDevice,pDeviceContext)
{
}

CInstanceStaticMesh::CInstanceStaticMesh(const CInstanceStaticMesh& rhs)
	: CVIBuffer(rhs), m_pReferModel(rhs.m_pReferModel), m_pInstanceBuffer(rhs.m_pInstanceBuffer)
{
}


HRESULT CInstanceStaticMesh::Initialize_Prototype()
{
	/* 둘다 접근 불가 완전 Render용 객체로 만들어준다 움직임이 필요하다면 WorldMatrix를 변화시켜서 움직여준다 */
	CVIBuffer::VIBUFFER_ORIGIN_DESC tDesc{};
	tDesc.IB_Usage = D3D11_USAGE_IMMUTABLE;
	tDesc.VB_Usage = D3D11_USAGE_IMMUTABLE;

	if (FAILED(Super::Initialize_Prototype(&tDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CInstanceStaticMesh::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	D3D11_BUFFER_DESC tVertexBufferDesc{};
	tVertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	tVertexBufferDesc.


	return S_OK;
}

HRESULT CInstanceStaticMesh::Make_InsatnceDataByPrototypeModel(CModel* pModel)
{
	return S_OK;
}


CInstanceStaticMesh* CInstanceStaticMesh::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CInstanceStaticMesh* pInsStaticMesh = new CInstanceStaticMesh(pDevice, pDeviceContext);

	if (FAILED(pInsStaticMesh->Initialize_Prototype()))
	{

	}

	return nullptr;
}

CComponent* CInstanceStaticMesh::Clone(void* pArg)
{
	return nullptr;
}

void CInstanceStaticMesh::Free()
{
}

