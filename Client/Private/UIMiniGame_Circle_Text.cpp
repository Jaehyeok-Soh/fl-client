#include "pch.h"
#include "UIMiniGame_Circle_Text.h"
#include "Client_Defines.h"
#include "Client_EventDefine.h"

//=================
// Component
//=================
#include "Canvas.h"
#include "IInteractable.h"
#include "WorldUI_Component.h"
#include "MyStat.h"
#include "Texture.h"
#include "Shader.h"
#include "VIBuffer_Rect_Tex.h"
#include "GameInstance.h"
#include "QuestManager.h"
#include "DialogueManager.h"
#include <UI_Manager.h>

#define FAILED_TO_CLEAR_SLOT 7

#define TIME_LIMIT 12.f

CUIMiniGame_Circle_Text::CUIMiniGame_Circle_Text(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	:CUIText(pDevice, pDeviceContext)
{
}

CUIMiniGame_Circle_Text::CUIMiniGame_Circle_Text(const CUIMiniGame_Circle_Text& rhs)
	:CUIText(rhs)
{
}

HRESULT CUIMiniGame_Circle_Text::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIMiniGame_Circle_Text::Initialize(void* pArg)
{
	MINIGAME_CIRCLE_TEXT_DESC* pDesc = static_cast<MINIGAME_CIRCLE_TEXT_DESC*>(pArg);
	m_iNumbering = pDesc->iNumber;
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;
	if (FAILED(Ready_Components(pDesc)))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIMiniGame_Circle_Text::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;

	if (FAILED(Attach_Personal_Info()))
		return E_FAIL;
	return S_OK;
}


void CUIMiniGame_Circle_Text::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CUIMiniGame_Circle_Text::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);

	Tick_By_Type(fTimeDelta);
}

void CUIMiniGame_Circle_Text::Update_Late(const _float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);
}

void CUIMiniGame_Circle_Text::Ready_Before_Render(const _float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);
	if (FAILED(Convert_Stat_To_Text()))
		return;
}

