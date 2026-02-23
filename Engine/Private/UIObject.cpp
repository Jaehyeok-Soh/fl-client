#include "Engine_pch.h"
#include "UIObject.h"
#include "Shader.h"	
#include "Texture.h"
#include "Engine_Utils.h"
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

	m_isVisible = pDesc->isInitVisible;
	m_isInteract = pDesc->isInitInteract;
	m_isActive	= pDesc->isInitActivate;

	m_eCategory = RENDER_CATEGORY::BLEND;

	//if(pDesc->isWorld)
	//	m_eCategory = RENDER_CATEGORY::BLEND;
	//else
	//	m_eCategory = (pDesc->isAlpha ? RENDER_CATEGORY::BLENDUI : RENDER_CATEGORY::UI);

	m_fX = pDesc->fX;
	m_fY = pDesc->fY;
	m_fZ = pDesc->fZ;
	m_fWidth = 1.f;
	m_fHeight = 1.f;

	D3D11_VIEWPORT          ViewportDesc = {};
	_uint					iNumViewports = { 1 };

	m_pDeviceContext->RSGetViewports(&iNumViewports, &ViewportDesc);
	m_iViewportWidth = (_uint)ViewportDesc.Width;
	m_iViewportHeight = (_uint)ViewportDesc.Height;

	Set_Size(pDesc->fWidth, pDesc->fHeight);
	Set_Position(pDesc->fX, pDesc->fY, pDesc->fZ);

	if (FAILED(Add_Component<CShader>(0, L"Prototype_Component_Shader_VtxPosTex", pArg)))
		return E_FAIL;

	m_isPreVisible		= m_isVisible;
	m_isVisibleTrigger	= m_isVisible;
	m_isPreActive		= m_isActive;
	m_isPreInteract		= m_isInteract;
	m_isInteractTrigger	= m_isInteract;

	return S_OK;
}

HRESULT CUIObject::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;

	return S_OK;
}

void CUIObject::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CUIObject::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);

	if (m_isPreActive != m_isActive)
	{
		m_isPlaying_ActiveEvent = true;

		if (m_isActive)
			Initialize_Activate_Event();
		else
			Initialize_InActivate_Event();
	}

	if (m_isPreInteract != m_isInteractTrigger)
	{
		m_isPlaying_InteractEvent = true;
		if (m_isInteractTrigger)
			Initialize_Interactable_Event();
		else
			Initialize_NonInteractable_Event();
	}

	if (m_isPreVisible != m_isVisibleTrigger)
	{
		m_isPlaying_VisibleEvent = true;
		m_isVisible = true;

		if (m_isVisibleTrigger)
			Initialize_Visible_Event();
		else
			Initialize_InVisible_Event();
	}

	m_isPreActive = m_isActive;
	m_isPreInteract = m_isInteractTrigger;
	m_isPreVisible = m_isVisibleTrigger;
}

void CUIObject::Update_Late(const _float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);

	if (m_isPlaying_ActiveEvent)
	{
		if (m_isActive)		m_isPlaying_ActiveEvent = !Tick_Activate_Event(fTimeDelta);
		else				m_isPlaying_ActiveEvent = !Tick_InActivate_Event(fTimeDelta);
	}

	if (m_isPlaying_InteractEvent)
	{
		if (m_isInteractTrigger)	m_isPlaying_InteractEvent = !Tick_Interactable_Event(fTimeDelta);
		else						m_isPlaying_InteractEvent = !Tick_NonInteractable_Event(fTimeDelta);

		if (!m_isPlaying_InteractEvent)
		{
			m_isInteract = m_isInteractTrigger;
		}
	}

	if (m_isPlaying_VisibleEvent)
	{
		if (m_isVisibleTrigger)	m_isPlaying_VisibleEvent = !Tick_Visible_Event(fTimeDelta);
		else					m_isPlaying_VisibleEvent = !Tick_InVisible_Event(fTimeDelta);

		if (!m_isVisibleTrigger && !m_isPlaying_VisibleEvent)
		{
			m_isVisible = false;
			m_isVisibleTrigger = false;
		}
	}
}

void CUIObject::Ready_Before_Render(const _float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);
}

