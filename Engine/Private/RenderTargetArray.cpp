#include "Engine_pch.h"
#include "RenderTargetArray.h"
#include "GameInstance.h"

CRenderTargetArray::CRenderTargetArray(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: m_pDevice(pDevice)
	, m_pDeviceContext(pDeviceContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);
}

HRESULT CRenderTargetArray::Initialize(const RENDERTARGET_ARR_DESC& desc)
{
	if (desc.iWidth <= 0 || desc.iHeight <= 0)
		return;

	m_tDesc = desc;

	if (FAILED(Create_Texture()))
		return E_FAIL;

	if (FAILED(Create_SRV()))
		return E_FAIL;

	if (FAILED(Create_RTVs()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderTargetArray::Bind_Slice(_uint iSlice, ID3D11DepthStencilView* pDSV)
{
	if (iSlice >= m_vecRTVs.size() || m_vecRTVs[iSlice] == nullptr)
		return E_FAIL;

	ID3D11RenderTargetView* pRTV = m_vecRTVs[iSlice];
	m_pDeviceContext->OMSetRenderTargets(1, &pRTV, pDSV);
	return S_OK;
}

HRESULT CRenderTargetArray::Clear(_uint iSlice)
{
	if (iSlice >= m_vecRTVs.size() || m_vecRTVs[iSlice] == nullptr)
		return E_FAIL;

	const FLOAT vClear[4] =
	{
		m_tDesc.vClearColor.x,
		m_tDesc.vClearColor.y,
		m_tDesc.vClearColor.z,
		m_tDesc.vClearColor.w
	};

	m_pDeviceContext->ClearRenderTargetView(m_vecRTVs[iSlice], vClear);
	return S_OK;
}

HRESULT CRenderTargetArray::Create_Texture()
{
	D3D11_TEXTURE2D_DESC desc = {};
	desc.Width = m_tDesc.iWidth;
	desc.Height = m_tDesc.iHeight;
	desc.MipLevels = 1;
	desc.ArraySize = m_tDesc.iArraySize;
	desc.Format = m_tDesc.ePixelFormat;
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

	return m_pDevice->CreateTexture2D(&desc, nullptr, &m_pTexture);
}

HRESULT CRenderTargetArray::Create_SRV()
{
	D3D11_SHADER_RESOURCE_VIEW_DESC desc = {};
	desc.Format = m_tDesc.ePixelFormat;
	desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
	desc.Texture2DArray.MostDetailedMip = 0;
	desc.Texture2DArray.MipLevels = 1;
	desc.Texture2DArray.FirstArraySlice = 0;
	desc.Texture2DArray.ArraySize = m_tDesc.iArraySize;

	return m_pDevice->CreateShaderResourceView(m_pTexture, &desc, &m_pSRV);
}

HRESULT CRenderTargetArray::Create_RTVs()
{
	m_vecRTVs.resize(m_tDesc.iArraySize, nullptr);

	for (_uint i = 0; i < m_tDesc.iArraySize; ++i)
	{
		D3D11_RENDER_TARGET_VIEW_DESC desc = {};
		desc.Format = m_tDesc.ePixelFormat;
		desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
		desc.Texture2DArray.MipSlice = 0;
		desc.Texture2DArray.FirstArraySlice = i;
		desc.Texture2DArray.ArraySize = 1;

		if (FAILED(m_pDevice->CreateRenderTargetView(m_pTexture, &desc, &m_vecRTVs[i])))
			return E_FAIL;
	}

	return S_OK;
}

CRenderTargetArray* CRenderTargetArray::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const RENDERTARGET_ARR_DESC& desc)
{
	CRenderTargetArray* pInstance = new CRenderTargetArray(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize(desc)))
	{
		MSG_BOX("CRenderTargetArray::Create, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CRenderTargetArray::Free()
{
	Safe_Release(m_pTexture);
	Safe_Release(m_pSRV);
	for (auto& rtv : m_vecRTVs)
		Safe_Release(rtv);
	m_vecRTVs.clear();
	Safe_Release(m_pDeviceContext);
	Safe_Release(m_pDevice);
	Super::Free();
}
