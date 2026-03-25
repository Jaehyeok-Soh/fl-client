#include "pch.h"
#include "UITitle_Image.h"
#include "Client_Defines.h"
#include "Client_EventDefine.h"
//=================
// Component
//=================
#include "Canvas.h"
#include "WorldUI_Component.h"
#include "Texture.h"
#include "Shader.h"
#include "VIBuffer_Rect_Tex.h"
#include "UI_Manager.h"
#include "GameInstance.h"

#define TITLE_WARNING_END_SLOT 0
#define TITLE_INFO_END_SLOT 1
#define TITLE_ICON_END_SLOT 2

CUITitle_Image::CUITitle_Image(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	:CUIDynamic_Image(pDevice, pDeviceContext)
{
}

CUITitle_Image::CUITitle_Image(const CUITitle_Image& rhs)
	:CUIDynamic_Image(rhs)
{
}

HRESULT CUITitle_Image::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUITitle_Image::Initialize(void* pArg)
{
	TITLE_IMAGE_DESC* pDesc = static_cast<TITLE_IMAGE_DESC*>(pArg);
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;
	if (FAILED(Ready_Components(pDesc)))
		return E_FAIL;

	return S_OK;
}


HRESULT CUITitle_Image::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;
	if (FAILED(Attach_Personal_Info()))
		return E_FAIL;

	return S_OK;
}

void CUITitle_Image::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CUITitle_Image::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
	Tick_By_Type(fTimeDelta);
}

void CUITitle_Image::Update_Late(const _float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);
}

void CUITitle_Image::Ready_Before_Render(const _float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);
}

