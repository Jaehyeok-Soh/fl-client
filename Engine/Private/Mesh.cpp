#include "Mesh.h"
#include "Bone.h"
#include "Ray.h"
#include "Shader.h"
#include "GameInstance.h"

CMesh::CMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: Super(pDevice, pDeviceContext)
{
	for (_float4x4& matBone : m_boneMatrices.transforms)
		::XMStoreFloat4x4(&matBone, ::XMMatrixIdentity());
}

CMesh::CMesh(const CMesh& rhs)
	: Super(rhs)
	, m_iMaterialIndex(rhs.m_iMaterialIndex)
	, m_iAffectBoneCount(rhs.m_iAffectBoneCount)
	, m_vecAffectBoneIndices(rhs.m_vecAffectBoneIndices)
	, m_boneMatrices(rhs.m_boneMatrices)
	, m_vecOffsetMatrices(rhs.m_vecOffsetMatrices)
	, m_pNormals(rhs.m_pNormals)
	, m_pSurfaceTypes(rhs.m_pSurfaceTypes)
{
	::strcpy_s(m_szName, rhs.m_szName);
}

// ModelLoader가 PreMatrix, Bone 등 처리 다 해서 넘기기
HRESULT CMesh::Initialize_Prototype(void* pArg)
{
	if (!pArg)
		return E_FAIL;

	if(FAILED(Super::Initialize_Prototype(pArg)))
		return E_FAIL;

	MESH_ORIGIN_DESC* pDesc = static_cast<MESH_ORIGIN_DESC*>(pArg);
	::strcpy_s(m_szName, pDesc->strName.c_str());
	m_ePrimitiveType = pDesc->ePrimitiveType;
	m_iMaterialIndex = pDesc->iMaterialIndex;

	m_iVertexBufferCount = 1;
	m_iVertexCount = pDesc->iVertexCount;

	m_iIndexStride = sizeof(_uint);
	m_eIndexFormat = DXGI_FORMAT::DXGI_FORMAT_R32_UINT;
	m_iIndexCount = pDesc->iIndexCount;

	m_iAffectBoneCount = pDesc->iAffectBoneCount;
	if (m_iAffectBoneCount > 0)
	{
		m_vecAffectBoneIndices.resize(m_iAffectBoneCount);
		::memcpy(m_vecAffectBoneIndices.data(), pDesc->spanAffectBoneIndex.data(), sizeof(_uint) * m_iAffectBoneCount);
	}

	if (pDesc->iOffsetMatricesCount > 0)
	{
		m_vecOffsetMatrices.resize(pDesc->iOffsetMatricesCount);
		::memcpy(m_vecOffsetMatrices.data(), pDesc->spanOffsetMatrices.data(), sizeof(_float4x4) * pDesc->iOffsetMatricesCount);
	}

	m_pVertexPositions = new _float3[m_iVertexCount];
	for (_uint i = 0; i < m_iVertexCount; ++i)
	{
		m_pVertexPositions[i] = pDesc->spanVertex[i].vPosition;
	}

	HRESULT hr = {};
	switch (pDesc->eModelType)
	{
	case EModelType::ANIM:
	{
		hr = Load_AnimVertices(pDesc->spanVertex);
	} break;
	case EModelType::NONANIM:
	{
		hr = Load_NonAnimVertices(pDesc->spanVertex);
	} break;
	case EModelType::STATIC:
	{
		hr = Load_NonAnimVertices(pDesc->spanVertex);
	} break;
	default:
	{
		MSG_BOX("CMesh::Initialize_Prototype, Modeltype is wrong");
		return E_FAIL;
	}
	}	
	
	if (FAILED(hr))
		return E_FAIL;
	
	m_pIndices = new _uint[m_iIndexCount];

	// IndexBuffer
	{
		D3D11_BUFFER_DESC IndexBufferDesc = {};
		IndexBufferDesc.ByteWidth = (_uint)(m_iIndexStride * m_iIndexCount);
		IndexBufferDesc.Usage = m_IB_Usage;
		IndexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		IndexBufferDesc.CPUAccessFlags = m_IB_CPUAccesFlag;
		IndexBufferDesc.MiscFlags = 0;
		IndexBufferDesc.StructureByteStride = m_iIndexStride;

		D3D11_SUBRESOURCE_DATA IndexBufferData = {};
		::ZeroMemory(&IndexBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
		IndexBufferData.pSysMem = pDesc->spanIndex.data();
		::memcpy(m_pIndices, pDesc->spanIndex.data(), (_uint)(m_iIndexStride * m_iIndexCount));
		if (FAILED(m_pDevice->CreateBuffer(&IndexBufferDesc, &IndexBufferData, &m_pIB)))
			return E_FAIL;
	}

	if (pDesc->bSaveNormal)
	{
		const _uint iTriangleCount = m_iIndexCount / 3;

		m_pNormals = new _float3[iTriangleCount];
		m_pSurfaceTypes = new ESurfaceType[iTriangleCount];
		_vector vWorldUp = ::XMVectorSet(0.f, 1.f, 0.f, 0.f);
		_uint iIndex = { 0 };
		for (_uint i = 0; i < iTriangleCount; ++i)
		{
			const _float3& A = m_pVertexPositions[m_pIndices[iIndex++]];
			const _float3& B = m_pVertexPositions[m_pIndices[iIndex++]];
			const _float3& C = m_pVertexPositions[m_pIndices[iIndex++]];

			_vector vA = ::XMLoadFloat3(&A);
			_vector vB = ::XMLoadFloat3(&B);
			_vector vC = ::XMLoadFloat3(&C);

			_vector vAB = vB - vA;
			_vector vAC = vC - vA;
			_vector vNormal = ::XMVector3Normalize(::XMVector3Cross(vAB, vAC));

			ESurfaceType eType = ESurfaceType::CEILING;
			const _float fDot = ::XMVectorGetX(::XMVector3Dot(vNormal, vWorldUp));
			const _float fCosGroundMax = ::cosf(::XMConvertToRadians(50.f));

			if (fDot >= fCosGroundMax)
				eType = ESurfaceType::GROUND;
			else if (fDot > -0.1f)
				eType = ESurfaceType::WALL;

			::XMStoreFloat3(&m_pNormals[i], vNormal);
			m_pSurfaceTypes[i] = eType;
		}
	}		

	return S_OK;
}

HRESULT CMesh::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMesh::Bind_Bones(CShader* pShader, const vector<CBone*>& vecBones, _uint iIndexDistance)
{
	for (size_t i = 0; i < m_vecAffectBoneIndices.size(); ++i)
	{
		::XMStoreFloat4x4(&m_boneMatrices.transforms[i + iIndexDistance],
			::XMLoadFloat4x4(&m_vecOffsetMatrices[i]) *
			vecBones[m_vecAffectBoneIndices[i]]->Get_CombinedTransformMatrix());
	}
	return pShader->Bind_BoneData(m_boneMatrices);
}

_bool CMesh::IntsersectWithPlane(OUT _float4& vOut)
{
	const _uint iTriangleCount = m_iIndexCount / 3;
	_uint iIndex = { 0 };
	for (_uint i = 0; i < iTriangleCount; ++i)
	{
		if (m_pGameInstance->IntersectrayWithTriangle_Local(m_pVertexPositions[m_pIndices[iIndex++]],
			m_pVertexPositions[m_pIndices[iIndex++]],
			m_pVertexPositions[m_pIndices[iIndex++]], vOut))
		{
			return true;
		}
	}
	return false;
}

_bool CMesh::IntsersectWithPlane(CRay* const pRay, _fmatrix matWorld, _float fMaxDistance, OUT MESH_RAY_HITINFO& outHit)
{
	if (m_iIndexCount == 0 || !m_pVertexPositions || !m_pIndices)
		return false;

	MESH_RAY_HITINFO bestHit = {};
	bestHit.fDistance = fMaxDistance;

	_vector vRayOrigin = ::XMLoadFloat4(&pRay->Get_Origin());
	_vector vRayDir = ::XMVector3Normalize(::XMLoadFloat3(&pRay->Get_Dir()));

	const _uint iTriangleCount = m_iIndexCount / 3;
	_uint iIndex = { 0 };

	_bool bHit = { false };
	for (_uint i = 0; i < iTriangleCount; ++i)
	{
		_float4 vHitted = {};

		_float3 A = {};
		_float3 B = {};
		_float3 C = {};

		::XMStoreFloat3(&A, ::XMVector3TransformCoord(::XMLoadFloat3(&m_pVertexPositions[m_pIndices[iIndex++]]), matWorld));
		::XMStoreFloat3(&B, ::XMVector3TransformCoord(::XMLoadFloat3(&m_pVertexPositions[m_pIndices[iIndex++]]), matWorld));
		::XMStoreFloat3(&C, ::XMVector3TransformCoord(::XMLoadFloat3(&m_pVertexPositions[m_pIndices[iIndex++]]), matWorld));

		if (pRay->IntersectrayWithTriangle_World(A, B, C, vHitted) == false)
			continue;

		_vector vHitPoint = ::XMLoadFloat4(&vHitted);

		_vector vDiff = vHitPoint - vRayOrigin;
		_float fDistance = ::XMVectorGetX(::XMVector3Dot(vDiff, vRayDir));

		if (fDistance < 0.f || fDistance > bestHit.fDistance)
			continue;

		bestHit.fDistance = fDistance;
		bestHit.iTriangleIndex = static_cast<_int>(i);
		bestHit.vNormal = m_pNormals[i];
		bestHit.eSurfaceType = m_pSurfaceTypes[i];
		::XMStoreFloat3(&bestHit.vHitPos, vHitPoint);

		bHit = true;
	}

	if (bHit == false)
		return false;

	outHit = bestHit;
	return true;
}

HRESULT CMesh::Load_AnimVertices(std::span<VTXANIMMESH> spanVertex)
{
	m_iVertexStride = sizeof(VTXANIMMESH);

	{
		D3D11_BUFFER_DESC VertexBufferDesc = {};
		VertexBufferDesc.ByteWidth = (_uint)(m_iVertexStride * m_iVertexCount);
		VertexBufferDesc.Usage = m_VB_Usage;
		VertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		VertexBufferDesc.CPUAccessFlags = m_VB_CPUAccesFlag;
		VertexBufferDesc.MiscFlags = 0;
		VertexBufferDesc.StructureByteStride = m_iVertexStride;

		D3D11_SUBRESOURCE_DATA VertexBufferData = {};
		::ZeroMemory(&VertexBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
		VertexBufferData.pSysMem = spanVertex.data();

		if (FAILED(m_pDevice->CreateBuffer(&VertexBufferDesc, &VertexBufferData, &m_pVB)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CMesh::Load_NonAnimVertices(std::span<VTXANIMMESH> spanVertex)
{
	m_iVertexStride = sizeof(VTXMESH);

	{
		D3D11_BUFFER_DESC VertexBufferDesc = {};
		VertexBufferDesc.ByteWidth = (_uint)(m_iVertexStride * m_iVertexCount);
		VertexBufferDesc.Usage = m_VB_Usage;
		VertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		VertexBufferDesc.CPUAccessFlags = m_VB_CPUAccesFlag;
		VertexBufferDesc.MiscFlags = 0;
		VertexBufferDesc.StructureByteStride = m_iVertexStride;

		vector<VTXMESH> vertices(m_iVertexCount);
		for (_uint i = 0; i < m_iVertexCount; ++i)
		{
			vertices[i].vPosition = spanVertex[i].vPosition;
			vertices[i].vNormal = spanVertex[i].vNormal;
			vertices[i].vUV = spanVertex[i].vUV;
			vertices[i].vBinormal = spanVertex[i].vBinormal;
			vertices[i].vTangent = spanVertex[i].vTangent;
		}

		D3D11_SUBRESOURCE_DATA VertexBufferData = {};
		::ZeroMemory(&VertexBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
		VertexBufferData.pSysMem = vertices.data();

		if (FAILED(m_pDevice->CreateBuffer(&VertexBufferDesc, &VertexBufferData, &m_pVB)))
			return E_FAIL;
	}

	return S_OK;
}

CMesh* CMesh::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg)
{
	CMesh* pInstance = new CMesh(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype(pArg)))
	{
		MSG_BOX("CMesh::Create, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CMesh::Clone(void* pArg)
{
	CMesh* pInstance = new CMesh(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CMesh::Clone, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMesh::Free()
{
	if (IsClone() == false)
	{
		Safe_Delete_Array(m_pNormals);
		Safe_Delete_Array(m_pSurfaceTypes);
	}
	Super::Free();
}
