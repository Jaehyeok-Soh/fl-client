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

	HRESULT Bind_SRV(EFXSRV eSlot, ID3D11ShaderResourceView* pSRV);
	HRESULT Bind_SRVArray(EFXSRV eSlot, ID3D11ShaderResourceView** ppSRV, _uint iCount);
	HRESULT Bind_Scalar(EFXScalar eSlot, _uint iValue);
	HRESULT Bind_Scalar(EFXScalar eSlot, _int iValue);
	HRESULT Bind_Scalar(EFXScalar eSlot, _float fValue);
	HRESULT Bind_Scalar(EFXScalar eSlot, _bool bValue);

	HRESULT Bind_MaterialData(const SHADER_MATERIALDESC& desc);
	HRESULT Bind_MaterialInstanceData(const SHADER_MI_DESC& desc);
	HRESULT Bind_EffectData(const SHADER_EFFECT_DESC& desc);
	HRESULT Bind_TransformData(const SHADER_TRANSFORMDESC& trnasformDesc);
	HRESULT Bind_TransformData(const Matrix& matTransform);
	HRESULT Bind_BoneData(const SHADER_BONEDESC& boneDesc);
	HRESULT Bind_KeyFrameData(const SHADER_KEYFRAMEDESC& keyframeDesc);
	HRESULT Bind_ObjectInfoData(const SHADER_OBJECTINFO_DESC& objectInfoDesc);

	// 외부에서 ConstantBuffer를 생성후 바인딩할 때
	HRESULT Set_ConstantBuffer(EFXCB eSlot, ID3D11Buffer* pBuffer);
private:
	// ShaderComponent내에 ConstantBuffer 생성
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
	CConstant_Buffer<SHADER_OBJECTINFO_DESC>* m_pObjectInfo_CBuffer{ nullptr };

public:
	static CShader* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END