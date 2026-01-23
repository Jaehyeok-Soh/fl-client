#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class CGraphic_Device final : public CBase
{
	using Super = CBase;
private:
	CGraphic_Device();
	virtual ~CGraphic_Device() = default;
public:
	HRESULT Initialize(const ENGINE_DESC&Engine_Desc, _Inout_ ID3D11Device** ppDevice, _Inout_ ID3D11DeviceContext** ppContext);
	
	HRESULT Resize(D3D11_VIEWPORT viewport);
	HRESULT Copy_BackBufferTexture(ID3D11Texture2D** ppTexture);

	// Render Begin
	HRESULT Clear_BackBuffer_View(const Vec4* pClearColor);
	HRESULT Clear_DepthStencil_View();
	// Render End
	HRESULT Present();
public:
	ID3D11Device*			Get_Device()		{ return m_pDevice; }
	ID3D11DeviceContext*	Get_DeviceContext()	{ return m_pDeviceContext; }
private:
	ID3D11Device*				m_pDevice				= { nullptr };
	ID3D11DeviceContext*		m_pDeviceContext		= { nullptr };
	IDXGISwapChain*				m_pSwapChain			= { nullptr };
	ID3D11RenderTargetView*		m_pBackBufferRTV		= { nullptr };
	ID3D11DepthStencilView*		m_pDepthStencilView		= { nullptr };

	ID3D11Texture2D*			m_pBackBufferTexture	= { nullptr };
private:
	HRESULT						Ready_SwapChain(HWND hWnd, WINMODE isWindowed, _uint iWinCX, _uint iWinCY);
	HRESULT						Ready_BackBufferRenderTargetView();
	HRESULT						Ready_DepthStencilView(_uint iWinCX, _uint iWinCY);
private:

public:
	virtual void				Free() override;
	static CGraphic_Device*		Create(const ENGINE_DESC& Engine_Desc, _Inout_ ID3D11Device** ppDevice, _Inout_ ID3D11DeviceContext** ppContext);
};

NS_END
