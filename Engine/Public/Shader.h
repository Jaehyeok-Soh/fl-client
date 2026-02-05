#pragma once
#include "Component.h"

NS_BEGIN(Engine)

template<typename T>
class CConstant_Buffer;

typedef struct tagPass
{
	ID3DX11EffectPass* pPass = { nullptr };
	ID3D11InputLayout* pInputLayout = { nullptr };
	wstring wstrName = L"";
	D3DX11_PASS_DESC tDesc = {};
	D3DX11_PASS_SHADER_DESC tVertexShaderDesc = {};
	D3DX11_EFFECT_SHADER_DESC tEffectVsDesc = {};
	vector<D3D11_SIGNATURE_PARAMETER_DESC> vecSignatureDescs;
} PASS;

typedef struct tagTechnique
{
	ID3DX11EffectTechnique* pTechnique = { nullptr };
	wstring wstrName = L"";
	D3DX11_TECHNIQUE_DESC tDesc = {};
	vector<tagPass> vecPasses;
} TECHNIQUE;

class ENGINE_DLL CShader final : public CComponent
{
	using Super = CComponent;
public:
	constexpr static EComponentType _ID = EComponentType::SHADER;
	typedef struct tagShaderOriginDesc
	{
		const _tchar* pShaderFilePath = { nullptr };
		_uint iNumElements = { 0 };
		const D3D11_INPUT_ELEMENT_DESC* pElements = { nullptr };
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
	void Dispatch(_uint iX, _uint iY, _uint iZ);
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
	//void Bind_Compute_EffectSRV();
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
	HRESULT Load_Shader(const D3D11_INPUT_ELEMENT_DESC* pElements, const _uint iNumElements);
	void Create_ConstantBuffer();
	void Clear_ConstantBuffer();
	const wstring &Get_Path() const { return m_wstrPath; }
private:
	_bool m_bInit = { false };
	_uint m_iPass = { 0 };
	wstring m_wstrPath = { L"" };
	ID3DBlob* m_pBlob = { nullptr };
	ID3DX11Effect* m_pEffect = { nullptr };
	D3DX11_EFFECT_DESC m_tEffectDesc = {};
	vector<TECHNIQUE> m_vecTechniques;

	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pDeviceContext = { nullptr };

	// Global
	// ===========  CONSTANT BUFFER   =========== 
	ID3DX11EffectConstantBuffer* m_pGlobalEffectBuffer = { nullptr };
	ID3DX11EffectConstantBuffer* m_pInvEffectBuffer = { nullptr };
	ID3DX11EffectConstantBuffer* m_pGlobalLightEffectBuffer = { nullptr };

	ID3DX11EffectShaderResourceVariable*  m_pMaterialSRV_Effect;
	ID3DX11EffectScalarVariable* m_pMaterialMask_Effect = { nullptr };

	CConstant_Buffer<SHADER_BONEDESC>* m_pBone_CBuffer = { nullptr };
	ID3DX11EffectConstantBuffer* m_pBoneEffectBuffer = { nullptr };

	CConstant_Buffer<SHADER_MATERIALDESC>* m_pMaterial_CBuffer = { nullptr };
	ID3DX11EffectConstantBuffer* m_pMaterialEffectBuffer = { nullptr };

	CConstant_Buffer<SHADER_MI_DESC>* m_pMI_CBuffer = { nullptr };
	ID3DX11EffectConstantBuffer* m_pMI_EffectBuffer = { nullptr };

	CConstant_Buffer<SHADER_TRANSFORMDESC>* m_pTransform_CBuffer = { nullptr };
	ID3DX11EffectConstantBuffer* m_pTransformEffectBuffer = { nullptr };

	CConstant_Buffer<SHADER_KEYFRAMEDESC>* m_pKeyFrame_CBuffer = { nullptr };
	ID3DX11EffectConstantBuffer* m_pKeyFrameEffectBuffer = { nullptr };

	CConstant_Buffer<SHADER_EFFECT_DESC>* m_pEffect_CBuffer = { nullptr };
	ID3DX11EffectConstantBuffer* m_pEffectBuffer = { nullptr };

	// ===========  STRUCTURED BUFFER   =========== 
				// EFFECT PARTICLE DATA
	// <INPUT>


	// <INPUT>
	//StructuredBuffer<EFFECT_PARTICLE_IMMU_ELEMENT>* m_pEffect_Immutable_Element_CBuffer = { nullptr };
	//ID3DX11EffectShaderResourceVariable* m_pEffect_Immutable_Element_SRV = { nullptr };
	//
	//// <OUTPUT>
	//StructuredBuffer<EFFECT_INSTANCE>* m_pEffect_Result_SBuffer = { nullptr };
	//ID3DX11EffectUnorderedAccessViewVariable* m_pEffect_Result_UAV = { nullptr };

	//  ===========   ===========    =========== 

	ID3DX11EffectScalarVariable* m_pGlobalMask_Effect = { nullptr };

	ID3DX11EffectShaderResourceVariable* m_pTransformTexture = { nullptr };
	ID3DX11EffectShaderResourceVariable* m_pRenderTargetTexture = { nullptr };
	ID3DX11EffectShaderResourceVariable* m_pRenderTargetDiffuseTexture = { nullptr };
	ID3DX11EffectShaderResourceVariable* m_pRenderTargetNormalTexture = { nullptr };
	ID3DX11EffectShaderResourceVariable* m_pRenderTargetShadeTexture = { nullptr };
	ID3DX11EffectShaderResourceVariable* m_pRenderTargetDepthTexture = { nullptr };
	// Effect Shader 전용 텍스처
	ID3DX11EffectShaderResourceVariable* m_prenderTargetSceneTexture = { nullptr };	// 유니티에서 SceneTexture라고 하더라.

	ID3DX11EffectShaderResourceVariable* m_pDefaultTextures = { nullptr };
	ID3DX11EffectShaderResourceVariable* m_pCubeTexture = { nullptr };
public:
	static CShader* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END