HRESULT CUITitle_Image::Render()
{
	if (!m_isVisible)
		return S_OK;
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;


	if (FAILED(Super::Render()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUITitle_Image::Ready_Components(TITLE_IMAGE_DESC* pDesc)
{
	if (FAILED(Super::Ready_Components(pDesc)))
		return E_FAIL;
	return S_OK;
}

HRESULT CUITitle_Image::Bind_ShaderResources()
{
	CShader* pShader = Get_Component<CShader>();
	if (FAILED(Get_Component<CTransform>()->Bind_ShaderResource(pShader)))
		return E_FAIL;

	if (m_eDImageSubClass == DTO::EUIDImageSubClassType::TITLE_NAME)
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

HRESULT CUITitle_Image::Attach_Personal_Info()
{
	switch (m_eDImageSubClass)
	{
	case DTO::EUIDImageSubClassType::TITLE_WARNING:
		Set_Visible();
		break;
	case DTO::EUIDImageSubClassType::TITLE_INFO:
		break;
	case DTO::EUIDImageSubClassType::TITLE_ICON:
		break;
	case DTO::EUIDImageSubClassType::TITLE_NAME:
	{
		m_iHorizontal = 7;
		m_iVertical = 6;

		m_vUVScale = Vec2{ 1.f / m_iHorizontal, 1.f / m_iVertical };
	}
		break;
	}

	return S_OK;
}

void CUITitle_Image::Bind_Events()
{
}

void CUITitle_Image::Tick_By_Type(const _float fTimeDelta)
{
	switch (m_eDImageSubClass)
	{
	case DTO::EUIDImageSubClassType::TITLE_WARNING:
		break;
	case DTO::EUIDImageSubClassType::TITLE_INFO:
		if (m_pParentCanvasCache->Get_CommonParam_bool()[TITLE_WARNING_END_SLOT])
		{
			if (!m_isTitleVisibleTrigger)
			{
				Set_Visible();
				m_isTitleVisibleTrigger = true;
			}
		}
		break;
	case DTO::EUIDImageSubClassType::TITLE_ICON:
		if (m_pParentCanvasCache->Get_CommonParam_bool()[TITLE_INFO_END_SLOT])
		{
			if (!m_isTitleVisibleTrigger)
			{
				Set_Visible();
				m_isTitleVisibleTrigger = true;
			}
		}
		break;
	case DTO::EUIDImageSubClassType::TITLE_NAME:
	{
		if (m_pParentCanvasCache->Get_CommonParam_bool()[TITLE_ICON_END_SLOT])
		{
			if (!m_isTitleVisibleTrigger)
			{
				Set_Visible();
				m_isTitleVisibleTrigger = true;
			}
		}
	}
		break;
	}
}

void CUITitle_Image::Initialize_Visible_Event()
{
	switch (m_eDImageSubClass)
	{
	case DTO::EUIDImageSubClassType::TITLE_WARNING:
		Ready_Fade(1.5f, 0.f, 1.f, m_fDelay);
		break;
	case DTO::EUIDImageSubClassType::TITLE_INFO:
		Ready_Fade(1.5f, 0.f, 1.f, m_fDelay);
		break;
	case DTO::EUIDImageSubClassType::TITLE_ICON:
		Ready_Fade(1.5f, 0.f, 1.f, m_fDelay);
		break;
	case DTO::EUIDImageSubClassType::TITLE_NAME:
		Ready_Fade(1.f, 0.f, 1.f, m_fDelay);

		break;
	}
}

_bool CUITitle_Image::Tick_Visible_Event(const _float fTimeDelta)
{
	switch (m_eDImageSubClass)
	{
	case DTO::EUIDImageSubClassType::TITLE_WARNING:
	{
		_bool isFade = Tick_Fade(fTimeDelta);

		if (isFade)
		{
			Set_Invisible();
			return true;
		}
	}
		break;
	case DTO::EUIDImageSubClassType::TITLE_INFO:
	{
		_bool isFade = Tick_Fade(fTimeDelta);

		if (isFade)
		{
			Set_Invisible();
			return true;
		}
	}
		break;
	case DTO::EUIDImageSubClassType::TITLE_ICON:
	{
		_bool isFade = Tick_Fade(fTimeDelta);

		if (isFade)
		{
			Set_Invisible();
			return true;
		}
	}
		break;
	case DTO::EUIDImageSubClassType::TITLE_NAME:
	{
		_bool isFade = Tick_Fade(fTimeDelta);

		if (m_pParentCanvasCache->Get_CommonParam_bool()[TITLE_ICON_END_SLOT])
		{
			m_fCurFrame += fTimeDelta * 30.f;

			_uint iMaxFrame = m_iHorizontal * m_iVertical;
			_uint iFrame = static_cast<_uint>(m_fCurFrame);

			if (iFrame >= iMaxFrame)
			{
				if (isFade)
					return true;
			}

			_uint iCol = iFrame % m_iHorizontal;
			_uint iRow = iFrame / m_iHorizontal;

			m_vUVOffset = Vec2
			{
				m_vUVScale.x * iCol,
				m_vUVScale.y * iRow
			};
		}
	}
		break;
	}

	return false;
}

void CUITitle_Image::Initialize_InVisible_Event()
{
	switch (m_eDImageSubClass)
	{
	case DTO::EUIDImageSubClassType::TITLE_WARNING:
		Ready_Fade(1.5f, 1.f, 0.f, 2.5f);
		break;
	case DTO::EUIDImageSubClassType::TITLE_INFO:
		Ready_Fade(1.5f, 1.f, 0.f, 2.5f);
		break;
	case DTO::EUIDImageSubClassType::TITLE_ICON:
		Ready_Fade(1.5f, 1.f, 0.f, 2.5f);
		break;
	case DTO::EUIDImageSubClassType::TITLE_NAME:
		break;
	}
}

_bool CUITitle_Image::Tick_InVisible_Event(const _float fTimeDelta)
{
	switch (m_eDImageSubClass)
	{
	case DTO::EUIDImageSubClassType::TITLE_WARNING:
	{
		_bool isFade = Tick_Fade(fTimeDelta);

		if (isFade)
		{
			Set_Active(false);
			if (!m_pParentCanvasCache->Get_CommonParam_bool()[TITLE_WARNING_END_SLOT])
				m_pParentCanvasCache->Get_CommonParam_bool_Ref()[TITLE_WARNING_END_SLOT] = true;

			return true;
		}
	}
		break;
	case DTO::EUIDImageSubClassType::TITLE_INFO:
	{
		_bool isFade = Tick_Fade(fTimeDelta);

		if (isFade)
		{
			Set_Active(false);
			if (!m_pParentCanvasCache->Get_CommonParam_bool()[TITLE_INFO_END_SLOT])
				m_pParentCanvasCache->Get_CommonParam_bool_Ref()[TITLE_INFO_END_SLOT] = true;

			return true;
		}
	}
		break;
	case DTO::EUIDImageSubClassType::TITLE_ICON:
	{
		_bool isFade = Tick_Fade(fTimeDelta);

		if (isFade)
		{
			Set_Active(false);
			if (!m_pParentCanvasCache->Get_CommonParam_bool()[TITLE_ICON_END_SLOT])
				m_pParentCanvasCache->Get_CommonParam_bool_Ref()[TITLE_ICON_END_SLOT] = true;

			return true;
		}
	}
		break;
	case DTO::EUIDImageSubClassType::TITLE_NAME:
		return true;
		break;
	}

	return false;
}

CUITitle_Image* CUITitle_Image::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CUITitle_Image* pInstance = new CUITitle_Image(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CUITitle_Image::Create, Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUITitle_Image::Clone(void* pArg)
{
	CUITitle_Image* pInstance = new CUITitle_Image(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CUITitle_Image::Clone, Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUITitle_Image::Free()
{
	Super::Free();
}
