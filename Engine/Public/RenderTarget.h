#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class CRenderTarget final : public CBase
{
	using Super = CBase;
public:
	typedef struct tagRenderTargetDesc
	{
		_uint iWidth = {};
		_uint iHeight = {};
		DXGI_FORMAT ePixelFormat = {};
		Vec4 vClearColor = {};
	}RENDERTARGET_DESC;
private:
	CRenderTarget(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CRenderTarget() = default;

	HRESULT Initialize(const RENDERTARGET_DESC* pDesc);
public:
	ID3D11RenderTargetView* Get_RTV() const { return m_pRTV; }
	ID3D11ShaderResourceView* Get_SRV() const { return m_pSRV; }
	void Clear();

#ifdef _DEBUG
	HRESULT Ready_Debug(_float fX, _float fY, _float fSizeX, _float fSizeY);
	HRESULT Render(class CShader* pShader, class CVIBuffer_Rect_Tex* pVIBuffer);
	Matrix m_matWorld = {};
#endif

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pDeviceContext = { nullptr };
	ID3D11Texture2D* m_pTexture2D = { nullptr };
	ID3D11RenderTargetView* m_pRTV = { nullptr };
	ID3D11ShaderResourceView* m_pSRV = { nullptr };

private:
	DXGI_FORMAT m_ePixelFormat = {};
	_uint m_iWidth = {};
	_uint m_iHeight = {};
	Vec4 m_vClearColor = {};
public:
	static CRenderTarget* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const RENDERTARGET_DESC* pDesc);
	virtual void Free() override;
};

NS_END