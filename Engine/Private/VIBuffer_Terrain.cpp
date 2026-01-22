#include "VIBuffer_Terrain.h"

CVIBuffer_Terrain::CVIBuffer_Terrain(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: Super(pDevice, pDeviceContext)
{
}

CVIBuffer_Terrain::CVIBuffer_Terrain(const CVIBuffer_Terrain& rhs)
	: Super(rhs)
	, m_iCol_VerticesCount(rhs.m_iCol_VerticesCount)
	, m_iRow_VerticesCount(rhs.m_iRow_VerticesCount)
{
}

HRESULT CVIBuffer_Terrain::Initialize_Prototype(void *pArg)
{
	if (FAILED(Super::Initialize_Prototype(pArg)))
		return E_FAIL;

    if (pArg)
    {
        VIBUFFER_TERRAIN_ORIGIN_DESC* pDesc = static_cast<VIBUFFER_TERRAIN_ORIGIN_DESC*>(pArg);
        m_iCol_VerticesCount = pDesc->iCol_VerticesCount;
        m_iRow_VerticesCount = pDesc->iRow_VerticesCount;
        m_fHeightRatio = pDesc->fHeightRatio;
        if (pDesc->pHeightFileMapPath)
        {
            if (FAILED(Set_HeightMapTerrain(pDesc->pHeightFileMapPath)))
                return E_FAIL;
            return S_OK;
        }
    }

    if (FAILED(Set_DefaultTerrain()))
        return E_FAIL;


	return S_OK;
}

HRESULT CVIBuffer_Terrain::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CVIBuffer_Terrain::Resize(VIBUFFER_TERRAIN_ORIGIN_DESC* pDesc)
{
    if ((m_VB_CPUAccesFlag & D3D11_CPU_ACCESS_WRITE) == 0)
        return E_FAIL;

    m_iCol_VerticesCount = pDesc->iCol_VerticesCount;
    m_iRow_VerticesCount = pDesc->iRow_VerticesCount;
    m_iVertexCount = m_iCol_VerticesCount * m_iRow_VerticesCount;
    if (pDesc->pHeightFileMapPath)
        return Resize_HeightMap();
    else
        return Resize_Default();
}

HRESULT CVIBuffer_Terrain::Set_HeightMapTerrain(const _tchar* pHeightFileMapPath)
{
	_ulong          dwByte = { };
	HANDLE          hFile = CreateFile(pHeightFileMapPath, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	BITMAPFILEHEADER        fh{};
	BITMAPINFOHEADER        ih{};
	_uint* pPixels = { nullptr };

	::ReadFile(hFile, &fh, sizeof(fh), &dwByte, nullptr);
	::ReadFile(hFile, &ih, sizeof(ih), &dwByte, nullptr);

	pPixels = new _uint[ih.biWidth * ih.biHeight];
	::ReadFile(hFile, pPixels, sizeof(_uint) * ih.biWidth * ih.biHeight, &dwByte, nullptr);

	CloseHandle(hFile);

    m_iVertexBufferCount = 1;

    m_iCol_VerticesCount = ih.biWidth;
    m_iRow_VerticesCount = ih.biHeight;

    m_iVertexStride = sizeof(VTXNORTEX);
    m_iVertexCount = m_iCol_VerticesCount * m_iRow_VerticesCount;
    m_iIndexStride = 4;
    m_eIndexFormat = m_iIndexStride == 2 ? DXGI_FORMAT::DXGI_FORMAT_R16_UINT : DXGI_FORMAT::DXGI_FORMAT_R32_UINT;

    m_iIndexCount = (m_iCol_VerticesCount - 1) * (m_iRow_VerticesCount - 1) * 2 * 3;
    m_ePrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

#pragma region VERTEX_BUFFER
    D3D11_BUFFER_DESC           VertexBufferDesc{};
    VertexBufferDesc.ByteWidth = m_iVertexStride * m_iVertexCount;
    VertexBufferDesc.Usage = m_VB_Usage;
    VertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    VertexBufferDesc.CPUAccessFlags = m_VB_CPUAccesFlag;
    VertexBufferDesc.MiscFlags = 0;
    VertexBufferDesc.StructureByteStride = m_iVertexStride;

    VTXNORTEX* pVertices = new VTXNORTEX[m_iVertexCount];
    ZeroMemory(pVertices, sizeof(VTXNORTEX) * m_iVertexCount);

    m_pVertexPositions = new _float3[m_iVertexCount];
    ZeroMemory(m_pVertexPositions, sizeof(_float3) * m_iVertexCount);

    for (_uint i = 0; i < m_iRow_VerticesCount; i++)
    {
        for (_uint j = 0; j < m_iCol_VerticesCount; j++)
        {
            _uint       iIndex = i * m_iCol_VerticesCount + j;

            m_pVertexPositions[iIndex] = pVertices[iIndex].vPosition = _float3((_float)j, (pPixels[iIndex] & 0x000000ff) / m_fHeightRatio, (_float)i);
            pVertices[iIndex].vNormal = _float3(0.f, 0.f, 0.f);
            pVertices[iIndex].vUV = _float2(j / (m_iCol_VerticesCount - 1.f), i / (m_iRow_VerticesCount - 1.f));
        }
    }



#pragma endregion

#pragma region INDEX_BUFFER
    D3D11_BUFFER_DESC           IndexBufferDesc{};
    IndexBufferDesc.ByteWidth = m_iIndexStride * m_iIndexCount;
    IndexBufferDesc.Usage = m_IB_Usage;
    IndexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    IndexBufferDesc.CPUAccessFlags = m_IB_CPUAccesFlag;
    IndexBufferDesc.MiscFlags = 0;
    IndexBufferDesc.StructureByteStride = m_iIndexStride;

    _uint* pIndices = new _uint[m_iIndexCount];

    _uint       iNumIndices = {};

    for (_uint i = 0; i < m_iRow_VerticesCount - 1; i++)
    {
        for (_uint j = 0; j < m_iCol_VerticesCount - 1; j++)
        {
            _uint       iIndex = i * m_iCol_VerticesCount + j;

            _uint       iIndices[4] = {
                iIndex + m_iCol_VerticesCount,
                iIndex + m_iCol_VerticesCount + 1,
                iIndex + 1,
                iIndex
            };

            _vector     vSour, vDest, vNormal;

            pIndices[iNumIndices++] = iIndices[0];
            pIndices[iNumIndices++] = iIndices[1];
            pIndices[iNumIndices++] = iIndices[2];

            vSour = ::XMLoadFloat3(&pVertices[iIndices[1]].vPosition) - ::XMLoadFloat3(&pVertices[iIndices[0]].vPosition);
            vDest = ::XMLoadFloat3(&pVertices[iIndices[2]].vPosition) - ::XMLoadFloat3(&pVertices[iIndices[1]].vPosition);

            vNormal = ::XMVector3Normalize(::XMVector3Cross(vSour, vDest));

            ::XMStoreFloat3(&pVertices[iIndices[0]].vNormal,
                ::XMLoadFloat3(&pVertices[iIndices[0]].vNormal) + vNormal);
            ::XMStoreFloat3(&pVertices[iIndices[1]].vNormal,
                ::XMLoadFloat3(&pVertices[iIndices[1]].vNormal) + vNormal);
            ::XMStoreFloat3(&pVertices[iIndices[2]].vNormal,
                ::XMLoadFloat3(&pVertices[iIndices[2]].vNormal) + vNormal);


            pIndices[iNumIndices++] = iIndices[0];
            pIndices[iNumIndices++] = iIndices[2];
            pIndices[iNumIndices++] = iIndices[3];


            vSour = ::XMLoadFloat3(&pVertices[iIndices[2]].vPosition) - ::XMLoadFloat3(&pVertices[iIndices[0]].vPosition);
            vDest = ::XMLoadFloat3(&pVertices[iIndices[3]].vPosition) - ::XMLoadFloat3(&pVertices[iIndices[2]].vPosition);

            vNormal = ::XMVector3Normalize(::XMVector3Cross(vSour, vDest));

            ::XMStoreFloat3(&pVertices[iIndices[0]].vNormal,
                ::XMLoadFloat3(&pVertices[iIndices[0]].vNormal) + vNormal);
            ::XMStoreFloat3(&pVertices[iIndices[2]].vNormal,
                ::XMLoadFloat3(&pVertices[iIndices[2]].vNormal) + vNormal);
            ::XMStoreFloat3(&pVertices[iIndices[3]].vNormal,
                ::XMLoadFloat3(&pVertices[iIndices[3]].vNormal) + vNormal);

        }
    }

    for (size_t i = 0; i < m_iVertexCount; i++)
    {
        ::XMStoreFloat3(&pVertices[i].vNormal,
            ::XMVector3Normalize(::XMLoadFloat3(&pVertices[i].vNormal)));
    }

#pragma endregion

    D3D11_SUBRESOURCE_DATA      VertexInitialData{};
    VertexInitialData.pSysMem = pVertices;

    if (FAILED(m_pDevice->CreateBuffer(&VertexBufferDesc, &VertexInitialData, &m_pVB)))
        return E_FAIL;

    Safe_Delete_Array(pVertices);
    Safe_Delete_Array(pPixels);

    D3D11_SUBRESOURCE_DATA      IndexInitialData{};
    IndexInitialData.pSysMem = pIndices;

    if (FAILED(m_pDevice->CreateBuffer(&IndexBufferDesc, &IndexInitialData, &m_pIB)))
        return E_FAIL;

    Safe_Delete_Array(pIndices);

    return S_OK;
}

HRESULT CVIBuffer_Terrain::Set_DefaultTerrain()
{
    m_iVertexBufferCount = 1;

    m_iVertexStride = sizeof(VTXNORTEX);
    m_iVertexCount = m_iCol_VerticesCount * m_iRow_VerticesCount;
    m_iIndexStride = 4;
    m_eIndexFormat = DXGI_FORMAT::DXGI_FORMAT_R32_UINT;

    m_iIndexCount = (m_iCol_VerticesCount - 1) * (m_iRow_VerticesCount - 1) * 2 * 3;
    m_ePrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

#pragma region VERTEX_BUFFER
    D3D11_BUFFER_DESC           VertexBufferDesc{};
    VertexBufferDesc.ByteWidth = m_iVertexStride * m_iVertexCount;
    VertexBufferDesc.Usage = m_VB_Usage;
    VertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    VertexBufferDesc.CPUAccessFlags = m_VB_CPUAccesFlag;
    VertexBufferDesc.MiscFlags = 0;
    VertexBufferDesc.StructureByteStride = m_iVertexStride;

    VTXNORTEX* pVertices = new VTXNORTEX[m_iVertexCount];
    ZeroMemory(pVertices, sizeof(VTXNORTEX) * m_iVertexCount);

    m_pVertexPositions = new _float3[m_iVertexCount];
    ZeroMemory(m_pVertexPositions, sizeof(_float3) * m_iVertexCount);

    for (_uint i = 0; i < m_iRow_VerticesCount; i++)
    {
        for (_uint j = 0; j < m_iCol_VerticesCount; j++)
        {
            _uint       iIndex = i * m_iCol_VerticesCount + j;

            m_pVertexPositions[iIndex] = pVertices[iIndex].vPosition = _float3((_float)j, 0.f, (_float)i);
            pVertices[iIndex].vNormal = _float3(0.f, 1.f, 0.f);
            pVertices[iIndex].vUV = _float2(j / (m_iCol_VerticesCount - 1.f), i / (m_iRow_VerticesCount - 1.f));
        }
    }
#pragma endregion

    D3D11_SUBRESOURCE_DATA      VertexInitialData{};
    VertexInitialData.pSysMem = pVertices;

    if (FAILED(m_pDevice->CreateBuffer(&VertexBufferDesc, &VertexInitialData, &m_pVB)))
        return E_FAIL;

    Safe_Delete_Array(pVertices);

#pragma region INDEX_BUFFER
    D3D11_BUFFER_DESC           IndexBufferDesc{};
    IndexBufferDesc.ByteWidth = m_iIndexStride * m_iIndexCount;
    IndexBufferDesc.Usage = m_IB_Usage;
    IndexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    IndexBufferDesc.CPUAccessFlags = m_IB_CPUAccesFlag;
    IndexBufferDesc.MiscFlags = 0;
    IndexBufferDesc.StructureByteStride = m_iIndexStride;

    _uint* pIndices = new _uint[m_iIndexCount];

    _uint       iNumIndices = {};

    for (_uint i = 0; i < m_iRow_VerticesCount - 1; i++)
    {
        for (_uint j = 0; j < m_iCol_VerticesCount - 1; j++)
        {
            _uint       iIndex = i * m_iCol_VerticesCount + j;

            _uint       iIndices[4] = {
                iIndex + m_iCol_VerticesCount,
                iIndex + m_iCol_VerticesCount + 1,
                iIndex + 1,
                iIndex
            };

            pIndices[iNumIndices++] = iIndices[0];
            pIndices[iNumIndices++] = iIndices[1];
            pIndices[iNumIndices++] = iIndices[2];

            pIndices[iNumIndices++] = iIndices[0];
            pIndices[iNumIndices++] = iIndices[2];
            pIndices[iNumIndices++] = iIndices[3];
        }
    }

#pragma endregion

    D3D11_SUBRESOURCE_DATA      IndexInitialData{};
    IndexInitialData.pSysMem = pIndices;

    if (FAILED(m_pDevice->CreateBuffer(&IndexBufferDesc, &IndexInitialData, &m_pIB)))
        return E_FAIL;

    Safe_Delete_Array(pIndices);

    return S_OK;
}

HRESULT CVIBuffer_Terrain::Resize_HeightMap()
{
    return S_OK;
}

HRESULT CVIBuffer_Terrain::Resize_Default()
{
    // Resize VB
    {
        VTXNORTEX* pVertices = new VTXNORTEX[m_iVertexCount];
        ::ZeroMemory(pVertices, sizeof(VTXNORTEX) * m_iVertexCount);

        for (_uint i = 0; i < m_iRow_VerticesCount; i++)
        {
            for (_uint j = 0; j < m_iCol_VerticesCount; j++)
            {
                _uint       iIndex = i * m_iCol_VerticesCount + j;

                m_pVertexPositions[iIndex] = pVertices[iIndex].vPosition = _float3((_float)j, 0.f, (_float)i);
                pVertices[iIndex].vNormal = _float3(0.f, 1.f, 0.f);
                pVertices[iIndex].vUV = _float2(j / (m_iCol_VerticesCount - 1.f), i / (m_iRow_VerticesCount - 1.f));
            }
        }

        D3D11_MAPPED_SUBRESOURCE subResource = {};
        if (FAILED(m_pDeviceContext->Map(m_pVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource)))
            return E_FAIL;
        ::memcpy(subResource.pData, pVertices, sizeof(VTXNORTEX) * m_iVertexCount);
        m_pDeviceContext->Unmap(m_pVB, 0);
        Safe_Delete_Array(pVertices);
    }

    // Resize IB
    {
        _uint* pIndices = new _uint[m_iIndexCount];

        _uint       iNumIndices = {};

        for (_uint i = 0; i < m_iRow_VerticesCount - 1; i++)
        {
            for (_uint j = 0; j < m_iCol_VerticesCount - 1; j++)
            {
                _uint       iIndex = i * m_iCol_VerticesCount + j;

                _uint       iIndices[4] = {
                    iIndex + m_iCol_VerticesCount,
                    iIndex + m_iCol_VerticesCount + 1,
                    iIndex + 1,
                    iIndex
                };

                pIndices[iNumIndices++] = iIndices[0];
                pIndices[iNumIndices++] = iIndices[1];
                pIndices[iNumIndices++] = iIndices[2];

                pIndices[iNumIndices++] = iIndices[0];
                pIndices[iNumIndices++] = iIndices[2];
                pIndices[iNumIndices++] = iIndices[3];
            }
        }

        D3D11_MAPPED_SUBRESOURCE subResource = {};
        if (FAILED(m_pDeviceContext->Map(m_pIB, 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource)))
            return E_FAIL;
        ::memcpy(subResource.pData, pIndices, sizeof(_uint) * m_iIndexCount);
        m_pDeviceContext->Unmap(m_pIB, 0);
        Safe_Delete_Array(pIndices);
    }

    return S_OK;
}

CVIBuffer_Terrain* CVIBuffer_Terrain::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg)
{
	CVIBuffer_Terrain* pInstance = new CVIBuffer_Terrain(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype(pArg)))
	{
		MSG_BOX("CVIBuffer_Terrain::Create, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CVIBuffer_Terrain::Clone(void* pArg)
{
	CVIBuffer_Terrain* pInstance = new CVIBuffer_Terrain(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CVIBuffer_Terrain::Clone, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CVIBuffer_Terrain::Free()
{
	Super::Free();
}
