#include "Engine_pch.h"
#include "ComputeShader.h"

// has class
#include "StructuredBuffer.h"

CComputeShader::CComputeShader(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: Super()
	, m_pDevice(pDevice)
	, m_pDeviceContext(pDeviceContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);
}

CComputeShader::CComputeShader(const CComputeShader& rhs)
	: Super(rhs)
	, m_pComputeShader(rhs.m_pComputeShader)
	, m_pDevice(rhs.m_pDevice)
	, m_pDeviceContext(rhs.m_pDeviceContext)
{
	Safe_AddRef(m_pComputeShader);
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);
}

HRESULT CComputeShader::Initialize_Prototype(void* pArg)
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;

	COMSHADER_ORIGIN_DESC* pDesc = static_cast<COMSHADER_ORIGIN_DESC*>(pArg);
	m_wstrPath = pDesc->pShaderFilePath;

	if (FAILED(Ready_ComputeShader(pDesc)))
		return E_FAIL;
		
	//Create_ConstantBuffer();
	return S_OK;
}

HRESULT CComputeShader::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	//Create_StructuredBuffer();
	return S_OK;
}

void CComputeShader::Bind_SRV(_uint iSolt, ID3D11ShaderResourceView* pSRV)
{
	//m_SRVs.push_back({ iSolt, pSRV });
}

void CComputeShader::Bind_UAV(_uint iSolt, ID3D11UnorderedAccessView* pUAV)
{
	//m_UAVs.push_back({ iSolt, pUAV });
}

void CComputeShader::Bind_CB(_uint iSolt, ID3D11Buffer* pCB)
{
	//m_CBs.push_back({ iSolt, pCB });
}

void CComputeShader::Dispatch(_uint iX, _uint iY, _uint iZ)
{
	m_pDeviceContext->CSSetShader(m_pComputeShader, nullptr, 0);

	//for (auto& srv : m_SRVs)
	//	m_pDeviceContext->CSSetShaderResources(srv.first, 1, &srv.second);

	//for (auto& uav : m_UAVs)
	//	m_pDeviceContext->CSSetUnorderedAccessViews(uav.first, 1, &uav.second, NULL);

	//for (auto& cb : m_CBs)
	//	m_pDeviceContext->CSSetConstantBuffers(cb.first, 1, &cb.second);

	m_pDeviceContext->Dispatch(iX, iY, iZ);

	// 값 해제
	Unbind_ComputeResources();
}

HRESULT CComputeShader::Ready_ComputeShader(COMSHADER_ORIGIN_DESC* pDesc)
{
	HRESULT hr = S_OK;

	ID3DBlob* pCSBlob		= { nullptr };
	ID3DBlob* pErrorBlob	= { nullptr };

	_int flag = {};
#ifdef _DEBUG
	flag = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	flag = D3DCOMPILE_OPTIMIZATION_LEVEL1;
#endif  

	// 파일을 읽는다
	hr = D3DCompileFromFile
		(
		pDesc->pShaderFilePath,
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		pDesc->pEntryPoint,          // 엔트리 포인트 : compute shader는 하나의 연산만 하기때문에 고정
		"cs_5_0",           // Compute Shader
		flag,
		0,
		&pCSBlob,
		&pErrorBlob
	);

	// 파일 읽기 실패시
	if (FAILED(hr))
	{
		if (pErrorBlob)
		{
			OutputDebugStringA((char*)pErrorBlob->GetBufferPointer());
			Safe_Release(pErrorBlob);
		}
		return E_FAIL;
	}

	// 드디어 computeShader 생성
	hr = m_pDevice->CreateComputeShader(
		pCSBlob->GetBufferPointer(),
		pCSBlob->GetBufferSize(),
		nullptr,
		&m_pComputeShader
	);

	Safe_Release(pCSBlob);
	Safe_Release(pErrorBlob);

	return hr;
}

void CComputeShader::Unbind_ComputeResources()
{
	// 모든걸 unscribe 해준다
	ID3D11ShaderResourceView*	nullSRV[1]	= { nullptr };
	ID3D11UnorderedAccessView*	nullUAV[1]	= { nullptr };
	ID3D11Buffer*				nullCB[1]	= { nullptr };

	//for (auto& srv : m_SRVs)
	//	m_pDeviceContext->CSSetShaderResources(srv.first, 1, nullSRV);

	//for (auto& uav : m_UAVs)
	//	m_pDeviceContext->CSSetUnorderedAccessViews(uav.first, 1, nullUAV, nullptr);

	//for (auto& cb : m_CBs)
	//	m_pDeviceContext->CSSetConstantBuffers(cb.first, 1, nullCB);

	//m_SRVs.clear();
	//m_UAVs.clear();
	//m_CBs.clear();
}

CComputeShader* CComputeShader::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg)
{
	CComputeShader* pInstance = new CComputeShader(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype(pArg)))
	{
		MSG_BOX("CComputeShader::Create, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CComputeShader::Clone(void* pArg)
{
	CComputeShader* pInstance = new CComputeShader(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CComputeShader::Clone, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CComputeShader::Free()
{
	__super::Free();

	Safe_Release(m_pComputeShader);
	Safe_Release(m_pDevice);
	Safe_Release(m_pDeviceContext);
}