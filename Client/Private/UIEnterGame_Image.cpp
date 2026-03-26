#include "pch.h"
#include "UIEnterGame_Image.h"
#include "Client_Defines.h"

#include "Canvas.h"
#include "Texture.h"
#include "Shader.h"
#include "VIBuffer_Rect_Tex.h"
#include "UI_Manager.h"
#include "GameInstance.h"


#define MOUSE_HOVER_ENTER_SLOT 0
#define MOUSE_HOVER_EXIT_SLOT 1
#define ENTERGAME_SLOT 2

CUIEnterGame_Image::CUIEnterGame_Image(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	:CUIDynamic_Image(pDevice, pDeviceContext)
{
}

CUIEnterGame_Image::CUIEnterGame_Image(const CUIEnterGame_Image& rhs)
	:CUIDynamic_Image(rhs)
{
}

HRESULT CUIEnterGame_Image::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIEnterGame_Image::Initialize(void* pArg)
{
	ENTERGAME_IMAGE_DESC* pDesc = static_cast<ENTERGAME_IMAGE_DESC*>(pArg);
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;
	if (FAILED(Ready_Components(pDesc)))
		return E_FAIL;
	return S_OK;
}


HRESULT CUIEnterGame_Image::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;
	if (FAILED(Attach_Personal_Info()))
		return E_FAIL;
	return S_OK;
}

void CUIEnterGame_Image::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CUIEnterGame_Image::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
	Tick_By_Type(fTimeDelta);
}

void CUIEnterGame_Image::Update_Late(const _float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);
}

void CUIEnterGame_Image::Ready_Before_Render(const _float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);
}

HRESULT CUIEnterGame_Image::Render()
{
	if (!m_isVisible)
		return S_OK;
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;
	if (FAILED(Super::Render()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIEnterGame_Image::Ready_Components(ENTERGAME_IMAGE_DESC* pDesc)
{
	if (FAILED(Super::Ready_Components(pDesc)))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIEnterGame_Image::Bind_ShaderResources()
{
	CShader* pShader = Get_Component<CShader>();
	if (FAILED(Get_Component<CTransform>()->Bind_ShaderResource(pShader)))
		return E_FAIL;

	if (m_eDImageSubClass == DTO::EUIDImageSubClassType::ENTERGAME_BG)
	{
		if (FAILED(pShader->Get_Variable("g_vScale")->SetRawValue(&m_vUVScale, 0, sizeof(Vec2))))
			return E_FAIL;
		if (FAILED(pShader->Get_Variable("g_vOffset")->SetRawValue(&m_vUVOffset, 0, sizeof(Vec2))))
			return E_FAIL;
	}

	if (FAILED(Super::Bind_ShaderResources()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIEnterGame_Image::Attach_Personal_Info()
{
	Set_Visible();

	switch (m_eDImageSubClass)
	{
	case DTO::EUIDImageSubClassType::ENTERGAME_BG:
	{
		m_iHorizontal = 12;
		m_iVertical = 18;

		m_vUVScale = Vec2{ 1.f / m_iHorizontal, 1.f / m_iVertical };
	}
		break;
	case DTO::EUIDImageSubClassType::ENTERGAME_ICONS:
		break;
	case DTO::EUIDImageSubClassType::ENTERGAME_QUIT_ICONS:
		break;
	case DTO::EUIDImageSubClassType::ENTERGAME_START_ICON_LEFT:
		break;
	case DTO::EUIDImageSubClassType::ENTERGAME_START_ICON_RIGHT:
		break;
	}
	return S_OK;
}

void CUIEnterGame_Image::Bind_Events()
{
	Super::Bind_Events();
}

void CUIEnterGame_Image::Tick_By_Type(const _float fTimeDelta)
{
	switch (m_eDImageSubClass)
	{
	case DTO::EUIDImageSubClassType::ENTERGAME_BG:
	{
		m_fCurFrame += fTimeDelta * 30.f;

		_uint iMaxFrame = m_iHorizontal * m_iVertical;
		_uint iFrame = static_cast<_uint>(m_fCurFrame);

		if (iMaxFrame == 0)
			return;

		if (iFrame >= iMaxFrame)
		{
			iFrame = 0;
			m_fCurFrame = static_cast<_float>(iFrame);
		}

		_uint iCol = iFrame % m_iHorizontal;
		_uint iRow = iFrame / m_iHorizontal;

		m_vUVOffset = Vec2
		{
			m_vUVScale.x * iCol,
			m_vUVScale.y * iRow
		};
	}
		break;
	case DTO::EUIDImageSubClassType::ENTERGAME_ICONS:
		break;
	case DTO::EUIDImageSubClassType::ENTERGAME_QUIT_ICONS:
		break;
	case DTO::EUIDImageSubClassType::ENTERGAME_START_ICON_LEFT:
		if (m_pParentCanvasCache->Get_CommonParam_bool()[MOUSE_HOVER_ENTER_SLOT])
		{
			if (!m_isHoverTrigger)
			{
				m_isHoverTrigger = true;
				Ready_Lerp_Movement(m_vMoveOffset, Vec2{ -30.f, 0.f }, 0.5f, 3.f, m_fDelay, true);
			}
		}
		else if (m_pParentCanvasCache->Get_CommonParam_bool()[MOUSE_HOVER_EXIT_SLOT])
		{
			if (m_isHoverTrigger)
			{
				m_isHoverTrigger = false;
				Ready_Lerp_Movement(m_vMoveOffset, Vec2{ 0.f, 0.f }, 0.5f, 3.f, m_fDelay, true);
			}
		}
		Tick_Lerp_Movement(fTimeDelta);
		break;
	case DTO::EUIDImageSubClassType::ENTERGAME_START_ICON_RIGHT:
		if (m_pParentCanvasCache->Get_CommonParam_bool()[MOUSE_HOVER_ENTER_SLOT])
		{
			if (!m_isHoverTrigger)
			{
				m_isHoverTrigger = true;
				Ready_Lerp_Movement(m_vMoveOffset, Vec2{ 30.f, 0.f }, 0.5f, 3.f, m_fDelay, true);
			}
		}
		else if (m_pParentCanvasCache->Get_CommonParam_bool()[MOUSE_HOVER_EXIT_SLOT])
		{
			if (m_isHoverTrigger)
			{
				m_isHoverTrigger = false;
				Ready_Lerp_Movement(m_vMoveOffset, Vec2{ 0.f, 0.f }, 0.5f, 3.f, m_fDelay, true);
			}
		}
		Tick_Lerp_Movement(fTimeDelta);
		break;
	}
}

void CUIEnterGame_Image::Initialize_Visible_Event()
{
	Ready_Fade(0.3f, 0.f, 1.f, m_fDelay);
}

_bool CUIEnterGame_Image::Tick_Visible_Event(const _float fTimeDelta)
{
	_bool isFade = Tick_Fade(fTimeDelta);
	if (isFade)
	{
		return true;
	}
	return false;
}

void CUIEnterGame_Image::Initialize_InVisible_Event()
{
}

_bool CUIEnterGame_Image::Tick_InVisible_Event(const _float fTimeDelta)
{
	return true;
}

CUIEnterGame_Image* CUIEnterGame_Image::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CUIEnterGame_Image* pInstance = new CUIEnterGame_Image(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CUIEnterGame_Image::Create, Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUIEnterGame_Image::Clone(void* pArg)
{
	CUIEnterGame_Image* pInstance = new CUIEnterGame_Image(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CUIEnterGame_Image::Clone, Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUIEnterGame_Image::Free()
{
	Super::Free();
}
