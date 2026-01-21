#include "VIBuffer_Cube_Color.h"

CVIBuffer_Cube_Color::CVIBuffer_Cube_Color(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
    : Super(pDevice, pDeviceContext)
{
}

CVIBuffer_Cube_Color::CVIBuffer_Cube_Color(const CVIBuffer_Cube_Color& rhs)
    : Super(rhs)
    , m_vColor(rhs.m_vColor)
{
}

HRESULT CVIBuffer_Cube_Color::Initialize_Prototype(void* pArg)
{
    if (FAILED(Super::Initialize_Prototype(pArg)))
        return E_FAIL;

    if (!pArg)
    {
        m_vColor = { 0.f, 1.f, 0.f, 1.f };
    }
    else
    {
        VIBUFFER_CUBE_ORIGIN_DESC* pDesc = static_cast<VIBUFFER_CUBE_ORIGIN_DESC*>(pArg);
        m_vColor = pDesc->vColor;
    }

    constexpr _float fWidth = 0.5f;
    constexpr _float fHeight = 0.5f;
    constexpr _float fDepth = 0.5f;

    m_iVertexBufferCount = 1;
    m_iVertexStride = sizeof(VTXPOSCOL);
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

    VTXPOSCOL* pVertices = new VTXPOSCOL[m_iVertexCount];
    ZeroMemory(pVertices, sizeof(VTXPOSCOL) * m_iVertexCount);

    m_pVertexPositions = new _float3[m_iVertexCount];
    ZeroMemory(m_pVertexPositions, sizeof(_float3) * m_iVertexCount);

    // ¾Õ¸é
    pVertices[0] = VTXPOSCOL{ _float3(-fWidth, -fHeight, -fDepth), m_vColor };
    pVertices[1] = VTXPOSCOL{ _float3(-fWidth, +fHeight, -fDepth), m_vColor };
    pVertices[2] = VTXPOSCOL{ _float3(+fWidth, +fHeight, -fDepth), m_vColor };
    pVertices[3] = VTXPOSCOL{ _float3(+fWidth, -fHeight, -fDepth), m_vColor };
    // µÞ¸é
    pVertices[4] = VTXPOSCOL{ _float3(-fWidth, -fHeight, +fDepth), m_vColor };
    pVertices[5] = VTXPOSCOL{ _float3(+fWidth, -fHeight, +fDepth), m_vColor };
    pVertices[6] = VTXPOSCOL{ _float3(+fWidth, +fHeight, +fDepth), m_vColor };
    pVertices[7] = VTXPOSCOL{ _float3(-fWidth, +fHeight, +fDepth), m_vColor };
    // À­¸é
    pVertices[8] = VTXPOSCOL{ _float3(-fWidth, +fHeight, -fDepth), m_vColor };
    pVertices[9] = VTXPOSCOL{ _float3(-fWidth, +fHeight, +fDepth), m_vColor };
    pVertices[10] = VTXPOSCOL{ _float3(+fWidth, +fHeight, +fDepth), m_vColor };
    pVertices[11] = VTXPOSCOL{ _float3(+fWidth, +fHeight, -fDepth), m_vColor };
    // ¾Æ·§¸é
    pVertices[12] = VTXPOSCOL{ _float3(-fWidth, -fHeight, -fDepth), m_vColor };
    pVertices[13] = VTXPOSCOL{ _float3(+fWidth, -fHeight, -fDepth), m_vColor };
    pVertices[14] = VTXPOSCOL{ _float3(+fWidth, -fHeight, +fDepth), m_vColor };
    pVertices[15] = VTXPOSCOL{ _float3(-fWidth, -fHeight, +fDepth), m_vColor };
    // ¿ÞÂÊ¸é
    pVertices[16] = VTXPOSCOL{ _float3(-fWidth, -fHeight, +fDepth), m_vColor };
    pVertices[17] = VTXPOSCOL{ _float3(-fWidth, +fHeight, +fDepth), m_vColor };
    pVertices[18] = VTXPOSCOL{ _float3(-fWidth, +fHeight, -fDepth), m_vColor };
    pVertices[19] = VTXPOSCOL{ _float3(-fWidth, -fHeight, -fDepth), m_vColor };
    // ¿À¸¥ÂÊ¸é
    pVertices[20] = VTXPOSCOL{ _float3(+fWidth, -fHeight, -fDepth), m_vColor };
    pVertices[21] = VTXPOSCOL{ _float3(+fWidth, +fHeight, -fDepth), m_vColor };
    pVertices[22] = VTXPOSCOL{ _float3(+fWidth, +fHeight, +fDepth), m_vColor };
    pVertices[23] = VTXPOSCOL{ _float3(+fWidth, -fHeight, +fDepth), m_vColor };

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

HRESULT CVIBuffer_Cube_Color::Initialize(void* pArg)
{
    if (FAILED(Super::Initialize(pArg)))
        return E_FAIL;

    return S_OK;
}

CVIBuffer_Cube_Color* CVIBuffer_Cube_Color::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void *pArg)
{
    CVIBuffer_Cube_Color* pInstance = new CVIBuffer_Cube_Color(pDevice, pDeviceContext);
    if (FAILED(pInstance->Initialize_Prototype(pArg)))
    {
        MSG_BOX("CVIBuffer_Cube_Color::Create, Failed");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CComponent* CVIBuffer_Cube_Color::Clone(void* pArg)
{
    CVIBuffer_Cube_Color* pInstance = new CVIBuffer_Cube_Color(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("CVIBuffer_Cube_Color::Create, Clone");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CVIBuffer_Cube_Color::Free()
{
    Super::Free();
}
