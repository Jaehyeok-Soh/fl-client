#include "pch.h"
#include "ToolUI.h"

#include "Engine_Utils.h"

/* Components */
#include "Shader.h"
#include "VIBuffer_Rect_Tex.h"
#include "Texture.h"
#include "GameInstance.h"

CToolUI::CToolUI(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	:CUIObject(pDevice, pDeviceContext)
{
}

CToolUI::CToolUI(const CToolUI& rhs)
	:CUIObject(rhs)
{
}

HRESULT CToolUI::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;
	return S_OK;
}

HRESULT CToolUI::Initialize(void* pArg)
{
	TOOLUI_DESC* pDesc = static_cast<TOOLUI_DESC*>(pArg);
	m_strName = pDesc->strName;
	m_wstrTextureTag = Engine_Utils::ToWString(pDesc->strInitTextureTag);
	m_iTextureIndex = pDesc->iInitTextureIndex;

	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

    if (FAILED(Ready_Components(pDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CToolUI::Awake(const _uint iCurrentLevelID)
{   
	if (FAILED(Super::Awake(iCurrentLevelID)))
        return E_FAIL;

    return S_OK;
}

void CToolUI::Update_Priority(const _float fTimeDelta)
{
	Set_Size(m_fWidth, m_fHeight);
	Set_Position(m_fX, m_fY, m_fZ);
	Super::Update_Priority(fTimeDelta);
}

void CToolUI::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
}

void CToolUI::Update_Late(const _float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);
}

void CToolUI::Ready_Before_Render(const _float fTimeDelta)
{
	Sync_Data();
	Super::Ready_Before_Render(fTimeDelta);
}

HRESULT CToolUI::Render()
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

HRESULT CToolUI::Ready_Components(TOOLUI_DESC* pDesc)
{ 
	if (FAILED(Add_Component<CTexture>(ENUM_TO_UINT(ELevelType::UI), m_wstrTextureTag, pDesc)))
        return E_FAIL;

    if (FAILED(Add_Component<CShader>(0, L"Prototype_Component_Shader_VtxPosTex", pDesc)))
        return E_FAIL;

    if (FAILED(Add_Component<CVIBuffer_Rect_Tex>(0, L"Prototype_Component_VIBuffer_Rect_Tex", pDesc)))
        return E_FAIL;

	return S_OK;
}

HRESULT CToolUI::Bind_ShaderResources()
{
    CShader* pShader = Get_Component<CShader>();

    if (FAILED(Get_Component<CTransform>()->Bind_ShaderResource(pShader)))
        return E_FAIL;

    if (FAILED(Get_Component<CTexture>()->Bind_ShaderResource(pShader, m_iTextureIndex)))
        return E_FAIL;

    return S_OK;
}

void CToolUI::Sync_Data()
{
	m_tUIData.iRectTransformType =static_cast<uint32_t>( m_eRectTransformType);
	m_tUIData.iUIType = static_cast<uint32_t>(m_eUIType);
	m_tUIData.strName = m_strName;
	m_tUIData.fWidth = m_fWidth;
	m_tUIData.fHeight = m_fHeight;
	m_tUIData.fPosX = m_fX;
	m_tUIData.fPosY = m_fY;
	m_tUIData.fPosZ = m_fZ;
}

CToolUI* CToolUI::Create(EToolObjectType eType, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CToolUI* pInstance = new CToolUI(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CToolUI::Create, Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CToolUI::Clone(void* pArg)
{
	CToolUI* pInstance = new CToolUI(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CToolUI::Clone, Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CToolUI::Free()
{
	Super::Free();
}

