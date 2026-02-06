#pragma once
#include "Component.h"

NS_BEGIN(Engine)

template<typename T>
class CConstant_Buffer;

class CFxShaderVariant;

class ENGINE_DLL CShader final : public CComponent
{
	using Super = CComponent;
public:
	constexpr static EComponentType _ID = EComponentType::SHADER;
	typedef struct tagShaderOriginDesc
	{
		const _tchar* pShaderFilePath = { nullptr };
		EVtxLayout eLayout = EVtxLayout::NONE;
	}SHADER_ORIGIN_DESC;
private:
	CShader(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CShader(const CShader& rhs); 
	virtual ~CShader() = default;
public:
	virtual HRESULT Initialize_Prototype(void *pArg);
	virtual HRESULT Initialize(void* pArg);
public:
	void Apply();
	void Set_Pass(_uint iPass) { m_iPass = iPass; }

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

	HRESULT Bind_RenderTargetTexture(ID3D11ShaderResourceView* pTexture);
	HRESULT Bind_RenderTargetDiffuseTexture(ID3D11ShaderResourceView* pTexture);
	HRESULT Bind_RenderTargetNormalTexture(ID3D11ShaderResourceView* pTexture);
	HRESULT Bind_RenderTargetShadeTexture(ID3D11ShaderResourceView* pTexture);
	HRESULT Bind_RenderTargetDepthTexture(ID3D11ShaderResourceView* pTexture);
	HRESULT Bind_RenderTargetSceneTexture(ID3D11ShaderResourceView* pTexture); // 이펙트 전용 후처리 쉐이더 텍스처
	HRESULT Bind_DiffuseTexture(ID3D11ShaderResourceView* pDiffuse);
	void Bind_MaterialTextures(ID3D11ShaderResourceView** ppSRV, _uint iCount);
	void Bind_MaterialMask(_short iMask);
	void Bind_MaterialData(const SHADER_MATERIALDESC& desc);
	void Bind_MaterialInstanceData(const SHADER_MI_DESC& desc);
	void Bind_EffectData(const SHADER_EFFECT_DESC& desc);
	void Bind_GlobalMask(_uint iMask);
	HRESULT Bind_DefaultTexture(ID3D11ShaderResourceView* pSRV);
	HRESULT Bind_CubeTexture(ID3D11ShaderResourceView* pSRV);
	HRESULT Bind_DefaultTextures(ID3D11ShaderResourceView** ppSRV, _uint iCount = 8);
	HRESULT Bind_TransformData(const SHADER_TRANSFORMDESC& trnasformDesc);
	HRESULT Bind_TransformData(const Matrix& matTransform);
	void Bind_TransformTexture(ID3D11ShaderResourceView* pSRV);
	HRESULT Bind_BoneData(const SHADER_BONEDESC& boneDesc);
	void Bind_KeyFrameData(const SHADER_KEYFRAMEDESC& keyframeDesc);

private:
	void Create_ConstantBuffer();
	void Clear_ConstantBuffer();
private:
	_uint m_iPass = { 0 };
	
	CFxShaderVariant* m_pVariant{ nullptr };
	ID3D11Device* m_pDevice{ nullptr };
	ID3D11DeviceContext* m_pDeviceContext{ nullptr };

	// TODO - 나중에 풀링 / 프레임소스
	CConstant_Buffer<SHADER_BONEDESC>* m_pBone_CBuffer{ nullptr };
	CConstant_Buffer<SHADER_MATERIALDESC>* m_pMaterial_CBuffer{ nullptr };
	CConstant_Buffer<SHADER_MI_DESC>* m_pMI_CBuffer{ nullptr };
	CConstant_Buffer<SHADER_TRANSFORMDESC>* m_pTransform_CBuffer{ nullptr };
	CConstant_Buffer<SHADER_KEYFRAMEDESC>* m_pKeyFrame_CBuffer{ nullptr };
	CConstant_Buffer<SHADER_EFFECT_DESC>* m_pEffect_CBuffer{ nullptr };
public:
	static CShader* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END