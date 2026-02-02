#include "pch.h"
#include "Image.h"
#include "GameInstance.h"

CImage::CImage()
{
}

CImage::CImage(const CImage& rhs)
{
}

HRESULT CImage::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CImage::Initialize_Prototype(const IMAGE_DESC& Desc)
{
	return S_OK;
}

HRESULT CImage::Initialize(void* pArg)
{
	IMAGE_DESC* pDesc = static_cast<IMAGE_DESC*>(pArg);

	return S_OK;
}

void CImage::Update(const _float fTimeDelta)
{
}

CImage* CImage::Create(const IMAGE_DESC& Desc)
{
	CImage* pInstance = new CImage();
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CImage::Create, Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CImage::Clone(void* pArg)
{
	CImage* pInstance = new CImage(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CImage::Clone, Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CImage::Free()
{
	Super::Free();
}


