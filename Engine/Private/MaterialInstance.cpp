#include "MaterialInstance.h"
#include "Shader.h"
#include "Material.h"
#include "GameInstance.h"

CMaterialInstance::CMaterialInstance(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: Super(EResourceType::MATERIAL_INSTANCE, pDevice, pDeviceContext)
{
}

HRESULT CMaterialInstance::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	MI_ORIGIN_DESC* pDesc = static_cast<MI_ORIGIN_DESC*>(pArg);
	m_eMIType = pDesc->eType;
	m_shaderDesc.vTintColor = pDesc->vTintColor;
	m_shaderDesc.fEmissivePower = pDesc->fEmissivePower;

	return S_OK;
}

HRESULT CMaterialInstance::Bind_ShaderResource(CShader* pShader)
{
	//if (FAILED(m_pBaseMaterial->Bind_ShaderResource(pShader)))
	//	return E_FAIL;

	pShader->Bind_MaterialInstanceData(m_shaderDesc);
	return S_OK;
}

CMaterialInstance* CMaterialInstance::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg)
{
	CMaterialInstance* pInstance = new CMaterialInstance(pDevice, pDeviceContext);
	if(FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CMaterialInstance::Create, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMaterialInstance::Free()
{
	Safe_Release(m_pBaseMaterial);
	Super::Free();
}
