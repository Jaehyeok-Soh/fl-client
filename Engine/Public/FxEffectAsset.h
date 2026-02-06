#pragma once
#include "Base.h"

NS_BEGIN(Engine)

typedef struct tagFxBindingCache
{
	// Global ConstantBuffer Handle
	ID3DX11EffectConstantBuffer* pCB_Global{ nullptr };
	ID3DX11EffectConstantBuffer* pCB_Inv{ nullptr };
	ID3DX11EffectConstantBuffer* pCB_Light{ nullptr };
	
	ID3DX11EffectConstantBuffer* pCB_Transform{ nullptr };
	ID3DX11EffectConstantBuffer* pCB_Material{ nullptr };
	ID3DX11EffectConstantBuffer* pCB_MaterialInst{ nullptr };

	ID3DX11EffectConstantBuffer* pCB_Keyframe{ nullptr };
	ID3DX11EffectConstantBuffer* pCB_Bone{ nullptr };
	ID3DX11EffectConstantBuffer* pCB_Effect{ nullptr };

	// Scalars
	ID3DX11EffectScalarVariable* pG_MaterialMask{ nullptr };
	ID3DX11EffectScalarVariable* pG_TextureMask{ nullptr };

	// SRVs
	ID3DX11EffectShaderResourceVariable* pSRV_RT{ nullptr };
	ID3DX11EffectShaderResourceVariable* pSRV_RT_Diffuse{ nullptr };
	ID3DX11EffectShaderResourceVariable* pSRV_RT_Normal{ nullptr };
	ID3DX11EffectShaderResourceVariable* pSRV_RT_Shade{ nullptr };
	ID3DX11EffectShaderResourceVariable* pSRV_RT_Depth{ nullptr };
	// Effect Shader 전용 텍스처
	ID3DX11EffectShaderResourceVariable* pSRV_RT_Scene{ nullptr };	// 유니티에서 SceneTexture라고 하더라.

	ID3DX11EffectShaderResourceVariable* pSRV_Transform{ nullptr };
	ID3DX11EffectShaderResourceVariable* pSRV_Material{ nullptr };
	
	ID3DX11EffectShaderResourceVariable* pSRV_Textures{ nullptr };
	ID3DX11EffectShaderResourceVariable* pSRV_Cube{ nullptr };
}FXBINDING_CACHE;

class CFxEffectAsset final : public CBase
{
	using Super = CBase;
private:
	CFxEffectAsset(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CFxEffectAsset() = default;

	HRESULT Initialize();
public:
	// Effect 컴파일 + Pass 메타 수집 + BindingCache 채우기
	HRESULT Load_EffectFromFile(const std::wstring& path);
	ID3DX11EffectPass* Get_Pass(_uint iPass, _uint iTechnique = 0);
	ID3DX11Effect* Get_Effect() { return m_pEffect; }
	const vector<tagPass>& Get_Passes(_uint iTechnique = 0) const { return m_vecTechniques[iTechnique].vecPasses; }
	const wstring& Get_Path() const { return m_wstrPath; }
	FXBINDING_CACHE* Get_BindingCache() { return &m_tBindingCache; }

	ID3DX11EffectVariable* Get_Variable(string name);
	ID3DX11EffectScalarVariable* Get_Scalar(string name);
	ID3DX11EffectVectorVariable* Get_Vector(string name);
	ID3DX11EffectMatrixVariable* Get_Matrix(string name);
	ID3DX11EffectStringVariable* Get_String(string name);
	ID3DX11EffectShaderResourceVariable* Get_SRV(string name);
	ID3DX11EffectRenderTargetViewVariable* Get_RTV(string name);
	ID3DX11EffectDepthStencilViewVariable* Get_DSV(string name);
	ID3DX11EffectUnorderedAccessViewVariable* Get_UAV(string name);
	ID3DX11EffectConstantBuffer* Get_ConstantBuffer(string name);
	ID3DX11EffectShaderVariable* Get_Shader(string name);
	ID3DX11EffectBlendVariable* Get_Blend(string name);
	ID3DX11EffectDepthStencilVariable* Get_DepthStencil(string name);
	ID3DX11EffectRasterizerVariable* Get_Rasterizer(string name);
	ID3DX11EffectSamplerVariable* Get_Sampler(string name);
private:
	void Binding_Cache();
	void Clear_Cache();
private:
	ID3D11Device* m_pDevice{ nullptr };
	ID3D11DeviceContext* m_pDeviceContext{ nullptr };
private:
	ID3DX11Effect* m_pEffect{ nullptr };
	wstring m_wstrPath{ L"" };
	D3DX11_EFFECT_DESC m_tEffectDesc{};
	vector<TECHNIQUE> m_vecTechniques;
	FXBINDING_CACHE m_tBindingCache;
public:
	static CFxEffectAsset* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual void Free() override;
};

NS_END