HRESULT CUIMiniGame_Circle_Text::Render()
{
	if (!m_isVisible)
		return S_OK;
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;
	if (FAILED(Super::Render()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIMiniGame_Circle_Text::Ready_Components(MINIGAME_CIRCLE_TEXT_DESC* pDesc)
{
	if (FAILED(Super::Ready_Components(pDesc)))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIMiniGame_Circle_Text::Bind_ShaderResources()
{
	CShader* pShader = Get_Component<CShader>();
	if (FAILED(Get_Component<CTransform>()->Bind_ShaderResource(pShader)))
		return E_FAIL;
	if (FAILED(Super::Bind_ShaderResources()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIMiniGame_Circle_Text::Attach_Personal_Info()
{
	switch (m_eTextSubClassType)
	{
	case DTO::EUITextSubClassType::MINIGAME_CIRCLE_TIMER_TEXT:
		break;
	case DTO::EUITextSubClassType::MINIGAME_CIRCLE_CLEAR_TEXT:
		break;
	case DTO::EUITextSubClassType::MINIGAME_CIRCLE_CLEAR_WORLD_TEXT:
		break;
	}

	return S_OK;
}

HRESULT CUIMiniGame_Circle_Text::Convert_Stat_To_Text()
{
	return S_OK;
}

void CUIMiniGame_Circle_Text::Bind_Events()
{
	Super::Bind_Events();

	m_vecEventHandles.push_back(
		m_pGameInstance->Subscribe<MINIGAME_CIRCLE_ON>([this]()
			{
				Set_Active(true);
				m_isFirstEntered = false;

				switch (m_eTextSubClassType)
				{
				case DTO::EUITextSubClassType::MINIGAME_CIRCLE_TIMER_TEXT:
					Set_Visible();
					Set_Active(true);

					m_fMiniGameTimer = TIME_LIMIT;

					break;
				case DTO::EUITextSubClassType::MINIGAME_CIRCLE_CLEAR_TEXT:
				case DTO::EUITextSubClassType::MINIGAME_CIRCLE_CLEAR_WORLD_TEXT:
					break;
				}
			}));

	m_vecEventHandles.push_back(
		m_pGameInstance->Subscribe<MINIGAME_CIRCLE_CLEAR>([this]()
			{
				switch (m_eTextSubClassType)
				{
				case DTO::EUITextSubClassType::MINIGAME_CIRCLE_TIMER_TEXT:
				case DTO::EUITextSubClassType::MINIGAME_CIRCLE_CLEAR_TEXT:
				case DTO::EUITextSubClassType::MINIGAME_CIRCLE_CLEAR_WORLD_TEXT:
					Set_Invisible();
					break;
				}
			}));

	m_vecEventHandles.push_back(
		m_pGameInstance->Subscribe<MINIGAME_CIRCLE_OFF>([this]()
			{
			}));
}

void CUIMiniGame_Circle_Text::Initialize_Visible_Event()
{
	switch (m_eTextSubClassType)
	{
	case DTO::EUITextSubClassType::MINIGAME_CIRCLE_TIMER_TEXT:
		Ready_Fade_Text(0.5f, 0.f, 1.f, m_fDelay);
		Ready_LerpChange(0.5f, 2.f, 1.f, 3.f, m_fDelay, true);
		break;
	case DTO::EUITextSubClassType::MINIGAME_CIRCLE_CLEAR_TEXT:
	case DTO::EUITextSubClassType::MINIGAME_CIRCLE_CLEAR_WORLD_TEXT:
		Ready_Fade_Text(0.5f, 0.f, 0.f, m_fDelay);
		break;
	}
}

void CUIMiniGame_Circle_Text::Initialize_InVisible_Event()
{
	switch (m_eTextSubClassType)
	{
	case DTO::EUITextSubClassType::MINIGAME_CIRCLE_TIMER_TEXT:
		Ready_Fade_Text(0.5f, 1.f, 0.f, 0.3f);
		Ready_LerpChange(0.5f, 1.f, 2.f, 3.f, 0.3f, true);
		break;
	case DTO::EUITextSubClassType::MINIGAME_CIRCLE_CLEAR_TEXT:
	case DTO::EUITextSubClassType::MINIGAME_CIRCLE_CLEAR_WORLD_TEXT:
		Ready_Fade_Text(0.5f, 1.f, 0.f, 0.3f);
		break;
	}

}

_bool CUIMiniGame_Circle_Text::Tick_Visible_Event(const _float fTimeDelta)
{
	switch (m_eTextSubClassType)
	{
	case DTO::EUITextSubClassType::MINIGAME_CIRCLE_TIMER_TEXT:
	{
		_bool isFade = Tick_Fade_Text(fTimeDelta);
		_bool isChange = Tick_LerpChange(&m_fFontScale, fTimeDelta);

		if (isFade && isChange)
		{
			return true;
		}
	}
	break;
	case DTO::EUITextSubClassType::MINIGAME_CIRCLE_CLEAR_TEXT:
	case DTO::EUITextSubClassType::MINIGAME_CIRCLE_CLEAR_WORLD_TEXT:
	{
		_bool isFade = Tick_Fade_Text(fTimeDelta);

		if (isFade)
			return true;
	}
		break;
	}
	return false;
}

_bool CUIMiniGame_Circle_Text::Tick_InVisible_Event(const _float fTimeDelta)
{
	switch (m_eTextSubClassType)
	{
	case DTO::EUITextSubClassType::MINIGAME_CIRCLE_TIMER_TEXT:
	{
		_bool isFade = Tick_Fade_Text(fTimeDelta);
		_bool isChange = Tick_LerpChange(&m_fFontScale, fTimeDelta);

		if (isFade && isChange)
		{
			Set_Active(false);
			return true;
		}
	}
	break;
	case DTO::EUITextSubClassType::MINIGAME_CIRCLE_CLEAR_TEXT:
	case DTO::EUITextSubClassType::MINIGAME_CIRCLE_CLEAR_WORLD_TEXT:
	{
		_bool isFade = Tick_Fade_Text(fTimeDelta);
		if (isFade)
			return true;
	}
		break;
	}
	return false;

}

void CUIMiniGame_Circle_Text::Tick_By_Type(const _float fTimeDelta)
{
	switch (m_eTextSubClassType)
	{
	case DTO::EUITextSubClassType::MINIGAME_CIRCLE_TIMER_TEXT:
		Tick_For_TimerText(fTimeDelta);
		break;
	case DTO::EUITextSubClassType::MINIGAME_CIRCLE_CLEAR_TEXT:
		break;
	case DTO::EUITextSubClassType::MINIGAME_CIRCLE_CLEAR_WORLD_TEXT:
		break;
	}
}

void CUIMiniGame_Circle_Text::Tick_For_TimerText(const _float fTimeDelta)
{
	m_fMiniGameTimer -= fTimeDelta;

	_uint i = static_cast<_uint>(m_fMiniGameTimer);
	_wstring wstr = std::to_wstring(i);

	if (m_fMiniGameTimer <= 0.f)
	{
		m_fMiniGameTimer = TIME_LIMIT;
		m_pParentCanvasCache->Get_CommonParam_bool_Ref()[FAILED_TO_CLEAR_SLOT] = true;
	}

	if (i < 10)
		wstr = L"0" + wstr;

	m_wstrText = L"00:" + wstr;
}

CUIMiniGame_Circle_Text* CUIMiniGame_Circle_Text::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CUIMiniGame_Circle_Text* pInstance = new CUIMiniGame_Circle_Text(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CUIMiniGame_Circle_Text::Create, Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUIMiniGame_Circle_Text::Clone(void* pArg)
{
	CUIMiniGame_Circle_Text* pInstance = new CUIMiniGame_Circle_Text(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CUIMiniGame_Circle_Text::Clone, Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUIMiniGame_Circle_Text::Free()
{
	Super::Free();
}
