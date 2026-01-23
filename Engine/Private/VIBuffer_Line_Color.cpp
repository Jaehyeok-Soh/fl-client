#include "VIBuffer_Line_Color.h"
#include "GameInstance.h"


CVIBuffer_Line_Color::CVIBuffer_Line_Color(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
    : Super(pDevice, pDeviceContext)
{
}

CVIBuffer_Line_Color::CVIBuffer_Line_Color(const CVIBuffer_Line_Color& rhs)
    : Super(rhs)
    , m_vColor(rhs.m_vColor)
    , m_iMaxLineCount(rhs.m_iMaxLineCount)
{
}

HRESULT CVIBuffer_Line_Color::Initialize_Prototype(void* pArg)
{
    // VB/IB Usage 세팅 (부모가 Usage/CPUAccessFlag 결정)
    VIBUFFER_LINE_ORIGIN_DESC* pDesc = nullptr;
    if (pArg)
        pDesc = static_cast<VIBUFFER_LINE_ORIGIN_DESC*>(pArg);

    // 강제: 라인 디버그용은 Dynamic VB + Immutable IB 조합 추천
    VIBUFFER_ORIGIN_DESC baseDesc{};
    baseDesc.VB_Usage = D3D11_USAGE_DYNAMIC;
    baseDesc.IB_Usage = D3D11_USAGE_IMMUTABLE;

    if (FAILED(Super::Initialize_Prototype(&baseDesc)))
        return E_FAIL;

    if (pDesc)
    {
        m_iMaxLineCount = pDesc->iMaxLineCount;
        m_vColor = pDesc->vColor;
    }
    else
    {
        m_iMaxLineCount = 2048;
        m_vColor = { 0.f, 1.f, 0.f, 1.f };
    }

    // ---- 공통 세팅 ----
    m_iVertexBufferCount = 1;
    m_iVertexStride = sizeof(VTXPOSCOL);
    m_iVertexCount = m_iMaxLineCount * 2;   // 라인 N개 → 정점 2N
    m_iIndexStride = 2;
    m_eIndexFormat = DXGI_FORMAT_R16_UINT;
    m_iIndexCount = m_iVertexCount;        // 0,1,2,3,... 그대로 쓴다
    m_ePrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_LINELIST;

    // ---- VB 생성 (Dynamic) ----
    {
        D3D11_BUFFER_DESC desc{};
        desc.ByteWidth = m_iVertexStride * m_iVertexCount;
        desc.Usage = m_VB_Usage;         // D3D11_USAGE_DYNAMIC
        desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        desc.CPUAccessFlags = m_VB_CPUAccesFlag;  // D3D11_CPU_ACCESS_WRITE
        desc.MiscFlags = 0;
        desc.StructureByteStride = m_iVertexStride;

        // 초기에는 0으로 채움
        std::unique_ptr<VTXPOSCOL[]> vertices(new VTXPOSCOL[m_iVertexCount]);
        ZeroMemory(vertices.get(), sizeof(VTXPOSCOL) * m_iVertexCount);

        D3D11_SUBRESOURCE_DATA init{};
        init.pSysMem = vertices.get();

        if (FAILED(m_pDevice->CreateBuffer(&desc, &init, &m_pVB)))
            return E_FAIL;
    }

    // ---- IB 생성 (Immutable) ----
    {
        D3D11_BUFFER_DESC desc{};
        desc.ByteWidth = m_iIndexStride * m_iIndexCount;
        desc.Usage = m_IB_Usage;            // D3D11_USAGE_IMMUTABLE
        desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
        desc.CPUAccessFlags = m_IB_CPUAccesFlag;     // 0
        desc.MiscFlags = 0;
        desc.StructureByteStride = m_iIndexStride;

        std::unique_ptr<_ushort[]> indices(new _ushort[m_iIndexCount]);
        for (_uint i = 0; i < m_iIndexCount; ++i)
            indices[i] = static_cast<_ushort>(i);

        D3D11_SUBRESOURCE_DATA init{};
        init.pSysMem = indices.get();

        if (FAILED(m_pDevice->CreateBuffer(&desc, &init, &m_pIB)))
            return E_FAIL;
    }

    // 디버그/피킹용 저장
    m_pVertexPositions = new Vec3[m_iVertexCount];
    ZeroMemory(m_pVertexPositions, sizeof(Vec3) * m_iVertexCount);

    return S_OK;
}

HRESULT CVIBuffer_Line_Color::Initialize(void* pArg)
{
    if (FAILED(Super::Initialize(pArg)))
        return E_FAIL;
    return S_OK;
}

// 폴리곤 윤곽: points 0..n-1 → (0-1,1-2,...,n-2,n-1, n-1-0)
HRESULT CVIBuffer_Line_Color::Update_PolygonEdges(const std::vector<Vec3>& points)
{
    const _uint n = static_cast<_uint>(points.size());
    if (n < 2)
        return S_OK; // 그릴 게 없음

    const _uint lineCount = n; // 마지막-첫번째 포함해서 N개
    if (lineCount > m_iMaxLineCount)
        return E_FAIL; // capacity 초과 (필요하면 재생성 로직 추가)

    // 이번 프레임에 실제로 사용할 인덱스 수 업데이트
    m_iIndexCount = lineCount * 2;
    m_iVertexCount = m_iIndexCount;

    D3D11_MAPPED_SUBRESOURCE mapped{};
    if (FAILED(m_pDeviceContext->Map(m_pVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped)))
        return E_FAIL;

    VTXPOSCOL* pVerts = reinterpret_cast<VTXPOSCOL*>(mapped.pData);

    for (_uint i = 0; i < lineCount; ++i)
    {
        const _uint i0 = i;
        const _uint i1 = (i + 1) % n;

        const _uint dst = i * 2;

        pVerts[dst + 0].vPosition = points[i0];
        pVerts[dst + 0].vColor = m_vColor;
        pVerts[dst + 1].vPosition = points[i1];
        pVerts[dst + 1].vColor = m_vColor;

        m_pVertexPositions[dst + 0] = points[i0];
        m_pVertexPositions[dst + 1] = points[i1];
    }

    m_pDeviceContext->Unmap(m_pVB, 0);
    return S_OK;
}

// 임의 라인 집합: lineVertices = [p0,p1,p2,p3,...] (짝수개) 라고 가정
HRESULT CVIBuffer_Line_Color::Update_Lines(const std::vector<Vec3>& lineVertices)
{
    const _uint vCount = static_cast<_uint>(lineVertices.size());
    if (vCount == 0 || (vCount & 1))
        return E_FAIL; // 라인 수 *2 여야 함

    const _uint lineCount = vCount / 2;
    if (lineCount > m_iMaxLineCount)
        return E_FAIL;

    m_iIndexCount = vCount;
    m_iVertexCount = vCount;

    D3D11_MAPPED_SUBRESOURCE mapped{};
    if (FAILED(m_pDeviceContext->Map(m_pVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped)))
        return E_FAIL;

    VTXPOSCOL* pVerts = reinterpret_cast<VTXPOSCOL*>(mapped.pData);

    for (_uint i = 0; i < vCount; ++i)
    {
        pVerts[i].vPosition = lineVertices[i];
        pVerts[i].vColor = m_vColor;
        m_pVertexPositions[i] = lineVertices[i];
    }

    m_pDeviceContext->Unmap(m_pVB, 0);
    return S_OK;
}

_bool CVIBuffer_Line_Color::IntersectWithPlane(OUT Vec3& vOut)
{
    const _uint iTriangleCount = m_iIndexCount / 3;
    _uint iCout = { 0 };
    for (_uint i = 0; i < iTriangleCount; ++i)
    {
        if (m_pGameInstance->IntersectrayWithTriangle_Local(m_pVertexPositions[m_pIndices[iCout++]],
            m_pVertexPositions[m_pIndices[iCout++]],
            m_pVertexPositions[m_pIndices[iCout++]], vOut))
        {
            return true;
        }
    }
    return false;
}

CVIBuffer_Line_Color* CVIBuffer_Line_Color::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg)
{
    CVIBuffer_Line_Color* pInstance = new CVIBuffer_Line_Color(pDevice, pDeviceContext);
    if (FAILED(pInstance->Initialize_Prototype(pArg)))
    {
        MSG_BOX("CVIBuffer_Line_Color::Create, Failed");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CComponent* CVIBuffer_Line_Color::Clone(void* pArg)
{
    CVIBuffer_Line_Color* pInstance = new CVIBuffer_Line_Color(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("CVIBuffer_Line_Color::Clone, Failed");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CVIBuffer_Line_Color::Free()
{
    Super::Free();
}
