#include "Engine_pch.h"
#include "RenderTarget_Manager.h"
#include "Shader.h"
#include "RenderTarget.h"

CRenderTarget_Manager::CRenderTarget_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
    : m_pDevice{ pDevice }
    , m_pDeviceContext{ pDeviceContext }
{
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pDeviceContext);
    m_arrRenderTargets.fill(nullptr);
}

HRESULT CRenderTarget_Manager::Add_RenderTarget(ERenderTarget eTarget, const CRenderTarget::RENDERTARGET_DESC* pDesc)
{
    if (nullptr != Get_RenderTarget(eTarget))
        return E_FAIL;

    CRenderTarget* pRenderTarget = CRenderTarget::Create(m_pDevice, m_pDeviceContext, pDesc);
    if (nullptr == pRenderTarget)
        return E_FAIL;

    m_arrRenderTargets[ENUM_TO_UINT(eTarget)] = pRenderTarget;
    return S_OK;
}

HRESULT CRenderTarget_Manager::Add_MRT(EMRTLayer eMRTLayer, ERenderTarget eTarget)
{
    CRenderTarget* pRenderTarget = Get_RenderTarget(eTarget);
    if (nullptr == pRenderTarget)
        return E_FAIL;

    list<CRenderTarget*>* pMRTList = Get_MRT(eMRTLayer);
    if (nullptr == pMRTList)
    {
        list<CRenderTarget*>    MRTList;

        MRTList.push_back(pRenderTarget);
        m_arrMRTs[ENUM_TO_UINT(eMRTLayer)] = std::move(MRTList);
    }
    else
        pMRTList->push_back(pRenderTarget);

    Safe_AddRef(pRenderTarget);
    return S_OK;
}

HRESULT CRenderTarget_Manager::Begin_MRT(EMRTLayer eMRTLayer, _bool bClear, _bool bUseDSV)
{
    list<CRenderTarget*>* pMRTList = Get_MRT(eMRTLayer);
    if (nullptr == pMRTList)
        return E_FAIL;

    // 원본 랜더타겟 백버퍼 RTV, DSV를 저장
    m_pDeviceContext->OMGetRenderTargets(1, &m_pBackBuffer, &m_pDSV);

    // VS와 PS의 SRV를 0 ~ 127번까지 nullptr로 채워준다
    // Wraning이 뜨는데, 그 의미는
    // 아래 RTV로 세팅하려는 리소스가 이미 다른 셰이더 단계에 바인딩된 상태
    // 때문에 같은 리소스를 동시에 읽기(SRV), 쓰기(RTV/UAV/DSV)로 묶는게 금지
    // 그래서 디버그 레이어가 충돌을 막기 위해 강제로 NULL 언바인드 했다는 경고문이 쏟아진다.
    m_pDeviceContext->VSSetShaderResources(0, 128, m_pNullSRVs);
    m_pDeviceContext->PSSetShaderResources(0, 128, m_pNullSRVs);

    ID3D11DepthStencilView* pDSV = bUseDSV ? m_pDSV : nullptr;
    ID3D11RenderTargetView* pRTVs[8]{ nullptr };
    _uint   iRenderTargetCount = {0};

    for (auto& pRenderTarget : *pMRTList)
    {
        if(bClear == true)
            pRenderTarget->Clear();
        pRTVs[iRenderTargetCount++] = pRenderTarget->Get_RTV();
    }

    m_pDeviceContext->OMSetRenderTargets(iRenderTargetCount, pRTVs, pDSV);
    return S_OK;
}

HRESULT CRenderTarget_Manager::End_MRT()
{
    ID3D11RenderTargetView* pRenderTargets[8]{nullptr};
    pRenderTargets[0] = m_pBackBuffer;

    m_pDeviceContext->OMSetRenderTargets(8, pRenderTargets, m_pDSV);
    Safe_Release(m_pBackBuffer);
    Safe_Release(m_pDSV);

    return S_OK;
}

