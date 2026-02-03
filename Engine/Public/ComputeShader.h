#pragma once
#include "Component.h"

NS_BEGIN(Engine)

class ENGINE_DLL CComputeShader final : public CComponent
{
	using Super = CComponent;
public:
	typedef struct tagComShaderOriginDesc
	{
		const _tchar* pShaderFilePath = { nullptr };
		_uint iNumElements = { 0 };
		const D3D11_INPUT_ELEMENT_DESC* pElements = { nullptr };
	}COMSHADER_ORIGIN_DESC;

private:
	CComputeShader(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
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
	ID3D11ComputeShader*	m_pComShader		= { nullptr };
	ID3D11Device*			m_pDevice			= { nullptr };
	ID3D11DeviceContext*	m_pDeviceContext	= { nullptr };

private:
	vector<std::pair<_uint, ID3D11ShaderResourceView*>>		m_SRVs;
	vector<std::pair<_uint, ID3D11UnorderedAccessView*>>	m_UAVs;
	vector<std::pair<_uint, ID3D11Buffer*>>					m_CBs;

private:
	wstring m_wstrPath = { L"" };

private:
	void Unbind_ComputeResources();

public:
	static CComputeShader*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg);
	virtual CComponent*		Clone(void* pArg) override;
	virtual void			Free() override;
};
NS_END