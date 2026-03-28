#include "pch.h"
#include "UISceneFade_Image.h"
#include "Client_Defines.h"

#include "Canvas.h"
#include "Texture.h"
#include "Shader.h"
#include "VIBuffer_Rect_Tex.h"
#include "UI_Manager.h"
#include "GameInstance.h"

CUISceneFade_Image::CUISceneFade_Image(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	:CUIDynamic_Image(pDevice, pDeviceContext)
{
}

CUISceneFade_Image::CUISceneFade_Image(const CUISceneFade_Image& rhs)
	:CUIDynamic_Image(rhs)
{
}

HRESULT CUISceneFade_Image::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUISceneFade_Image::Initialize(void* pArg)
{
	SCENEFADE_IMAGE_DESC* pDesc = static_cast<SCENEFADE_IMAGE_DESC*>(pArg);
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;
	if (FAILED(Ready_Components(pDesc)))
		return E_FAIL;
	return S_OK;
}


HRESULT CUISceneFade_Image::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;
	if (FAILED(Attach_Personal_Info()))
		return E_FAIL;
	return S_OK;
}

void CUISceneFade_Image::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CUISceneFade_Image::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
	Tick_By_Type(fTimeDelta);
}

void CUISceneFade_Image::Update_Late(const _float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);
}

void CUISceneFade_Image::Ready_Before_Render(const _float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);
}

HRESULT CUISceneFade_Image::Render()
{
	if (!m_isVisible)
		return S_OK;
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;
	if (FAILED(Super::Render()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUISceneFade_Image::Ready_Components(SCENEFADE_IMAGE_DESC* pDesc)
{
	if (FAILED(Super::Ready_Components(pDesc)))
		return E_FAIL;
	return S_OK;
}

HRESULT CUISceneFade_Image::Bind_ShaderResources()
{
	CShader* pShader = Get_Component<CShader>();
	if (FAILED(Get_Component<CTransform>()->Bind_ShaderResource(pShader)))
		return E_FAIL;

	if (FAILED(Super::Bind_ShaderResources()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUISceneFade_Image::Attach_Personal_Info()
{
	return S_OK;
}

void CUISceneFade_Image::Bind_Events()
{
	Super::Bind_Events();
}

void CUISceneFade_Image::Tick_By_Type(const _float fTimeDelta)
{
}

void CUISceneFade_Image::Initialize_Visible_Event()
{
}

_bool CUISceneFade_Image::Tick_Visible_Event(const _float fTimeDelta)
{
	return true;
}

void CUISceneFade_Image::Initialize_InVisible_Event()
{
}

_bool CUISceneFade_Image::Tick_InVisible_Event(const _float fTimeDelta)
{
	return true;
}

CUISceneFade_Image* CUISceneFade_Image::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CUISceneFade_Image* pInstance = new CUISceneFade_Image(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CUISceneFade_Image::Create, Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUISceneFade_Image::Clone(void* pArg)
{
	CUISceneFade_Image* pInstance = new CUISceneFade_Image(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CUISceneFade_Image::Clone, Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUISceneFade_Image::Free()
{
	Super::Free();
}
