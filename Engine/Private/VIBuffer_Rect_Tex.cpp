#include "VIBuffer_Rect_Tex.h"

CVIBuffer_Rect_Tex::CVIBuffer_Rect_Tex(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
    : Super(pDevice, pDeviceContext)
{
}

CVIBuffer_Rect_Tex::CVIBuffer_Rect_Tex(const CVIBuffer_Rect_Tex& rhs)
    : Super(rhs)
{
}

HRESULT CVIBuffer_Rect_Tex::Initialize_Prototype(void* pArg)
{
    if (FAILED(Super::Initialize_Prototype(pArg)))
        return E_FAIL;

    m_iVertexBufferCount = 1;
    m_iVertexStride = sizeof(VTXPOSTEX);
    m_iVertexCount = 4;

    m_iIndexStride = 2;
    m_eIndexFormat = m_iIndexStride == 2 ? DXGI_FORMAT::DXGI_FORMAT_R16_UINT : DXGI_FORMAT::DXGI_FORMAT_R32_UINT;
    m_iIndexCount = 6;
    m_ePrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

#pragma region VERTEX_BUFFER
    D3D11_BUFFER_DESC           VertexBufferDesc{};
    VertexBufferDesc.ByteWidth = m_iVertexStride * m_iVertexCount;
    VertexBufferDesc.Usage = m_VB_Usage;
    VertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    VertexBufferDesc.CPUAccessFlags = m_VB_CPUAccesFlag;
    VertexBufferDesc.MiscFlags = 0;
    VertexBufferDesc.StructureByteStride = m_iVertexStride;

    VTXPOSTEX* pVertices = new VTXPOSTEX[m_iVertexCount];
    ZeroMemory(pVertices, sizeof(VTXPOSTEX) * m_iVertexCount);

    m_pVertexPositions = new _float3[m_iVertexCount];
    ZeroMemory(m_pVertexPositions, sizeof(_float3) * m_iVertexCount);

    m_pVertexPositions[0] = pVertices[0].vPosition = _float3(-0.5f, 0.5f, 0.f);
    pVertices[0].vUV = _float2{ 0.f, 0.f };

    m_pVertexPositions[1] = pVertices[1].vPosition = _float3(0.5f, 0.5f, 0.f);
    pVertices[1].vUV = _float2{ 1.f, 0.f };

    m_pVertexPositions[2] = pVertices[2].vPosition = _float3(0.5f, -0.5f, 0.f);
    pVertices[2].vUV = _float2{ 1.f, 1.f };

    m_pVertexPositions[3] = pVertices[3].vPosition = _float3(-0.5f, -0.5f, 0.f);
    pVertices[3].vUV = _float2{ 0.f, 1.f };

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

    pIndices[0] = 0;
    pIndices[1] = 1;
    pIndices[2] = 2;

    pIndices[3] = 0;
    pIndices[4] = 2;
    pIndices[5] = 3;

    D3D11_SUBRESOURCE_DATA      IndexInitialData{};
    IndexInitialData.pSysMem = pIndices;

    if (FAILED(m_pDevice->CreateBuffer(&IndexBufferDesc, &IndexInitialData, &m_pIB)))
        return E_FAIL;

    Safe_Delete_Array(pIndices);
#pragma endregion

    return S_OK;
}

HRESULT CVIBuffer_Rect_Tex::Initialize(void* pArg)
{
    if (FAILED(Super::Initialize(pArg)))
        return E_FAIL;

    return S_OK;
}

CVIBuffer_Rect_Tex* CVIBuffer_Rect_Tex::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg)
{
    CVIBuffer_Rect_Tex* pInstance = new CVIBuffer_Rect_Tex(pDevice, pDeviceContext);
    if (FAILED(pInstance->Initialize_Prototype(pArg)))
    {
        MSG_BOX("CVIBuffer_Rect_Tex::Create, Failed");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CComponent* CVIBuffer_Rect_Tex::Clone(void* pArg)
{
    CVIBuffer_Rect_Tex* pInstance = new CVIBuffer_Rect_Tex(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("CVIBuffer_Rect_Tex::Create, Clone");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CVIBuffer_Rect_Tex::Free()
{
    Super::Free();
}
