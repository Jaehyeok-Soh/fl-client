#pragma once
#include "Component.h"

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

template<typename T>
class StructuredBuffer;

class ENGINE_DLL CComputeShader : public CComponent
{
	using Super = CComponent;
public:
	typedef struct tagComShaderOriginDesc
	{
		const _tchar* pShaderFilePath	= { nullptr };
		const _char* pEntryPoint		= { nullptr };		// shader 내부에서 어떤 함수를 쓸건데
	}COMSHADER_ORIGIN_DESC;

private:
	explicit CComputeShader(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CComputeShader(const CComputeShader& rhs);
	virtual ~CComputeShader() = default;

public:
	virtual HRESULT Initialize_Prototype(void* pArg);
	virtual HRESULT Initialize(void* pArg);

public:
	void	Bind_SRV(_uint iSolt, ID3D11ShaderResourceView* pSRV);
	void	Bind_UAV(_uint iSolt, ID3D11UnorderedAccessView* pUAV);
	void	Bind_CB(_uint iSolt, ID3D11Buffer* pCB);

	void	Dispatch(_uint iX, _uint iY, _uint iZ);

private:
	ID3D11ComputeShader*	m_pComputeShader		= { nullptr };
	ID3D11Device*			m_pDevice				= { nullptr };
	ID3D11DeviceContext*	m_pDeviceContext		= { nullptr };

private:
	_uint m_iSlot = { 0 }; // 이게 필요 할깝쇼

	CBase* m_pInputStructedBuffer	= nullptr;
	CBase* m_pConstBuffer			= nullptr;
	CBase* m_pOutputStructedBuffer	= nullptr;

private:
	wstring m_wstrPath = { L"" };

private:
	HRESULT Ready_ComputeShader(COMSHADER_ORIGIN_DESC* pDesc);
	void Unbind_ComputeResources();

public:
	static CComputeShader*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg);
	virtual CComponent*		Clone(void* pArg) override;
	virtual void			Free() override;
};
NS_END