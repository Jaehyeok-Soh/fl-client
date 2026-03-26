#include "pch.h"
#include "Engine_Enum.h"
#include "UITitle_Text.h"
#include "Client_Defines.h"
#include "Client_EventDefine.h"

//=================
// Component
//=================
#include "Canvas.h"
#include "WorldUI_Component.h"
#include "MyStat.h"
#include "Texture.h"
#include "Shader.h"
#include "VIBuffer_Rect_Tex.h"
#include "GameInstance.h"
#include "QuestManager.h"
#include <UI_Manager.h>

#define TITLE_WARNING_END_SLOT 0
#define TITLE_INFO_END_SLOT 1

CUITitle_Text::CUITitle_Text(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	:CUIText(pDevice, pDeviceContext)
{
}

CUITitle_Text::CUITitle_Text(const CUITitle_Text& rhs)
	:CUIText(rhs)
{
}

HRESULT CUITitle_Text::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUITitle_Text::Initialize(void* pArg)
{
	TITLE_TEXT_DESC* pDesc = static_cast<TITLE_TEXT_DESC*>(pArg);
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;
	if (FAILED(Ready_Components(pDesc)))
		return E_FAIL;
	return S_OK;
}

HRESULT CUITitle_Text::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;

	if (FAILED(Attach_Personal_Info()))
		return E_FAIL;
	return S_OK;
}


void CUITitle_Text::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CUITitle_Text::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);

	Tick_By_Type(fTimeDelta);
}

void CUITitle_Text::Update_Late(const _float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);
}

void CUITitle_Text::Ready_Before_Render(const _float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);
	if (FAILED(Convert_Stat_To_Text()))
		return;
}

HRESULT CUITitle_Text::Render()
{
	if (!m_isVisible)
		return S_OK;
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;
	if (FAILED(Super::Render()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUITitle_Text::Ready_Components(TITLE_TEXT_DESC* pDesc)
{
	if (FAILED(Super::Ready_Components(pDesc)))
		return E_FAIL;
	return S_OK;
}

HRESULT CUITitle_Text::Bind_ShaderResources()
{
	CShader* pShader = Get_Component<CShader>();
	if (FAILED(Get_Component<CTransform>()->Bind_ShaderResource(pShader)))
		return E_FAIL;
	if (FAILED(Super::Bind_ShaderResources()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUITitle_Text::Attach_Personal_Info()
{
	switch (m_eTextSubClassType)
	{
	case DTO::EUITextSubClassType::TITLE_WARNING:
		Set_Visible();
		break;
	case DTO::EUITextSubClassType::TITLE_INFO:
		break;
	}

	return S_OK;
}

HRESULT CUITitle_Text::Convert_Stat_To_Text()
{
	return S_OK;
}

void CUITitle_Text::Bind_Events()
{
	return;
}

void CUITitle_Text::Initialize_Visible_Event()
{
	switch (m_eTextSubClassType)
	{
	case DTO::EUITextSubClassType::TITLE_WARNING:
		Ready_Fade_Text(1.5f, 0.f, 1.f, m_fDelay);
		break;
	case DTO::EUITextSubClassType::TITLE_INFO:
		Ready_Fade_Text(1.5f, 0.f, 1.f, m_fDelay);
		break;
	}
}

void CUITitle_Text::Initialize_InVisible_Event()
{
	switch (m_eTextSubClassType)
	{
	case DTO::EUITextSubClassType::TITLE_WARNING:
		Ready_Fade_Text(1.5f, 1.f, 0.f, 2.5f);
		break;
	case DTO::EUITextSubClassType::TITLE_INFO:
		Ready_Fade_Text(1.5f, 1.f, 0.f, 2.5f);
		break;
	}
}

_bool CUITitle_Text::Tick_Visible_Event(const _float fTimeDelta)
{
	switch (m_eTextSubClassType)
	{
	case DTO::EUITextSubClassType::TITLE_WARNING:
	{
		_bool isFade = Tick_Fade_Text(fTimeDelta);

		if (isFade)
		{
			Set_Invisible();
			return true;
		}
	}
		break;
	case DTO::EUITextSubClassType::TITLE_INFO:
	{
		_bool isFade = Tick_Fade_Text(fTimeDelta);

		if (isFade)
		{
			Set_Invisible();
			return true;
		}
	}
	break;
		break;
	}
	return false;
}

_bool CUITitle_Text::Tick_InVisible_Event(const _float fTimeDelta)
{
	switch (m_eTextSubClassType)
	{
	case DTO::EUITextSubClassType::TITLE_WARNING:
	{
		_bool isFade = Tick_Fade_Text(fTimeDelta);

		if (isFade)
		{
			return true;
		}
	}
	break;
	case DTO::EUITextSubClassType::TITLE_INFO:
	{
		_bool isFade = Tick_Fade_Text(fTimeDelta);

		if (isFade)
		{
			return true;
		}
	}
	break;
	}

	return false;
}

void CUITitle_Text::Tick_By_Type(const _float fTimeDelta)
{
	switch (m_eTextSubClassType)
	{
	case DTO::EUITextSubClassType::TITLE_WARNING:

	break;
	case DTO::EUITextSubClassType::TITLE_INFO:
	{

		if (m_pParentCanvasCache->Get_CommonParam_bool()[TITLE_WARNING_END_SLOT])
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

CUITitle_Text* CUITitle_Text::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CUITitle_Text* pInstance = new CUITitle_Text(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CUITitle_Text::Create, Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUITitle_Text::Clone(void* pArg)
{
	CUITitle_Text* pInstance = new CUITitle_Text(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CUITitle_Text::Clone, Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUITitle_Text::Free()
{
	Super::Free();
}
