#include "Engine_pch.h"
#include "Engine_Utils.h"
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
	const _bool bSRGB = IsSRGB_ByName(texturePath);
	if (::lstrcmp(texturePath.filename().extension().c_str(), L".dds") == 0)
	{
		const DirectX::DDS_LOADER_FLAGS ddsFlags =
			bSRGB ? DirectX::DDS_LOADER_FORCE_SRGB : DirectX::DDS_LOADER_IGNORE_SRGB;

		hr = CreateDDSTextureFromFileEx(m_pDevice, Get_Path(),
			0, D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, 0, 0,
			ddsFlags, nullptr, &m_pSRV);

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
		const DirectX::WIC_LOADER_FLAGS wicFlags =
			bSRGB ? DirectX::WIC_LOADER_FORCE_SRGB : DirectX::WIC_LOADER_IGNORE_SRGB;

		hr = CreateWICTextureFromFileEx(m_pDevice, Get_Path(),
			0, D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, 0, 0,
			wicFlags, nullptr, &m_pSRV);

		if (SUCCEEDED(GetMetadataFromWICFile(Get_Path(), WIC_FLAGS_NONE, metadata)))
			m_vSize = { (_float)metadata.width, (_float)metadata.height };
	}

	if (FAILED(hr))
		return E_FAIL;

	D3D11_SHADER_RESOURCE_VIEW_DESC desc{};
	m_pSRV->GetDesc(&desc);
	
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

_bool CTextureBase::IsSRGB_ByName(const std::filesystem::path texturePath)
{
	std::wstring stem = Engine_Utils::To_Upper(texturePath.stem().wstring());

	// 긴 토큰 우선 ( _D 가 _DH 에 포함되는 문제 방지 )
	if (Engine_Utils::Has_Token(stem, L"_DH"))   return true;
	if (Engine_Utils::Has_Token(stem, L"_BC"))   return true;
	if (Engine_Utils::Has_Token(stem, L"_TINT")) return true;
	if (Engine_Utils::Has_Token(stem, L"_D"))    return true;

	return false;
	return _bool();
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
