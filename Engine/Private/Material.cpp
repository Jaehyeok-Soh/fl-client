#include "Engine_pch.h"
#include "Material.h"
#include "Shader.h"
#include "TextureBase.h"
#include "GameInstance.h"

CMaterial::CMaterial(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: Super(EResourceType::MATERIAL, pDevice, pDeviceContext)
{
	m_arrSRVs.fill(nullptr);
}

CMaterial::CMaterial(const CMaterial& rhs)
	: Super(rhs)
	, m_iTextureMask(rhs.m_iTextureMask)
{
	for (size_t i = 0; i < ENUM_TO_SZET(EMaterialTextureType::MAX_COUNT); ++i)
	{
		m_arrSRVs[i] = rhs.m_arrSRVs[i];
		if (m_arrSRVs[i])
			Safe_AddRef(m_arrSRVs[i]);
	}
}

HRESULT CMaterial::Initialize(void* pArg)
{
	if (!pArg)
		return E_FAIL;

	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	MATERIAL_DESC* pDesc = static_cast<MATERIAL_DESC*>(pArg);
	for (size_t i = 0; i < pDesc->spanTags.size(); ++i)
	{
		if(pDesc->spanTags[i].empty() == false)
			if (FAILED(Caching_Resource(Engine_Utils::ToWString(pDesc->spanTags[i]), static_cast<EMaterialTextureType>(i))))
				return E_FAIL;
	}

	return S_OK;
}

HRESULT CMaterial::Bind_ShaderResource(CShader* pShader)
{
	pShader->Bind_MaterialTextures(&m_arrSRVs[0], ENUM_TO_UINT(EMaterialTextureType::MAX_COUNT));
	pShader->Bind_MaterialMask(m_iTextureMask);
	return S_OK;
}

HRESULT CMaterial::Caching_Resource(const wstring& pTag, EMaterialTextureType eSlot)
{
	ID3D11ShaderResourceView* pSRV = Get_ShaderResourceView(pTag.c_str());
	if (!pSRV)
		return E_FAIL;

	m_arrSRVs[ENUM_TO_UINT(eSlot)] = pSRV;
	m_iTextureMask |= 1 << ENUM_TO_UINT(eSlot);
	return S_OK;
}

ID3D11ShaderResourceView* CMaterial::Get_ShaderResourceView(const _tchar* pTag)
{
	ID3D11ShaderResourceView* pSRV = { nullptr };
	CTextureBase* pTmp = m_pGameInstance->Get_Resource<CTextureBase>(pTag);
	if (!pTmp) return nullptr;
	if (!(pSRV = pTmp->Get_SRV()))
	{
		Safe_Release(pTmp);
		return nullptr;
	}

	Safe_Release(pTmp);
	Safe_AddRef(pSRV);
	return pSRV;
}

CMaterial* CMaterial::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg)
{
	CMaterial* pInstance = new CMaterial(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CMaterial::Create, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMaterial::Free()
{
	for (auto* pSRV : m_arrSRVs)
	{
		Safe_Release(pSRV);
	}
	m_arrSRVs.fill(nullptr);
	
	Super::Free();
}
