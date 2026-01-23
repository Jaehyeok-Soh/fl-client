#include "VIBuffer_Cube_NorTex.h"

CVIBuffer_Cube_NorTex::CVIBuffer_Cube_NorTex(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
    : Super(pDevice, pDeviceContext)
{
}

CVIBuffer_Cube_NorTex::CVIBuffer_Cube_NorTex(const CVIBuffer_Cube_NorTex& rhs)
    : Super(rhs)
{
}

HRESULT CVIBuffer_Cube_NorTex::Initialize_Prototype(void* pArg)
{
    if (FAILED(Super::Initialize_Prototype(pArg)))
        return E_FAIL;

    constexpr _float fWidth = 0.5f;
    constexpr _float fHeight = 0.5f;
    constexpr _float fDepth = 0.5f;

    m_iVertexBufferCount = 1;
    m_iVertexStride = sizeof(VTXNORTEX);
    m_iVertexCount = 24;

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

    VTXNORTEX* pVertices = new VTXNORTEX[m_iVertexCount];
    ZeroMemory(pVertices, sizeof(VTXNORTEX) * m_iVertexCount);

    m_pVertexPositions = new Vec3[m_iVertexCount];
    ZeroMemory(m_pVertexPositions, sizeof(Vec3) * m_iVertexCount);

    // ¾Õ¸é
    pVertices[0] = { Vec3(-fWidth, -fHeight, -fDepth), Vec3(0.0f, 0.0f, -1.0f), Vec2(0.0f, 1.0f) };
    pVertices[1] = { Vec3(-fWidth, +fHeight, -fDepth), Vec3(0.0f, 0.0f, -1.0f), Vec2(0.0f, 0.0f) };
    pVertices[2] = { Vec3(+fWidth, +fHeight, -fDepth), Vec3(0.0f, 0.0f, -1.0f), Vec2(1.0f, 0.0f) };
    pVertices[3] = { Vec3(+fWidth, -fHeight, -fDepth), Vec3(0.0f, 0.0f, -1.0f), Vec2(1.0f, 1.0f) };
    // µÞ¸é
    pVertices[4] = { Vec3(-fWidth, -fHeight, +fDepth), Vec3(0.0f, 0.0f, 1.0f), Vec2(1.0f, 1.0f) };
    pVertices[5] = { Vec3(+fWidth, -fHeight, +fDepth), Vec3(0.0f, 0.0f, 1.0f), Vec2(0.0f, 1.0f) };
    pVertices[6] = { Vec3(+fWidth, +fHeight, +fDepth), Vec3(0.0f, 0.0f, 1.0f), Vec2(0.0f, 0.0f) };
    pVertices[7] = { Vec3(-fWidth, +fHeight, +fDepth), Vec3(0.0f, 0.0f, 1.0f), Vec2(1.0f, 0.0f) };
    // À­¸é
    pVertices[8] = { Vec3(-fWidth, +fHeight, -fDepth), Vec3(0.0f, 1.0f, 0.0f), Vec2(0.0f, 1.0f) };
    pVertices[9] = { Vec3(-fWidth, +fHeight, +fDepth), Vec3(0.0f, 1.0f, 0.0f), Vec2(0.0f, 0.0f) };
    pVertices[10] = { Vec3(+fWidth, +fHeight, +fDepth), Vec3(0.0f, 1.0f, 0.0f), Vec2(1.0f, 0.0f) };
    pVertices[11] = { Vec3(+fWidth, +fHeight, -fDepth), Vec3(0.0f, 1.0f, 0.0f), Vec2(1.0f, 1.0f) };
    // ¾Æ·§¸é
    pVertices[12] = { Vec3(-fWidth, -fHeight, -fDepth), Vec3(0.0f, -1.0f, 0.0f), Vec2(1.0f, 1.0f) };
    pVertices[13] = { Vec3(+fWidth, -fHeight, -fDepth), Vec3(0.0f, -1.0f, 0.0f), Vec2(0.0f, 1.0f) };
    pVertices[14] = { Vec3(+fWidth, -fHeight, +fDepth), Vec3(0.0f, -1.0f, 0.0f), Vec2(0.0f, 0.0f) };
    pVertices[15] = { Vec3(-fWidth, -fHeight, +fDepth), Vec3(0.0f, -1.0f, 0.0f), Vec2(1.0f, 0.0f) };
    // ¿ÞÂÊ¸é
    pVertices[16] = { Vec3(-fWidth, -fHeight, +fDepth), Vec3(-1.0f, 0.0f, 0.0f), Vec2(0.0f, 1.0f) };
    pVertices[17] = { Vec3(-fWidth, +fHeight, +fDepth), Vec3(-1.0f, 0.0f, 0.0f), Vec2(0.0f, 0.0f) };
    pVertices[18] = { Vec3(-fWidth, +fHeight, -fDepth), Vec3(-1.0f, 0.0f, 0.0f), Vec2(1.0f, 0.0f) };
    pVertices[19] = { Vec3(-fWidth, -fHeight, -fDepth), Vec3(-1.0f, 0.0f, 0.0f), Vec2(1.0f, 1.0f) };
    // ¿À¸¥ÂÊ¸é
    pVertices[20] = { Vec3(+fWidth, -fHeight, -fDepth), Vec3(1.0f, 0.0f, 0.0f), Vec2(0.0f, 1.0f) };
    pVertices[21] = { Vec3(+fWidth, +fHeight, -fDepth), Vec3(1.0f, 0.0f, 0.0f), Vec2(0.0f, 0.0f) };
    pVertices[22] = { Vec3(+fWidth, +fHeight, +fDepth), Vec3(1.0f, 0.0f, 0.0f), Vec2(1.0f, 0.0f) };
    pVertices[23] = { Vec3(+fWidth, -fHeight, +fDepth), Vec3(1.0f, 0.0f, 0.0f), Vec2(1.0f, 1.0f) };

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

    // ¾Õ¸é
    pIndices[0] = 0; pIndices[1] = 1; pIndices[2] = 2;
    pIndices[3] = 0; pIndices[4] = 2; pIndices[5] = 3;
    // µÞ¸é
    pIndices[6] = 4; pIndices[7] = 5; pIndices[8] = 6;
    pIndices[9] = 4; pIndices[10] = 6; pIndices[11] = 7;
    // À­¸é
    pIndices[12] = 8; pIndices[13] = 9; pIndices[14] = 10;
    pIndices[15] = 8; pIndices[16] = 10; pIndices[17] = 11;
    // ¾Æ·§¸é
    pIndices[18] = 12; pIndices[19] = 13; pIndices[20] = 14;
    pIndices[21] = 12; pIndices[22] = 14; pIndices[23] = 15;
    // ¿ÞÂÊ¸é
    pIndices[24] = 16; pIndices[25] = 17; pIndices[26] = 18;
    pIndices[27] = 16; pIndices[28] = 18; pIndices[29] = 19;
    // ¿À¸¥ÂÊ¸é
    pIndices[30] = 20; pIndices[31] = 21; pIndices[32] = 22;
    pIndices[33] = 20; pIndices[34] = 22; pIndices[35] = 23;

    D3D11_SUBRESOURCE_DATA      IndexInitialData{};
    IndexInitialData.pSysMem = pIndices;

    if (FAILED(m_pDevice->CreateBuffer(&IndexBufferDesc, &IndexInitialData, &m_pIB)))
        return E_FAIL;

    Safe_Delete_Array(pIndices);
#pragma endregion

    return S_OK;
}

HRESULT CVIBuffer_Cube_NorTex::Initialize(void* pArg)
{
    if (FAILED(Super::Initialize(pArg)))
        return E_FAIL;

    return S_OK;
}

CVIBuffer_Cube_NorTex* CVIBuffer_Cube_NorTex::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg)
{
    CVIBuffer_Cube_NorTex* pInstance = new CVIBuffer_Cube_NorTex(pDevice, pDeviceContext);
    if (FAILED(pInstance->Initialize_Prototype(pArg)))
    {
        MSG_BOX("CVIBuffer_Cube_NorTex::Create, Failed");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CComponent* CVIBuffer_Cube_NorTex::Clone(void* pArg)
{
    CVIBuffer_Cube_NorTex* pInstance = new CVIBuffer_Cube_NorTex(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("CVIBuffer_Cube_NorTex::Create, Clone");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CVIBuffer_Cube_NorTex::Free()
{
    Super::Free();
}
