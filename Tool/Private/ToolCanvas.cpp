#include "pch.h"

#include "ToolCanvas.h"
#include "Tool_Defines.h"
#include "GameInstance.h"

/* Components */
#include "Shader.h"
#include "VIBuffer_Rect_Tex.h"
#include "Texture.h"

CToolCanvas::CToolCanvas(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	:CUIObject(pDevice, pDeviceContext)
{
}

CToolCanvas::CToolCanvas(const CToolCanvas& rhs)
	:CUIObject(rhs)
{
}

HRESULT CToolCanvas::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;
	return S_OK;
}

HRESULT CToolCanvas::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	TOOLUI_DESC* pDesc = static_cast<TOOLUI_DESC*>(pArg);

	if (FAILED(Ready_Components(pDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CToolCanvas::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;

	Set_SizeToTextureScale();
	return S_OK;
}

void CToolCanvas::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CToolCanvas::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
}

void CToolCanvas::Update_Late(const _float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);
}

void CToolCanvas::Ready_Before_Render(const _float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);
}

HRESULT CToolCanvas::Render()
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

HRESULT CToolCanvas::Ready_Components(TOOLUI_DESC* pDesc)
{
	if (FAILED(Add_Component<CTexture>(0, pDesc->wstrTextureTag, pDesc)))
		return E_FAIL;

	if (FAILED(Add_Component<CShader>(0, L"Prototype_Component_Shader_VtxPosTex", pDesc)))
		return E_FAIL;

	if (FAILED(Add_Component<CVIBuffer_Rect_Tex>(0, L"Prototype_Component_VIBuffer_Rect_Tex", pDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CToolCanvas::Bind_ShaderResources()
{
	CShader* pShader = Get_Component<CShader>();
	if (FAILED(Get_Component<CTransform>()->Bind_ShaderResource(pShader)))
		return E_FAIL;

	if (FAILED(Get_Component<CTexture>()->Bind_ShaderResource(pShader, 0)))
		return E_FAIL;

	return S_OK;
}

CToolCanvas* CToolCanvas::Create(EToolObjectType eType, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CToolCanvas* pInstance = new CToolCanvas(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CToolCanvas::Create, Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CToolCanvas::Clone(void* pArg)
{
	CToolCanvas* pInstance = new CToolCanvas(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CToolCanvas::Clone, Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CToolCanvas::Free()
{
	Super::Free();
}

