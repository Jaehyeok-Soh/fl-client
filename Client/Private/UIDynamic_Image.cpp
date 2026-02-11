#include "pch.h"
#include "UIDynamic_Image.h"
#include "Client_Defines.h"

//=================
// Component
//=================
#include "Texture.h"
#include "Shader.h"
#include "VIBuffer_Rect_Tex.h"
#include "StatComponent.h"
#include "GameInstance.h"

CUIDynamic_Image::CUIDynamic_Image(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	:CGenericUI(pDevice, pDeviceContext)
{
}

CUIDynamic_Image::CUIDynamic_Image(const CUIDynamic_Image& rhs)
	:CGenericUI(rhs)
{
}

HRESULT CUIDynamic_Image::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIDynamic_Image::Initialize(void* pArg)
{
	DIMAGE_DESC* pDesc = static_cast<DIMAGE_DESC*>(pArg);
	m_eDImageSubClass = pDesc->eSubClassType;

	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;
	if (FAILED(Ready_Components(pDesc)))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIDynamic_Image::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;

	return S_OK;
}

void CUIDynamic_Image::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CUIDynamic_Image::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
}

void CUIDynamic_Image::Update_Late(const _float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);
}

void CUIDynamic_Image::Ready_Before_Render(const _float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);
}

HRESULT CUIDynamic_Image::Render()
{
	if (!m_isVisible)
		return S_OK;

	if (FAILED(Super::Render()))
		return E_FAIL;

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIDynamic_Image::Ready_Components(DIMAGE_DESC* pDesc)
{
	return S_OK;
}

HRESULT CUIDynamic_Image::Bind_ShaderResources()
{
	return S_OK;
}

void CUIDynamic_Image::Free()
{
	Super::Free();
}
