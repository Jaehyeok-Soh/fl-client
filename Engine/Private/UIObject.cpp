#include "UIObject.h"
#include "Shader.h"
#include "Texture.h"
#include "Mesh.h"
#include "GameInstance.h"

CUIObject::CUIObject(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: Super(pDevice, pDeviceContext)
{
}
 
CUIObject::CUIObject(const CUIObject& rhs)
	: Super(rhs)
{
}

HRESULT CUIObject::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUIObject::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	UIOBJECT_DESC* pDesc = static_cast<UIOBJECT_DESC*>(pArg);
	m_fX = pDesc->fX;
	m_fY = pDesc->fY;
	m_fSizeX = pDesc->fSizeX;
	m_fSizeY = pDesc->fSizeY;
	m_eCategory = pDesc->bAlpha == true ? RENDER_CATEGORY::BLENDUI : RENDER_CATEGORY::UI;


	D3D11_VIEWPORT          ViewportDesc = {};
	_uint					iNumViewports = { 1 };
	m_pDeviceContext->RSGetViewports(&iNumViewports, &ViewportDesc);
	m_iViewportWidth = (_uint)ViewportDesc.Width;
	m_iViewportHeight = (_uint)ViewportDesc.Height;

	Set_Size(pDesc->fSizeX, pDesc->fSizeY);
	Set_Position(pDesc->fX, pDesc->fY);

	if (FAILED(Add_Component<CShader>(0, L"Prototype_Component_Shader_VtxPosTex", pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CUIObject::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;

	if (m_eCategory == RENDER_CATEGORY::BLENDUI)
		Get_Component<CShader>()->Set_Pass(1);

	return S_OK;
}

void CUIObject::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CUIObject::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
}

void CUIObject::Update_Late(const _float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);
}

void CUIObject::Ready_Before_Render(const _float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);
	m_pGameInstance->Push_RenderObject(RENDER_CATEGORY::UI, this);
}

HRESULT CUIObject::Render()
{
	if (FAILED(Super::Render()))
		return E_FAIL;

	return S_OK;
}

_bool CUIObject::IsPicked()
{
	if (::PtInRect(&m_tRect, m_pGameInstance->Get_MousePos()))
	{
		m_bPicked = true;
		return true;
	}
	else
	{
		if (m_bPicked)
			OffPicked();
		m_bPicked = false;
		return false;
	}
}

void CUIObject::OffPicked()
{

}

void CUIObject::Set_Size(_float fX, _float fY)
{
	m_fSizeX = fX;
	m_fSizeY = fY;
	Get_Component<CTransform>()->Set_Scale(m_fSizeX, m_fSizeY, 1.f);
	SetUp_Rect();
}

void CUIObject::Set_Size(const Vec2 &vSize)
{
	Set_Size(vSize.x, vSize.y);
}

void CUIObject::Set_Position(const Vec2 &vPosition)
{
	Set_Position(vPosition.x, vPosition.y);
}

void CUIObject::Set_Position(_float fX, _float fY)
{
	m_fX = fX;
	m_fY = fY;

	Get_Component<CTransform>()->Set_Info(TRANSFORM_INFO_STATE::POS, XMVectorSet(
		m_fX - m_iViewportWidth * 0.5f,
		-m_fY + m_iViewportHeight * 0.5f, 0.f, 1.f));
	SetUp_Rect();
}

void CUIObject::Move_Position_Lerp(const Vec2 &vTargetPosition)
{
	// TODO
	// Transform에 만들어놓고 꺼내쓰자
}

void CUIObject::Located_Left_In_Viewport()
{
	Set_Position(m_fSizeX * 0.5f, m_iViewportHeight * 0.5f);
}

void CUIObject::Located_Right_In_Viewport()
{
	Set_Position(m_iViewportWidth - m_fSizeX * 0.5f, m_iViewportHeight * 0.5f);
}

void CUIObject::Located_Top_In_Viewport()
{
	Set_Position(m_iViewportWidth * 0.5f, m_fSizeY * 0.5f);
}

void CUIObject::Located_Bottom_In_Viewport()
{
	Set_Position(m_iViewportWidth * 0.5f, m_iViewportHeight - m_fSizeY * 0.5f);
}

void CUIObject::Located_LeftTop_In_Viewport()
{
	Set_Position(m_fSizeX * 0.5f, m_fSizeY * 0.5f);
}

void CUIObject::Located_RightTop_In_Viewport()
{
	Set_Position(m_iViewportWidth - m_fSizeX * 0.5f, m_fSizeY * 0.5f);
}

void CUIObject::Located_LeftBottom_In_Viewport()
{
	Set_Position(m_fSizeX * 0.5f, m_iViewportHeight - m_fSizeY * 0.5f);
}

void CUIObject::Located_RightBottom_In_Viewport()
{
	Set_Position(m_iViewportWidth - m_fSizeX * 0.5f, m_iViewportHeight - m_fSizeY * 0.5f);
}

void CUIObject::Set_SizeToTextureScale()
{
	if (!Get_Component<CTexture>())
		return;
	Vec2 vSize = Get_Component<CTexture>()->Get_TextureSize();
	vSize.x *= m_fSizeX;
	vSize.y *= m_fSizeY;
	Set_Size(vSize);
	m_fRatio = vSize.x / vSize.y;
	SetUp_Rect();
}

void CUIObject::SetUp_Rect()
{
	m_tRect.left = (LONG)(m_fX - m_fSizeX / 2);
	m_tRect.right = (LONG)(m_fX + m_fSizeX / 2);
	m_tRect.top = (LONG)(m_fY - m_fSizeY / 2);
	m_tRect.bottom = (LONG)(m_fY + m_fSizeY / 2);
}

void CUIObject::Free()
{
	Safe_Release(m_pTexture);
	Safe_Release(m_pShader);
	Safe_Release(m_pMesh);
	Super::Free();
}
