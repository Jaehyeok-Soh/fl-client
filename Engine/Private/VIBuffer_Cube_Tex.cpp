#include "Engine_pch.h"
#include "VIBuffer_Cube_Tex.h"

CVIBuffer_Cube_Tex::CVIBuffer_Cube_Tex(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
    : Super(pDevice, pDeviceContext)
{
}

CVIBuffer_Cube_Tex::CVIBuffer_Cube_Tex(const CVIBuffer_Cube_Tex& rhs)
    : Super(rhs)
{
}

HRESULT CVIBuffer_Cube_Tex::Initialize_Prototype(void* pArg)
{
    if (FAILED(Super::Initialize_Prototype(pArg)))
        return E_FAIL;

    m_iVertexBufferCount = 1;
    m_iVertexStride = sizeof(VTXCUBE);
    m_iVertexCount = 8;

    m_iIndexStride = 2;
    m_eIndexFormat = m_iIndexStride == 2 ? DXGI_FORMAT::DXGI_FORMAT_R16_UINT : DXGI_FORMAT::DXGI_FORMAT_R32_UINT;
    m_iIndexCount = 36;
    m_ePrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

#pragma region VERTEX_BUFFER
    D3D11_BUFFER_DESC           VertexBufferDesc{};
    VertexBufferDesc.ByteWidth = m_iVertexStride * m_iVertexCount;
    VertexBufferDesc.Usage = m_VB_Usage;
    VertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    VertexBufferDesc.CPUAccessFlags = m_VB_CPUAccesFlag;
    VertexBufferDesc.MiscFlags = 0;
    VertexBufferDesc.StructureByteStride = m_iVertexStride;

    VTXCUBE* pVertices = new VTXCUBE[m_iVertexCount];
    ZeroMemory(pVertices, sizeof(VTXCUBE) * m_iVertexCount);

    m_pVertexPositions = new Vec3[m_iVertexCount];
    ZeroMemory(m_pVertexPositions, sizeof(Vec3) * m_iVertexCount);

    m_pVertexPositions[0] = pVertices[0].vPosition = Vec3(-0.5f, 0.5f, -0.5f);
    pVertices[0].vTexcoord = m_pVertexPositions[0];

    m_pVertexPositions[1] = pVertices[1].vPosition = Vec3(0.5f, 0.5f, -0.5f);
    pVertices[1].vTexcoord = m_pVertexPositions[1];

    m_pVertexPositions[2] = pVertices[2].vPosition = Vec3(0.5f, -0.5f, -0.5f);
    pVertices[2].vTexcoord = m_pVertexPositions[2];

    m_pVertexPositions[3] = pVertices[3].vPosition = Vec3(-0.5f, -0.5f, -0.5f);
    pVertices[3].vTexcoord = m_pVertexPositions[3];

    m_pVertexPositions[4] = pVertices[4].vPosition = Vec3(-0.5f, 0.5f, 0.5f);
    pVertices[4].vTexcoord = m_pVertexPositions[4];

    m_pVertexPositions[5] = pVertices[5].vPosition = Vec3(0.5f, 0.5f, 0.5f);
    pVertices[5].vTexcoord = m_pVertexPositions[5];

    m_pVertexPositions[6] = pVertices[6].vPosition = Vec3(0.5f, -0.5f, 0.5f);
    pVertices[6].vTexcoord = m_pVertexPositions[6];

    m_pVertexPositions[7] = pVertices[7].vPosition = Vec3(-0.5f, -0.5f, 0.5f);
    pVertices[7].vTexcoord = m_pVertexPositions[7];

    for (_uint i = 0; i < m_iVertexCount; ++i)
    {
        m_pVertexPositions[i] = pVertices[i].vPosition;
    }

    D3D11_SUBRESOURCE_DATA      VertexInitialData{};
    VertexInitialData.pSysMem = pVertices;

    if (FAILED(m_pDevice->CreateBuffer(&VertexBufferDesc, &VertexInitialData, &m_pVB)))
        return E_FAIL;

    Safe_Delete_Array(pVertices);

#pragma endregion

#pragma region INDEX_BUFFER
    D3D11_BUFFER_DESC           IndexBufferDesc{};
    IndexBufferDesc.ByteWidth = m_iIndexStride * m_iIndexCount;
    IndexBufferDesc.Usage = m_IB_Usage;
    IndexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    IndexBufferDesc.CPUAccessFlags = m_IB_CPUAccesFlag;
    IndexBufferDesc.MiscFlags = 0;
    IndexBufferDesc.StructureByteStride = m_iIndexStride;

    _ushort* pIndices = new _ushort[m_iIndexCount];

    pIndices[0] = 1; pIndices[1] = 5; pIndices[2] = 6;
    pIndices[3] = 1; pIndices[4] = 6; pIndices[5] = 2;

    pIndices[6] = 4; pIndices[7] = 0; pIndices[8] = 3;
    pIndices[9] = 4; pIndices[10] = 3; pIndices[11] = 7;


    pIndices[12] = 4; pIndices[13] = 5; pIndices[14] = 1;
    pIndices[15] = 4; pIndices[16] = 1; pIndices[17] = 0;


    pIndices[18] = 3; pIndices[19] = 2; pIndices[20] = 6;
    pIndices[21] = 3; pIndices[22] = 6; pIndices[23] = 7;


    pIndices[24] = 7; pIndices[25] = 6; pIndices[26] = 5;
    pIndices[27] = 7; pIndices[28] = 5; pIndices[29] = 4;


    pIndices[30] = 0; pIndices[31] = 1; pIndices[32] = 2;
    pIndices[33] = 0; pIndices[34] = 2; pIndices[35] = 3;

    D3D11_SUBRESOURCE_DATA      IndexInitialData{};
    IndexInitialData.pSysMem = pIndices;

    if (FAILED(m_pDevice->CreateBuffer(&IndexBufferDesc, &IndexInitialData, &m_pIB)))
        return E_FAIL;

    Safe_Delete_Array(pIndices);
#pragma endregion

    return S_OK;
}

HRESULT CVIBuffer_Cube_Tex::Initialize(void* pArg)
{
    if (FAILED(Super::Initialize(pArg)))
        return E_FAIL;

    return S_OK;
}

CVIBuffer_Cube_Tex* CVIBuffer_Cube_Tex::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg)
{
    CVIBuffer_Cube_Tex* pInstance = new CVIBuffer_Cube_Tex(pDevice, pDeviceContext);
    if (FAILED(pInstance->Initialize_Prototype(pArg)))
    {
        MSG_BOX("CVIBuffer_Cube_Tex::Create, Failed");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CComponent* CVIBuffer_Cube_Tex::Clone(void* pArg)
{
    CVIBuffer_Cube_Tex* pInstance = new CVIBuffer_Cube_Tex(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("CVIBuffer_Cube_Tex::Create, Clone");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CVIBuffer_Cube_Tex::Free()
{
    Super::Free();
}
