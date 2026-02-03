#include "Engine_pch.h"
#include "VIBuffer_Particle_Rect.h"
#include "GameInstance.h"

CVIBuffer_Particle_Rect::CVIBuffer_Particle_Rect(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
    : Super(pDevice, pDeviceContext)
{
}

CVIBuffer_Particle_Rect::CVIBuffer_Particle_Rect(const CVIBuffer_Particle_Rect& rhs)
    : Super(rhs)
{
}

HRESULT CVIBuffer_Particle_Rect::Initialize_Prototype(void* pArg)
{
    if (FAILED(Super::Initialize_Prototype(pArg)))
        return E_FAIL;

    m_iVertexStride = sizeof(VTXPOSTEX);
    m_iVertexCount = 4;
    m_iIndexStride = 2;
    m_eIndexFormat = m_iIndexStride == 2 ? DXGI_FORMAT::DXGI_FORMAT_R16_UINT : DXGI_FORMAT::DXGI_FORMAT_R32_UINT;

    m_iIndexCount = 6;
    m_iVertexBufferCount = 2;
    m_ePrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

    const PARTICLE_RECT_ORIGIN_DESC* pParticleDesc = static_cast<const PARTICLE_RECT_ORIGIN_DESC*>(pArg);

    m_iInstanceVertexStride = sizeof(VTXPARTICLE);
    m_iIndexCountPerInstance = m_iIndexCount;

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

    m_pVertexPositions = new Vec3[m_iVertexCount];
    ZeroMemory(m_pVertexPositions, sizeof(Vec3) * m_iVertexCount);

    m_pVertexPositions[0] = pVertices[0].vPosition = Vec3(-0.5f, 0.5f, 0.f);
    pVertices[0].vUV = Vec2(0.0f, 0.f);

    m_pVertexPositions[1] = pVertices[1].vPosition = Vec3(0.5f, 0.5f, 0.f);
    pVertices[1].vUV = Vec2(1.0f, 0.f);

    m_pVertexPositions[2] = pVertices[2].vPosition = Vec3(0.5f, -0.5f, 0.f);
    pVertices[2].vUV = Vec2(1.0f, 1.f);

    m_pVertexPositions[3] = pVertices[3].vPosition = Vec3(-0.5f, -0.5f, 0.f);
    pVertices[3].vUV = Vec2(0.0f, 1.f);

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


//#pragma region INSTANCE_BUFFER
//
//    m_InstanceBufferDesc.ByteWidth = m_iInstanceCount * m_iInstanceVertexStride;
//    m_InstanceBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
//    m_InstanceBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
//    m_InstanceBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
//    m_InstanceBufferDesc.MiscFlags = 0;
//    m_InstanceBufferDesc.StructureByteStride = m_iInstanceVertexStride;
//
//    m_pInstanceVertices = new VTXPARTICLE[m_iInstanceCount];
//    ZeroMemory(m_pInstanceVertices, sizeof(VTXPARTICLE) * m_iInstanceCount);
//
//    m_pSpeeds = new _float[m_iInstanceCount];
//    ZeroMemory(m_pSpeeds, sizeof(_float) * m_iInstanceCount);
//
//    for (size_t i = 0; i < m_iInstanceCount; i++)
//    {
//        _float      fScale = m_pGameInstance->Rand_Float(pParticleDesc->vSize.x, pParticleDesc->vSize.y);
//        m_pSpeeds[i] = m_pGameInstance->Rand_Float(pParticleDesc->vSpeed.x, pParticleDesc->vSpeed.y);
//
//        m_pInstanceVertices[i].vRight = Vec4(fScale, 0.f, 0.f, 0.f);
//        m_pInstanceVertices[i].vUp = Vec4(0.f, fScale, 0.f, 0.f);
//        m_pInstanceVertices[i].vLook = Vec4(0.f, 0.f, fScale, 0.f);
//        m_pInstanceVertices[i].vTranslation = Vec4(
//            m_pGameInstance->Rand_Float(pParticleDesc->vCenter.x - pParticleDesc->vRange.x * 0.5f, pParticleDesc->vCenter.x + pParticleDesc->vRange.x * 0.5f),
//            m_pGameInstance->Rand_Float(pParticleDesc->vCenter.y - pParticleDesc->vRange.y * 0.5f, pParticleDesc->vCenter.y + pParticleDesc->vRange.y * 0.5f),
//            m_pGameInstance->Rand_Float(pParticleDesc->vCenter.z - pParticleDesc->vRange.z * 0.5f, pParticleDesc->vCenter.z + pParticleDesc->vRange.z * 0.5f),
//            1.f
//        );
//
//        m_pInstanceVertices[i].vLifeTime = Vec2(0.f, m_pGameInstance->Rand_Float(pParticleDesc->vLifeTime.x, pParticleDesc->vLifeTime.y));
//    }

    Safe_Delete_Array(pVertices);
#pragma endregion

    return S_OK;
}

HRESULT CVIBuffer_Particle_Rect::Initialize(void* pArg)
{
    if (FAILED(Super::Initialize(pArg)))
        return E_FAIL;

    D3D11_SUBRESOURCE_DATA      InstanceInitialData{};
    InstanceInitialData.pSysMem = m_pInstanceVertices;

    if (FAILED(m_pDevice->CreateBuffer(&m_InstanceBufferDesc, &InstanceInitialData, &m_pVBInstance)))
        return E_FAIL;

    return S_OK;
}

CVIBuffer_Particle_Rect* CVIBuffer_Particle_Rect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
    CVIBuffer_Particle_Rect* pInstance = new CVIBuffer_Particle_Rect(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype(pArg)))
    {
        MSG_BOX("Failed to Created : CVIBuffer_Particle_Rect");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CComponent* CVIBuffer_Particle_Rect::Clone(void* pArg)
{
    CVIBuffer_Particle_Rect* pInstance = new CVIBuffer_Particle_Rect(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CVIBuffer_Particle_Rect");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CVIBuffer_Particle_Rect::Free()
{
    Super::Free();
}