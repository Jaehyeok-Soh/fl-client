#include "Engine_pch.h"
#include "RenderTarget.h"
#include "Shader.h"
#include "VIBuffer_Rect_Tex.h"

CRenderTarget::CRenderTarget(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: m_pDevice(pDevice)
	, m_pDeviceContext(pDeviceContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);
}

HRESULT CRenderTarget::Initialize(const RENDERTARGET_DESC* pDesc)
{
	m_vClearColor = pDesc->vClearColor;
	m_iWidth = pDesc->iWidth;
	m_iHeight = pDesc->iHeight;
	m_ePixelFormat = pDesc->ePixelFormat;

	D3D11_TEXTURE2D_DESC		TextureDesc{};
	TextureDesc.Width = m_iWidth;
	TextureDesc.Height = m_iHeight;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = m_ePixelFormat;

	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.SampleDesc.Count = 1;

	TextureDesc.Usage = D3D11_USAGE_DEFAULT;
	TextureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

	TextureDesc.CPUAccessFlags = 0;
	TextureDesc.MiscFlags = 0;

	if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &m_pTexture2D)))
		return E_FAIL;
	if (FAILED(m_pDevice->CreateRenderTargetView(m_pTexture2D, nullptr, &m_pRTV)))
		return E_FAIL;
	if (FAILED(m_pDevice->CreateShaderResourceView(m_pTexture2D, nullptr, &m_pSRV)))
		return E_FAIL;

	return S_OK;
}



void CRenderTarget::Clear()
{
	m_pDeviceContext->ClearRenderTargetView(m_pRTV, &m_vClearColor.x);
}

#ifdef _DEBUG

HRESULT CRenderTarget::Ready_Debug(_float fX, _float fY, _float fSizeX, _float fSizeY)
{
	_uint		iViewportCount = { 1 };
	D3D11_VIEWPORT		ViewPortDesc{};

	m_pDeviceContext->RSGetViewports(&iViewportCount, &ViewPortDesc);

	m_matWorld = Matrix::CreateScale(fSizeX, fSizeY, 1.f) * Matrix::CreateTranslation(fX - ViewPortDesc.Width * 0.5f, -fY + ViewPortDesc.Height * 0.5f, 0.f);
	return S_OK;
}

HRESULT CRenderTarget::Render(CShader* pShader, CVIBuffer_Rect_Tex* pVIBuffer)
{
	if (FAILED(pShader->Bind_TransformData(m_matWorld)))
		return E_FAIL;
	if (FAILED(pShader->Bind_RenderTargetTexture(m_pSRV)))
		return E_FAIL;

	pShader->Apply();
	pVIBuffer->Render();
	return S_OK;
}

#endif

CRenderTarget* CRenderTarget::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const RENDERTARGET_DESC* pDesc)
{
	CRenderTarget* pInstance = new CRenderTarget(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize(pDesc)))
	{
		MSG_BOX("Failed to Created : CRenderTarget");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CRenderTarget::Free()
{
	Safe_Release(m_pSRV);
	Safe_Release(m_pRTV);
	Safe_Release(m_pTexture2D);

	Safe_Release(m_pDeviceContext);
	Safe_Release(m_pDevice);
	Super::Free();
}
