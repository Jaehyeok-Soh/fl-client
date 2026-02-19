#include "Engine_pch.h"
#include "TextureBase.h"

CTextureBase::CTextureBase(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: Super(EResourceType::TEXTURE, pDevice, pDeviceContext)
{
}

HRESULT CTextureBase::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Load_TextureBase()))
		return E_FAIL;

	return S_OK;
}

HRESULT CTextureBase::Load_TextureBase()
{
	std::filesystem::path texturePath{ Get_Path() };

	ID3D11Resource* pResource = { nullptr };
	DirectX::TexMetadata metadata = {};

	HRESULT hr = {};
	if (::lstrcmp(texturePath.filename().extension().c_str(), L".dds") == 0)
	{
		hr = CreateDDSTextureFromFile(m_pDevice, Get_Path(), nullptr, &m_pSRV);
		if (SUCCEEDED(GetMetadataFromDDSFile(Get_Path(), DDS_FLAGS_NONE, metadata)))
			m_vSize = { (_float)metadata.width, (_float)metadata.height };

		if (m_pSRV)
		{
			D3D11_SHADER_RESOURCE_VIEW_DESC desc;
			m_pSRV->GetDesc(&desc);

			// 이게 D3D11_SRV_DIMENSION_TEXTURE2DARRAY (값 5) 라면 성공!
			if (desc.ViewDimension == D3D11_SRV_DIMENSION_TEXTURE2DARRAY)
			{
				// desc.Texture2DArray.ArraySize 가 7인지 확인해보세요.
			}
		}
	}
	else if (::lstrcmp(texturePath.filename().extension().c_str(), L".tga") == 0)
		return E_FAIL;
	else
	{
		hr = CreateWICTextureFromFile(m_pDevice, Get_Path(), nullptr, &m_pSRV);
		if (SUCCEEDED(GetMetadataFromWICFile(Get_Path(), WIC_FLAGS_NONE, metadata)))
			m_vSize = { (_float)metadata.width, (_float)metadata.height };
	}

	if (FAILED(hr))
		return E_FAIL;
	
	return S_OK;
}

HRESULT CTextureBase::Load_LegacyXTex()
{
	DirectX::TexMetadata md;
	DirectX::ScratchImage img;

	if (FAILED(::LoadFromWICFile(Get_Path(), WIC_FLAGS_NONE, &md, img)))
	{
		MSG_BOX("LoadFromWICFile, Failed");
		return E_FAIL;
	}

	if (FAILED(::CreateShaderResourceView(m_pDevice, img.GetImages(), img.GetImageCount(), md, &m_pSRV)))
	{
		MSG_BOX("CreateShaderResourceView, Failed");
		return E_FAIL;
	}

	m_vSize.x = (_float)md.width;
	m_vSize.y = (_float)md.height;

	return S_OK;
}

CTextureBase* CTextureBase::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg)
{
	CTextureBase* pInstance = new CTextureBase(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CTextureBase::Create, Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTextureBase::Free()
{
	Safe_Release(m_pSRV);
	Super::Free();
}
