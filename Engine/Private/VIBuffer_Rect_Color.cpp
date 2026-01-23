#include "VIBuffer_Rect_Color.h"

CVIBuffer_Rect_Color::CVIBuffer_Rect_Color(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
    : Super(pDevice, pDeviceContext)
{
}

CVIBuffer_Rect_Color::CVIBuffer_Rect_Color(const CVIBuffer_Rect_Color& rhs)
	: Super(rhs)
    , m_vColor(rhs.m_vColor)
{
}

HRESULT CVIBuffer_Rect_Color::Initialize_Prototype(void* pArg)
{
    if (FAILED(Super::Initialize_Prototype(pArg)))
        return E_FAIL;

    if (!pArg)
    {
        m_vColor = { 0.f, 1.f, 0.f, 1.f };
    }
    else
    {
        VIBUFFER_RECT_ORIGIN_DESC* pDesc = static_cast<VIBUFFER_RECT_ORIGIN_DESC*>(pArg);
        m_vColor = pDesc->vColor;
    }

    m_iVertexBufferCount = 1;
    m_iVertexStride = sizeof(VTXPOSCOL);
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

    VTXPOSCOL* pVertices = new VTXPOSCOL[m_iVertexCount];
    ZeroMemory(pVertices, sizeof(VTXPOSCOL) * m_iVertexCount);

    m_pVertexPositions = new Vec3[m_iVertexCount];
    ZeroMemory(m_pVertexPositions, sizeof(Vec3) * m_iVertexCount);

    m_pVertexPositions[0] = pVertices[0].vPosition = Vec3(-0.5f, 0.5f, 0.f);
    pVertices[0].vColor = m_vColor;

    m_pVertexPositions[1] = pVertices[1].vPosition = Vec3(0.5f, 0.5f, 0.f);
    pVertices[1].vColor = m_vColor;

    m_pVertexPositions[2] = pVertices[2].vPosition = Vec3(0.5f, -0.5f, 0.f);
    pVertices[2].vColor = m_vColor;

    m_pVertexPositions[3] = pVertices[3].vPosition = Vec3(-0.5f, -0.5f, 0.f);
    pVertices[3].vColor = m_vColor;

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

HRESULT CVIBuffer_Rect_Color::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

CVIBuffer_Rect_Color* CVIBuffer_Rect_Color::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg)
{
    CVIBuffer_Rect_Color* pInstance = new CVIBuffer_Rect_Color(pDevice, pDeviceContext);
    if (FAILED(pInstance->Initialize_Prototype(pArg)))
    {
        MSG_BOX("CVIBuffer_Rect_Color::Create, Failed");
        Safe_Release(pInstance);
    }
	return pInstance;
}

CComponent* CVIBuffer_Rect_Color::Clone(void* pArg)
{
    CVIBuffer_Rect_Color* pInstance = new CVIBuffer_Rect_Color(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("CVIBuffer_Rect_Color::Create, Clone");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CVIBuffer_Rect_Color::Free()
{
	Super::Free();
}