HRESULT CUIObject::Render()
{
	if (FAILED(Super::Render()))
		return E_FAIL;

	return S_OK;
}


void CUIObject::Set_Size(_float fWidth, _float fHeight)
{
	if (fWidth < 0.1f || fHeight < 0.1f)
		return;

	m_fWidth = fWidth;
	m_fHeight = fHeight;

	Get_Component<CTransform>()->Set_Scale(m_fWidth, m_fHeight, 1.f);
	SetUp_Rect();
}

void CUIObject::Set_Size(const Vec2 &vSize)
{
	Set_Size(vSize.x, vSize.y);
}

void CUIObject::Move_Size(_float fWidth, _float fHeight)
{
	if (fWidth < 0.1f || fHeight < 0.1f)
		return;
	Get_Component<CTransform>()->Set_Scale(fWidth, fHeight, 1.f);
	SetUp_Rect();
}

void CUIObject::Set_Position(const Vec3 &vPosition)
{
	Set_Position(vPosition.x, vPosition.y, vPosition.z);
}

void CUIObject::Set_Position(_float fX, _float fY, _float fZ)
{
	m_fX = fX;
	m_fY = fY;
	m_fZ = fZ;
	Get_Component<CTransform>()->Set_Info(TRANSFORM_INFO_STATE::POS, XMVectorSet(m_fX - m_iViewportWidth * 0.5f, -m_fY + m_iViewportHeight * 0.5f, m_fZ, 1.f));
	SetUp_Rect();
}

void CUIObject::Move_Position(_float fX, _float fY, _float fZ)
{
	Get_Component<CTransform>()->Set_Info(TRANSFORM_INFO_STATE::POS, XMVectorSet(fX - m_iViewportWidth * 0.5f, -fY + m_iViewportHeight * 0.5f, fZ, 1.f));
	SetUp_Rect();
}

void CUIObject::Located_Left_In_Viewport()
{
	Set_Position(m_fWidth * 0.5f, m_iViewportHeight * 0.5f, m_fZ);
}

void CUIObject::Located_Right_In_Viewport()
{
	Set_Position(m_iViewportWidth - m_fWidth * 0.5f, m_iViewportHeight * 0.5f, m_fZ);
}

void CUIObject::Located_Top_In_Viewport()
{
	Set_Position(m_iViewportWidth * 0.5f, m_fHeight * 0.5f, m_fZ);
}

void CUIObject::Located_Bottom_In_Viewport()
{
	Set_Position(m_iViewportWidth * 0.5f, m_iViewportHeight - m_fHeight * 0.5f, m_fZ);
}

void CUIObject::Located_LeftTop_In_Viewport()
{
	Set_Position(m_fWidth * 0.5f, m_fHeight * 0.5f, m_fZ);
}

void CUIObject::Located_RightTop_In_Viewport()
{
	Set_Position(m_iViewportWidth - m_fWidth * 0.5f, m_fHeight * 0.5f, m_fZ);
}

void CUIObject::Located_LeftBottom_In_Viewport()
{
	Set_Position(m_fWidth * 0.5f, m_iViewportHeight - m_fHeight * 0.5f, m_fZ);
}

void CUIObject::Located_RightBottom_In_Viewport()
{
	Set_Position(m_iViewportWidth - m_fWidth * 0.5f, m_iViewportHeight - m_fHeight * 0.5f, m_fZ);
}

void CUIObject::Set_SizeToTextureScale()
{
	if (!Get_Component<CTexture>())
		return;

	Vec2 vSize = Get_Component<CTexture>()->Get_TextureSize();
	Set_Size(vSize);
	m_fAspect = vSize.x / vSize.y;
	SetUp_Rect();
}

void CUIObject::SetUp_Rect()
{
	m_tRect.left = (LONG)(m_fX - m_fWidth / 2);
	m_tRect.right = (LONG)(m_fX + m_fWidth / 2);
	m_tRect.top = (LONG)(m_fY - m_fHeight / 2);
	m_tRect.bottom = (LONG)(m_fY + m_fHeight / 2);
}

void CUIObject::Free()
{
	Super::Free();
}
