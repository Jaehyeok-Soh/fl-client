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
        m_arrMRTs[ENUM_TO_UINT(eTarget)] = std::move(MRTList);
    }
    else
        pMRTList->push_back(pRenderTarget);

    Safe_AddRef(pRenderTarget);
    return S_OK;
}

HRESULT CRenderTarget_Manager::Begin_MRT(EMRTLayer eMRTLayer)
{
    list<CRenderTarget*>* pMRTList = Get_MRT(eMRTLayer);
    if (nullptr == pMRTList)
        return E_FAIL;

    m_pDeviceContext->OMGetRenderTargets(1, &m_pBackBuffer, &m_pDSV);

    ID3D11RenderTargetView* pRTVs[8]{ nullptr };
    _uint   iRenderTargetCount = { };

    for (auto& pRenderTarget : *pMRTList)
    {
        pRenderTarget->Clear();
        pRTVs[iRenderTargetCount++] = pRenderTarget->Get_RTV();
    }

    m_pDeviceContext->OMSetRenderTargets(iRenderTargetCount, pRTVs, m_pDSV);

    return S_OK;
}

HRESULT CRenderTarget_Manager::End_MRT()
{
    ID3D11RenderTargetView* pRenderTargets[8] = {
        m_pBackBuffer
    };

    m_pDeviceContext->OMSetRenderTargets(8, pRenderTargets, m_pDSV);

    Safe_Release(m_pBackBuffer);
    Safe_Release(m_pDSV);

    return S_OK;
}

HRESULT CRenderTarget_Manager::Bind_ShaderResource(ERenderTarget eTarget, CShader* pShader)
{
    _uint iIndex = ENUM_TO_UINT(eTarget);

    switch (eTarget)
    {
    case Engine::ERenderTarget::Diffuse:
        return pShader->Bind_RenderTargetDiffuseTexture(m_arrRenderTargets[iIndex]->Get_SRV());
    case Engine::ERenderTarget::Normal:
        return pShader->Bind_RenderTargetNormalTexture(m_arrRenderTargets[iIndex]->Get_SRV());
    case Engine::ERenderTarget::Shade:
        return pShader->Bind_RenderTargetShadeTexture(m_arrRenderTargets[iIndex]->Get_SRV());
    case Engine::ERenderTarget::Depth:
        return pShader->Bind_RenderTargetDepthTexture(m_arrRenderTargets[iIndex]->Get_SRV());
    case Engine::ERenderTarget::Scene:
        return pShader->Bind_RenderTargetSceneTexture(m_arrRenderTargets[iIndex]->Get_SRV());

    default:
        return E_FAIL; 
    }
}

HRESULT CRenderTarget_Manager::Copy_BackBufferResource(ERenderTarget eTarget)
{
    ID3D11Resource* pSrcResource = nullptr;
    m_pBackBuffer->GetResource(&pSrcResource);

    CRenderTarget* pTarget = Get_RenderTarget(eTarget);

    m_pDeviceContext->CopyResource(pTarget->Get_Texture2D(), pSrcResource);

    Safe_Release(pSrcResource);
    return S_OK;
}

#ifdef _DEBUG

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