HRESULT CRenderTarget_Manager::Bind_ShaderResource(ERenderTarget eTarget, CShader* pShader)
{
    _uint iIndex = ENUM_TO_UINT(eTarget);
    EFXSRV eSlot{EFXSRV::COUNT};

    switch (eTarget)
    {
    case Engine::ERenderTarget::Diffuse:
        eSlot = EFXSRV::RT_Diffuse; break;
    case Engine::ERenderTarget::Normal:
        eSlot = EFXSRV::RT_Normal; break;
    case Engine::ERenderTarget::Shade:
        eSlot = EFXSRV::RT_Shade; break;
    case Engine::ERenderTarget::SpecularMask:
        eSlot = EFXSRV::RT_SpecularMask; break;
    case Engine::ERenderTarget::Specular:
        eSlot = EFXSRV::RT_Specular; break;
    case Engine::ERenderTarget::Depth:
        eSlot = EFXSRV::RT_Depth; break;
    case Engine::ERenderTarget::ObjectInfo:
        eSlot = EFXSRV::RT_ObjectInfo; break;
    case Engine::ERenderTarget::Emissive:
        eSlot = EFXSRV::RT_Emissive; break;
    case Engine::ERenderTarget::SSAO_Ping:
    case Engine::ERenderTarget::SSAO_Pong:
    case Engine::ERenderTarget::SSAO_Full:
        eSlot = EFXSRV::RT_AO; break;
    case Engine::ERenderTarget::SceneHDR:
        eSlot = EFXSRV::RT_SceneHDR; break;
    case Engine::ERenderTarget::SceneHDR_Copy:
        eSlot = EFXSRV::RT_SceneHDR_Copy; break;
    case Engine::ERenderTarget::Bloom_Ping:
    case Engine::ERenderTarget::Bloom_Pong:
        eSlot = EFXSRV::RT_Bloom; break;
    default:
        return E_FAIL; 
    }

    return pShader->Bind_SRV(eSlot, m_arrRenderTargets[iIndex]->Get_SRV());
}

HRESULT CRenderTarget_Manager::Copy_SceneHDRResource(ERenderTarget eTarget)
{
    ID3D11Resource* pSrcResource = nullptr;
    m_arrRenderTargets[ENUM_TO_UINT(ERenderTarget::SceneHDR)]->Get_RTV()->GetResource(&pSrcResource);

    CRenderTarget* pTarget = Get_RenderTarget(eTarget);

    m_pDeviceContext->CopyResource(pTarget->Get_Texture2D(), pSrcResource);

    Safe_Release(pSrcResource);
    return S_OK;
}

#ifdef _DEBUG

ID3D11ShaderResourceView* CRenderTarget_Manager::Get_RenderTargetSRV(ERenderTarget eTarget)
{
    return m_arrRenderTargets[ENUM_TO_UINT(eTarget)]->Get_SRV();
}

HRESULT CRenderTarget_Manager::Ready_Debug(ERenderTarget eTarget, _float fX, _float fY, _float fSizeX, _float fSizeY)
{
    CRenderTarget* pRenderTarget = Get_RenderTarget(eTarget);
    if (nullptr == pRenderTarget)
        return E_FAIL;

    pRenderTarget->Ready_Debug(fX, fY, fSizeX, fSizeY);
    return S_OK;
}

HRESULT CRenderTarget_Manager::Render(EMRTLayer eMRTLayer, CShader* pShader, CVIBuffer_Rect_Tex* pVIBuffer)
{
    list<CRenderTarget*>* pMRTList = Get_MRT(eMRTLayer);
    if (nullptr == pMRTList)
        return E_FAIL;

    for (auto& pRenderTarget : *pMRTList)
    {
        pRenderTarget->Render(pShader, pVIBuffer);
    }

    return S_OK;
}

#endif

CRenderTarget_Manager* CRenderTarget_Manager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
    return new CRenderTarget_Manager(pDevice, pDeviceContext);
}

void CRenderTarget_Manager::Free()
{
    Super::Free();

    for (auto& list : m_arrMRTs)
    {
        for (auto& pRenderTarget : list)
            Safe_Release(pRenderTarget);
        list.clear();
    }

    for (auto& pRenderTarget : m_arrRenderTargets)
        Safe_Release(pRenderTarget);
    m_arrRenderTargets.fill(nullptr);

    Safe_Release(m_pDevice);
    Safe_Release(m_pDeviceContext);
}
