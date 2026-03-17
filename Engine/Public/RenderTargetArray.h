#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class CRenderTargetArray final : public CBase
{
	using Super = CBase;
public:
	typedef struct tagRenderTargetArrayDesc
	{
		DXGI_FORMAT ePixelFormat = DXGI_FORMAT_UNKNOWN;
		_uint iWidth{ 0 };
		_uint iHeight{ 0 };
		_uint iArraySize{ 1 };
		Vec4 vClearColor{ Vec4::Zero };
	}RENDERTARGET_ARR_DESC;
private:
	CRenderTargetArray(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CRenderTargetArray() = default;

	HRESULT Initialize(const RENDERTARGET_ARR_DESC& desc);
public:
	HRESULT Bind_Slice(_uint iSlice, ID3D11DepthStencilView* pDSV);
	HRESULT Clear(_uint iSlice);

	_uint Get_ArraySize() const { return m_tDesc.iArraySize; }
	_uint Get_Width() const { return m_tDesc.iWidth; }
	_uint Get_Height() const { return m_tDesc.iHeight; }
	ID3D11RenderTargetView* Get_RTV(_uint iSlice) const
	{
		if (iSlice >= m_vecRTVs.size())
			return nullptr;

		return m_vecRTVs[iSlice];
	}
	ID3D11ShaderResourceView* Get_SRV() const { return m_pSRV; }
	ID3D11Texture2D* Get_Texture2D() const { return m_pTexture; }
private:
	HRESULT Create_Texture();
	HRESULT Create_SRV();
	HRESULT Create_RTVs();
private:
	ID3D11Device* m_pDevice{ nullptr };
	ID3D11DeviceContext* m_pDeviceContext{ nullptr };

	ID3D11Texture2D* m_pTexture{ nullptr };
	ID3D11ShaderResourceView* m_pSRV{ nullptr };
	vector<ID3D11RenderTargetView*> m_vecRTVs;

	RENDERTARGET_ARR_DESC m_tDesc{};
public:
	CRenderTargetArray* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const RENDERTARGET_ARR_DESC& desc);
	virtual void Free() override;
};

NS_END