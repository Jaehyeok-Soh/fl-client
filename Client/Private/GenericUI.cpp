#include "pch.h"
#include "GenericUI.h"
#include "Client_Defines.h"

//=================
// Component
//=================
#include "Texture.h"
#include "Shader.h"
#include "VIBuffer_Rect_Tex.h"

#include "GameInstance.h"

CGenericUI::CGenericUI(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	:CUIObject(pDevice, pDeviceContext)
{
}

CGenericUI::CGenericUI(const CGenericUI& rhs)
	:CUIObject(rhs)
{
}

HRESULT CGenericUI::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;
	return S_OK;
}

HRESULT CGenericUI::Initialize(void* pArg)
{
	GENERIC_UI_DESC* pDesc = static_cast<GENERIC_UI_DESC*>(pArg);

	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;


	if (FAILED(Ready_Components(pDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CGenericUI::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;

	Set_SizeToTextureScale();
	return S_OK;
}

void CGenericUI::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CGenericUI::Update(const _float fTimeDelta)
{

	Super::Update(fTimeDelta);
}

void CGenericUI::Update_Late(const _float fTimeDelta)
{

	Super::Update_Late(fTimeDelta);
}

void CGenericUI::Ready_Before_Render(const _float fTimeDelta)
{

	Super::Ready_Before_Render(fTimeDelta);
}

HRESULT CGenericUI::Render()
{
	if (FAILED(Super::Render()))
		return E_FAIL;

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	Get_Component<CShader>()->Apply();
	Get_Component<CVIBuffer>()->Bind_Resource();
	Get_Component<CVIBuffer>()->Render();

	return S_OK;
}

HRESULT CGenericUI::Ready_Components(GENERIC_UI_DESC* pDesc)
{
	if (FAILED(Add_Component<CTexture>(ENUM_TO_UINT(ELevelType::LOGO), pDesc->wstrTextureTag, pDesc)))
		return E_FAIL;

	if (FAILED(Add_Component<CShader>(0, L"Prototype_Component_Shader_VtxPosTex", pDesc)))
		return E_FAIL;

	if (FAILED(Add_Component<CVIBuffer_Rect_Tex>(0, L"Prototype_Component_VIBuffer_Rect_Tex", pDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CGenericUI::Bind_ShaderResources()
{
	CShader* pShader = Get_Component<CShader>();
	if (FAILED(Get_Component<CTransform>()->Bind_ShaderResource(pShader)))
		return E_FAIL;

	if (FAILED(Get_Component<CTexture>()->Bind_ShaderResource(pShader, 0)))
		return E_FAIL;

	return S_OK;
}

CGenericUI* CGenericUI::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CGenericUI* pInstance = new CGenericUI(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CGenericUI::Create, Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CGenericUI::Clone(void* pArg)
{
	CGenericUI* pInstance = new CGenericUI(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CGenericUI::Clone, Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CGenericUI::Free()
{
	Super::Free();
}
