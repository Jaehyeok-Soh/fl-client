#include "GameInstance.h"
#include "Shader.h"
#include "Texture.h"

CTexture::CTexture()
	: Super()
{
}

CTexture::CTexture(const CTexture& rhs)
	: Super(rhs)
	, m_iTextureCount(rhs.m_iTextureCount)
	, m_vecTextures(rhs.m_vecTextures)
{
	for (CTextureBase*& pElement : m_vecTextures)
		Safe_AddRef(pElement);

	for (size_t i = 0; i < TEXTURE_MAGICNUMBER; ++i)
	{
		m_arrSRV[i] = rhs.m_arrSRV[i];
		Safe_AddRef(m_arrSRV[i]);
	}
}

HRESULT CTexture::Initialize_Prototype(void* pArg)
{
	if (!pArg)
		return E_FAIL;

	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;

	TEXTURE_COMPONENT_ORIGIN_DESC* pDesc = static_cast<TEXTURE_COMPONENT_ORIGIN_DESC*>(pArg);
	if (pDesc->wstrTexturePath.empty() || pDesc->iTextureCount <= 0)
	{
		if (pDesc->wstrTexturePath.empty() && pDesc->iTextureCount <= 0)
		{
			m_vecTextures.resize(TEXTURE_MAGICNUMBER);
			return S_OK;
		}
		else
			return E_FAIL;
	}

	m_iTextureCount = pDesc->iTextureCount;
	m_vecTextures.reserve(pDesc->iTextureCount);
	for (_uint i = 0; i < m_iTextureCount; ++i)
	{
		_tchar      wszFilePath[MAX_PATH] = {};

		HRESULT hr = {};
		wstring tag = L"Texture_";
		wsprintf(wszFilePath, pDesc->wstrTexturePath.c_str(), i);
		tag += std::filesystem::path(wszFilePath).stem();


		CTextureBase::RESOURCE_BASE_DESC desc = {};
		desc.wstrName = tag;
		desc.wstrPath = wszFilePath;
		CTextureBase* pTexture = m_pGameInstance->GetOrAddTexture(desc.wstrName, &desc);
		if (!pTexture)
			return E_FAIL;

		m_vecTextures.push_back(pTexture);
	}
	return S_OK;
}

HRESULT CTexture::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CTexture::Add_DefaultTexture(const wstring& wstrTextureTag, _uint iTextureIndex)
{
	if (iTextureIndex >= TEXTURE_MAGICNUMBER)
		return E_FAIL;
	
	if (m_vecTextures.size() < TEXTURE_MAGICNUMBER)
		m_vecTextures.resize(TEXTURE_MAGICNUMBER);
	
	CTextureBase* pTexture = { nullptr };
	if (!(pTexture = m_pGameInstance->Get_Resource<CTextureBase>(wstrTextureTag)))
	{
		if (m_vecTextures[iTextureIndex])
		{
			Safe_Release(m_vecTextures[iTextureIndex]);
			m_vecTextures[iTextureIndex] = nullptr;
			Safe_Release(m_arrSRV[iTextureIndex]);
			m_arrSRV[iTextureIndex] = nullptr;
			--m_iTextureCount;
		}
		return S_OK;
	}
	
	if (m_vecTextures[iTextureIndex] != nullptr)
	{
		Safe_Release(m_vecTextures[iTextureIndex]);
		m_vecTextures[iTextureIndex] = nullptr;
		Safe_Release(m_arrSRV[iTextureIndex]);
		m_arrSRV[iTextureIndex] = nullptr;
		--m_iTextureCount;
	}
	
	m_vecTextures[iTextureIndex] = pTexture;
	m_arrSRV[iTextureIndex] = pTexture->Get_SRV();
	Safe_AddRef(m_arrSRV[iTextureIndex]);
	++m_iTextureCount;
	return S_OK;
}

HRESULT CTexture::Release_DefaultTexture(_uint iTextureIndex)
{
	if (iTextureIndex >= TEXTURE_MAGICNUMBER)
		return E_FAIL;

	if (m_vecTextures.size() < TEXTURE_MAGICNUMBER)
		m_vecTextures.resize(TEXTURE_MAGICNUMBER);

	if (m_vecTextures[iTextureIndex] != nullptr)
	{
		Safe_Release(m_vecTextures[iTextureIndex]);
		m_vecTextures[iTextureIndex] = nullptr;
		Safe_Release(m_arrSRV[iTextureIndex]);
		m_arrSRV[iTextureIndex] = nullptr;
		--m_iTextureCount;
	}
	return S_OK;
}

_bool CTexture::Is_Valid(_uint iTextureIndex)
{
	return m_vecTextures[iTextureIndex] != nullptr;
}

HRESULT CTexture::Bind_ShaderResource(CShader* pShader, _uint iIndex)
{
	if (m_vecTextures.size() <= iIndex)
		return E_FAIL;

	return pShader->Bind_DefaultTexture(m_vecTextures[iIndex]->Get_SRV());
}

HRESULT CTexture::Bind_ShaderResourceBuffer(CShader* pShader)
{
	return pShader->Bind_DefaultTextures(&m_arrSRV[0], TEXTURE_MAGICNUMBER);
}

HRESULT CTexture::Bind_ShaderResource_Cube(CShader* pShader)
{
	return pShader->Bind_CubeTexture(m_vecTextures[0]->Get_SRV());
}

CTexture* CTexture::Create(void *pArg)
{
	CTexture* pInstance = new CTexture();
	if (FAILED(pInstance->Initialize_Prototype(pArg)))
	{
		MSG_BOX("CTexture::Create, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CTexture::Clone(void* pArg)
{
	CTexture* pInstance = new CTexture(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CTexture::Clone, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CTexture::Free()
{
	for (CTextureBase*& pElement : m_vecTextures)
		Safe_Release(pElement);
	for (auto& pSRV : m_arrSRV)
		Safe_Release(pSRV);

	Super::Free();
}
