#include "Engine_pch.h"
#include "GravityForce.h"
#include "GameInstance.h"

#include "Engine_pch.h"
#include "ComputeShader.h"
#include "Engine_Utils.h"

// has class
#include "StructuredBuffer.h"
#include "Constant_Buffer.h"

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
	, m_bInit(rhs.m_bInit)
	, m_wstrPath(rhs.m_wstrPath)
	, m_pBlob(rhs.m_pBlob)
	, m_pEffect(rhs.m_pEffect)
	, m_tEffectDesc(rhs.m_tEffectDesc)
	, m_vecTechniques(rhs.m_vecTechniques)
	, m_pComputeShader(rhs.m_pComputeShader)
	, m_pDevice(rhs.m_pDevice)
	, m_pDeviceContext(rhs.m_pDeviceContext)
	, m_pEffect_Mutable_Element_CBuffer(rhs.m_pEffect_Mutable_Element_CBuffer)
	, m_pEffect_MutableBuffer(rhs.m_pEffect_MutableBuffer)

{
	Safe_AddRef(m_pComputeShader);
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);
	Safe_AddRef(m_pBlob);
	Safe_AddRef(m_pEffect);
	Safe_AddRef(m_pEffect_Mutable_Element_CBuffer);
	Safe_AddRef(m_pEffect_MutableBuffer);
}

HRESULT CComputeShader::Initialize_Prototype(void* pArg)
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;

	COMSHADER_ORIGIN_DESC* pDesc = static_cast<COMSHADER_ORIGIN_DESC*>(pArg);
	m_wstrPath = pDesc->pShaderFilePath;

	if (FAILED(Ready_ComputeShader(pDesc)))
		return E_FAIL;

	Create_ConstantBuffer();
	return S_OK;
}

HRESULT CComputeShader::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	Create_StructBuffer(pArg);
	return S_OK;
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
	Safe_Release(m_pDevice);
	Safe_Release(m_pDeviceContext);
	Super::Free();
}