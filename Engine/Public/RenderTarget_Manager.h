#pragma once
#include "RenderTarget.h"

NS_BEGIN(Engine)

enum class ERenderTarget : _uint
{
	Diffuse,
	Normal,
	Shade,
	SpecularMask,
	Specular,
	Depth,
	SSAO_HalfPing,
	SSAO_HalfPong,
	Scene,	// 유니티에서 SceneTexture라고 함. Effect 전용
	END,
};

enum class EMRTLayer : _uint
{
	GameObjects,
	LightAcc,
	Effect,
	SSAO_Half,
	SSAO_BlurH,
	SSAO_BlurV,
	END,
};

class CRenderTarget_Manager final : public CBase
{
	using Super = CBase;
private:
	CRenderTarget_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CRenderTarget_Manager() = default;

public:
	HRESULT Add_RenderTarget(ERenderTarget eTarget, const CRenderTarget::RENDERTARGET_DESC* pDesc);
	HRESULT Add_MRT(EMRTLayer eMRTLayer, ERenderTarget eTarget);
	HRESULT Begin_MRT(EMRTLayer eMRTLayer);
	HRESULT End_MRT();
	HRESULT Bind_ShaderResource(ERenderTarget eTarget, class CShader* pShader);
	HRESULT Copy_BackBufferResource(ERenderTarget eTarget);

public:
	class CRenderTarget* Get_RenderTarget(ERenderTarget eTarget) { return m_arrRenderTargets[ENUM_TO_UINT(eTarget)]; }
	class list<class CRenderTarget*>* Get_MRT(EMRTLayer eMRTLayer) { return &m_arrMRTs[ENUM_TO_UINT(eMRTLayer)]; }

#ifdef _DEBUG
public:
	HRESULT Ready_Debug(ERenderTarget eTarget, _float fX, _float fY, _float fSizeX, _float fSizeY);
	HRESULT Render(EMRTLayer eMRTLayer, class CShader* pShader, class CVIBuffer_Rect_Tex* pVIBuffer);
#endif

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pDeviceContext = { nullptr };
	array<class CRenderTarget*, ENUM_TO_SZET(ERenderTarget::END)>		m_arrRenderTargets;
	array<list<class CRenderTarget*>, ENUM_TO_SZET(EMRTLayer::END)>		m_arrMRTs;

	ID3D11ShaderResourceView* m_pNullSRVs[128]{ nullptr };
	ID3D11RenderTargetView* m_pBackBuffer = { nullptr };
	ID3D11DepthStencilView* m_pDSV = { nullptr };
public:
	static CRenderTarget_Manager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual void Free() override;
};

NS_END