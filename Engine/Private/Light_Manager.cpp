#include "Light_Manager.h"
#include "Light.h"
#include "Shader.h"
#include "VIBuffer_Rect_Tex.h"
#include "Constant_Buffer.h"

CLight_Manager::CLight_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: m_pDevice(pDevice)
	, m_pDeviceContext(pDeviceContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);
}

HRESULT CLight_Manager::Initialize()
{
	m_pLight_CBuffer = CConstant_Buffer<SHADER_LIGHTDESC>::Create(m_pDevice, m_pDeviceContext);
	return S_OK;
}

HRESULT CLight_Manager::Add_Light(const LIGHT_DESC& LightDesc)
{
	LIGHT_TYPE eLightType = LightDesc.eType;
	if (eLightType >= LIGHT_TYPE::END || ENUM_TO_UINT(eLightType) < 0)
		return E_FAIL;

	CLight* pLight = { nullptr };
	if (!(pLight = CLight::Create(LightDesc)))
		return E_FAIL;

	m_Lights[ENUM_TO_UINT(eLightType)].push_back(pLight);
	return S_OK;
}

HRESULT CLight_Manager::Push_DynamicLight(CLight* pLight)
{
	if (pLight == nullptr)
		return E_FAIL;

	if (pLight->Get_Type() != LIGHT_TYPE::DYNAMICPOINT)
		return E_FAIL;

	m_Lights[ENUM_TO_UINT(LIGHT_TYPE::DYNAMICPOINT)].push_back(pLight);
	return E_NOTIMPL;
}

HRESULT CLight_Manager::Render(CShader* pShader, CVIBuffer_Rect_Tex* pVIBuffer)
{
	for (list<CLight*> &list : m_Lights)
	{
		for (CLight* pLight : list)
		{
			Setup_Pass(pShader, pLight->Get_Type());
			Setup_ConstantBuffer(pLight->Get_LightDesc());
			pShader->Apply();
			pVIBuffer->Bind_Resource();
			pVIBuffer->Render();
		}
	}
	m_Lights[ENUM_TO_UINT(LIGHT_TYPE::DYNAMICPOINT)].clear();
	return S_OK;
}

ID3D11Buffer* CLight_Manager::Get_Light_ConstantBuffer() const
{
	return m_pLight_CBuffer->Get_Buffer();
}

void CLight_Manager::Clear()
{
	for (auto& list : m_Lights)
	{
		for (CLight* pElement : list)
		{
			Safe_Release(pElement);
		}
		list.clear();
	}
}

void CLight_Manager::Setup_Pass(CShader* pShader, LIGHT_TYPE eType)
{
	switch (eType)
	{
	case Engine::LIGHT_TYPE::DIRECTIONAL:
		pShader->Set_Pass(ENUM_TO_UINT(DEFFERRED::DIRECTIONAL));
		break;
	case Engine::LIGHT_TYPE::STATICPOINT:
	case Engine::LIGHT_TYPE::DYNAMICPOINT:
		pShader->Set_Pass(ENUM_TO_UINT(DEFFERRED::POINT));
		break;
	}
}

void CLight_Manager::Setup_ConstantBuffer(const SHADER_LIGHTDESC& lightDesc)
{
	m_pLight_CBuffer->Copy_Data(lightDesc);
}

CLight_Manager *CLight_Manager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CLight_Manager* pInstance = new CLight_Manager(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("CLight_Manager::Create, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLight_Manager::Free()
{
	Clear();
	Safe_Release(m_pLight_CBuffer);
	Safe_Release(m_pDeviceContext);
	Safe_Release(m_pDevice);
	Super::Free();
}
