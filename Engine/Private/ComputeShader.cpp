#include "Engine_pch.h"
#include "ComputeShader.h"

CComputeShader::CComputeShader(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: Super()
	, m_pDevice(pDevice)
	, m_pDeviceContext(pDeviceContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);
}

CComputeShader::CComputeShader(const CComputeShader& rhs)
{
}

HRESULT CComputeShader::Initialize_Prototype(void* pArg)
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;

	COMSHADER_ORIGIN_DESC* pDesc = static_cast<COMSHADER_ORIGIN_DESC*>(pArg);
	m_wstrPath = pDesc->pShaderFilePath;
	//if (FAILED(Load_Shader(pDesc->pElements, pDesc->iNumElements)))
	//	return E_FAIL;

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
	m_SRVs.push_back({ iSolt, pSRV });
}

void CComputeShader::Bind_UAV(_uint iSolt, ID3D11UnorderedAccessView* pUAV)
{
	m_UAVs.push_back({ iSolt, pUAV });
}

void CComputeShader::Bind_CB(_uint iSolt, ID3D11Buffer* pCB)
{
	m_CBs.push_back({ iSolt, pCB });
}

void CComputeShader::Dispatch(_uint iX, _uint iY, _uint iZ)
{
	m_pDeviceContext->CSSetShader(m_pComShader, nullptr, 0);

	for (auto& srv : m_SRVs)
		m_pDeviceContext->CSSetShaderResources(srv.first, 1, &srv.second);

	for (auto& uav : m_UAVs)
		m_pDeviceContext->CSSetUnorderedAccessViews(uav.first, 1, &uav.second, NULL);

	for (auto& cb : m_CBs)
		m_pDeviceContext->CSSetConstantBuffers(cb.first, 1, &cb.second);

	m_pDeviceContext->Dispatch(iX, iY, iZ);

	// °ª ÇØÁ¦
	Unbind_ComputeResources();
}

void CComputeShader::Unbind_ComputeResources()
{
	ID3D11UnorderedAccessView* nullUAV = nullptr;

	for (auto& u : m_UAVs)
		m_pDeviceContext->CSSetUnorderedAccessViews(u.first, 1, &nullUAV, nullptr);

	m_UAVs.clear();
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

	for (auto& pPair : m_SRVs)
		Safe_Release(pPair.second);
	m_SRVs.clear();

	for (auto& pPair : m_UAVs)
		Safe_Release(pPair.second);
	m_UAVs.clear();

	for (auto& pPair : m_CBs)
		Safe_Release(pPair.second);
	m_CBs.clear();

	Safe_Release(m_pDevice);
	Safe_Release(m_pDeviceContext);
}