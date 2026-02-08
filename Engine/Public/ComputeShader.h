#pragma once
#include "MonoBehaviour.h"
#include "Shader.h"
/*
* 하나의 Compute Shdaer의 하나의 함수를 대변한다
* 하나의 Compute Shader는 하나의 연산을 책임진다 (하나의 함수만 실행한다 : dispatch 를 통해)
*
* 외부에서 resource들을 바인드 해준다
* 이 component는 저장된 자료들을 통해 계산을 하고
* 계산 결과를 정리하고 저장한다.
*
* 즉 compute shader 연산을 하기 위한 통로.
* 정쌤 식 shader component로 생각하면 편할듯
*
* BUT! 바뀔 수 있다
*/

NS_BEGIN(Engine)

class StructuredBuffer;
class CFxEffectAsset;

class ENGINE_DLL CComputeShader : public CMonoBehaviour
{
	using Super = CMonoBehaviour;
public:
	struct Data
	{
		string	sBufferName;
		_uint	iElementSize;
		_uint	iNumElements;
	};

	typedef struct ComShaderOriginDesc : public CComponent::COMPONENT_DESC
	{
		const _tchar* pShaderFilePath = { nullptr };
		// EVtxLayout eLayout = EVtxLayout::NONE;  // InputLayout 없으니까
		const _char* pEntryPoint = { nullptr };		// shader 내부에서 어떤 함수를 쓸건데
	}COMSHADER_ORIGIN_DESC;

	typedef struct ComShaderCopyDesc : public CComponent::COMPONENT_DESC
	{
		Data	Input_StructBuffer;
		_uint	InputBufferNum;

		Data	OutPut_StructBuffer;
		string	Output_SRVBuffer_Name;

	}COMSHADER_COPY_DESC;

private:
	explicit CComputeShader(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CComputeShader(const CComputeShader& rhs);
	virtual ~CComputeShader() = default;

public:
	virtual HRESULT Initialize_Prototype(void* pArg);
	virtual HRESULT Initialize_Prototype() override { return S_OK; } // Mono Behavior
	virtual HRESULT Initialize(void* pArg) override;  // Mono Behavior
	virtual void	Update(const _float fTimeDelta) override {};

public:
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

public:
	void	Bind_InputStructuredBuffer(_uint Index, ID3DX11EffectShaderResourceVariable* pSRV, StructuredBuffer* pSB);
	void	Dispatch(_uint iX, _uint iY, _uint iZ);

public:
	StructuredBuffer* Get_Input_Buffer(_uint Index);
	StructuredBuffer* Get_Output_Buffer();

public:
	// Struct Buffer 전용
	void Bind_InputStructuredBuffer_Data(_uint Index, void* pArg, _uint iElementSize, _uint iNumElements);

public:
	// Constant Buffer 전용
	void Bind_Compute_EffectData(const EFFECT_PARTICLE_MU_ELEMENT& desc);

public:
	void	Resize_InputStruct(_uint Index, void* pArg, _uint iElementSize, _uint iNumElements);
	void	Resize_OutputStruct(_uint Index, void* pArg, _uint iElementSize, _uint iNumElements);

private:
	//StructuredBuffer* m_pInputStructedBuffer = nullptr;
	//ID3DX11EffectShaderResourceVariable* m_pInputStructedBuffer_SRV = { nullptr };

	//std::map<string, std::pair<ID3DX11EffectShaderResourceVariable*, StructuredBuffer*>>	m_pInputStructuredBuffer = {};
	std::vector<std::pair<ID3DX11EffectShaderResourceVariable*, StructuredBuffer*>>	m_pInputStructuredBuffer = {};

	StructuredBuffer* m_pOutputStructedBuffer = nullptr;
	ID3DX11EffectUnorderedAccessViewVariable* m_pOutputStructedBuffer_UAV = { nullptr };

	//  ===================   ConstantBuffer   ====================
	CConstant_Buffer<EFFECT_PARTICLE_MU_ELEMENT>* m_pEffect_Mutable_Element_CBuffer = { nullptr };
	ID3DX11EffectConstantBuffer* m_pEffect_MutableBuffer = { nullptr };

	// SHader
private:
	HRESULT Ready_ComputeShader(COMSHADER_ORIGIN_DESC* pDesc);
private:
	HRESULT Create_ConstantBuffer();
	HRESULT Create_StructBuffer(void* pArg);

	void Clear_ConstantBuffer();
	void Clear_StructBuffer();

private:
	CFxEffectAsset* m_pOwner{ nullptr }; // .hlsl 파일 객체화
	_uint m_iPass = { 0 };
private:
	ID3D11ComputeShader* m_pComputeShader = { nullptr };
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pDeviceContext = { nullptr };

public:
	static CComputeShader* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg);
	virtual CComponent* Clone(void* pArg) override;
	virtual void		Free() override;
};
NS_END