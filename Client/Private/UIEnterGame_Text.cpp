#include "pch.h"
#include "UIEnterGame_Text.h"
#include "Client_Defines.h"
#include "Canvas.h"
#include "Texture.h"
#include "Shader.h"
#include "VIBuffer_Rect_Tex.h"
#include "GameInstance.h"
#include "Level_Loading.h"
#include <UI_Manager.h>

#define MOUSE_HOVER_ENTER_SLOT 0
#define MOUSE_HOVER_EXIT_SLOT 1
#define ENTERGAME_SLOT 2

CUIEnterGame_Text::CUIEnterGame_Text(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	:CUIText(pDevice, pDeviceContext)
{
}

CUIEnterGame_Text::CUIEnterGame_Text(const CUIEnterGame_Text& rhs)
	:CUIText(rhs)
{
}

HRESULT CUIEnterGame_Text::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIEnterGame_Text::Initialize(void* pArg)
{
	ENTERGAME_TEXT_DESC* pDesc = static_cast<ENTERGAME_TEXT_DESC*>(pArg);
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;
	if (FAILED(Ready_Components(pDesc)))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIEnterGame_Text::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;

	if (FAILED(Attach_Personal_Info()))
		return E_FAIL;
	return S_OK;
}


void CUIEnterGame_Text::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CUIEnterGame_Text::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);

	Tick_By_Type(fTimeDelta);
}

void CUIEnterGame_Text::Update_Late(const _float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);
}

void CUIEnterGame_Text::Ready_Before_Render(const _float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);
	if (FAILED(Convert_Stat_To_Text()))
		return;
}

HRESULT CUIEnterGame_Text::Render()
{
	if (!m_isVisible)
		return S_OK;
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;
	if (FAILED(Super::Render()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIEnterGame_Text::Ready_Components(ENTERGAME_TEXT_DESC* pDesc)
{
	if (FAILED(Super::Ready_Components(pDesc)))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIEnterGame_Text::Bind_ShaderResources()
{
	CShader* pShader = Get_Component<CShader>();
	if (FAILED(Get_Component<CTransform>()->Bind_ShaderResource(pShader)))
		return E_FAIL;
	if (FAILED(Super::Bind_ShaderResources()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIEnterGame_Text::Attach_Personal_Info()
{
	Set_Visible();

	switch (m_eTextSubClassType)
	{
	case DTO::EUITextSubClassType::ENTERGAME_GAMESTART_TEXT:
		m_fFontScale = 0.7f;
		Ready_LerpChange(0.1f, 0.7f, 0.7f, 1.f, m_fDelay);
		m_pGameInstance->PlayBGM(0, TO_HASH("UI_TITLE_BGM"), 0.5f);
		break;
	case DTO::EUITextSubClassType::ENTERGAME_GAMESTART_WORLDTEXT:
		m_pGameInstance->Play_OneShot(0, TO_HASH("UI_TITLE_AMBIENT_SOUND"), 1.f);
		break;
	}
	return S_OK;
}

HRESULT CUIEnterGame_Text::Convert_Stat_To_Text()
{
	return S_OK;
}

void CUIEnterGame_Text::Bind_Events()
{
	Super::Bind_Events();
	return;
}

void CUIEnterGame_Text::Initialize_Visible_Event()
{
}

void CUIEnterGame_Text::Initialize_InVisible_Event()
{
}

_bool CUIEnterGame_Text::Tick_Visible_Event(const _float fTimeDelta)
{
	return true;
}

_bool CUIEnterGame_Text::Tick_InVisible_Event(const _float fTimeDelta)
{
	return true;
}

void CUIEnterGame_Text::Tick_By_Type(const _float fTimeDelta)
{
	switch (m_eTextSubClassType)
	{
	case DTO::EUITextSubClassType::ENTERGAME_GAMESTART_TEXT:
	{
		if (Engine_Utils::Has_Flag(m_iInteractState, EUIInteract_Flag::HOVER_ENTER))
		{
			m_pParentCanvasCache->Get_CommonParam_bool_Ref()[MOUSE_HOVER_ENTER_SLOT] = true;
			m_pParentCanvasCache->Get_CommonParam_bool_Ref()[MOUSE_HOVER_EXIT_SLOT] = false;

			m_pGameInstance->Play_OneShot(0, TO_HASH("UI_TITLE_HOVER_ENTER_GAME"), 1.f);
			Ready_LerpChange(0.3f, m_fFontScale, 0.8f, 3.f, m_fDelay, true);
		}
		else if (Engine_Utils::Has_Flag(m_iInteractState, EUIInteract_Flag::HOVER_EXIT))
		{
			m_pParentCanvasCache->Get_CommonParam_bool_Ref()[MOUSE_HOVER_EXIT_SLOT] = true;
			m_pParentCanvasCache->Get_CommonParam_bool_Ref()[MOUSE_HOVER_ENTER_SLOT] = false;

			Ready_LerpChange(0.3f, m_fFontScale, 0.7f, 3.f, m_fDelay, true);
		}
		else if (Engine_Utils::Has_Flag(m_iInteractState, EUIInteract_Flag::PRESS_EXIT))
		{
			m_pParentCanvasCache->Get_CommonParam_bool_Ref()[ENTERGAME_SLOT] = true;

			m_pParentCanvasCache->Get_CommonParam_bool_Ref()[MOUSE_HOVER_EXIT_SLOT] = false;
			m_pParentCanvasCache->Get_CommonParam_bool_Ref()[MOUSE_HOVER_ENTER_SLOT] = false;

			m_pGameInstance->Play_OneShot(0, TO_HASH("UI_TITLE_ENTER_GAME"), 1.f);

			m_pGameInstance->Request_ChangeLevel(ENUM_TO_UINT(ELevelType::LOADING), CLevel_Loading::Create(m_pDevice, m_pDeviceContext, ELevelType::TUTORIAL_VILLAGE));
		}

		Tick_LerpChange(&m_fFontScale, fTimeDelta);
	}
		break;
	case DTO::EUITextSubClassType::ENTERGAME_GAMESTART_WORLDTEXT:
		m_fSfxSoundPlayTime += fTimeDelta;
		if (m_fSfxSoundPlayTime > 15.f)
		{
			m_pGameInstance->Play_OneShot(0, TO_HASH("UI_TITLE_AMBIENT_SOUND"), 1.f);
			m_fSfxSoundPlayTime = 0.f;
		}
		break;
	}
}

CUIEnterGame_Text* CUIEnterGame_Text::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CUIEnterGame_Text* pInstance = new CUIEnterGame_Text(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CUIEnterGame_Text::Create, Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUIEnterGame_Text::Clone(void* pArg)
{
	CUIEnterGame_Text* pInstance = new CUIEnterGame_Text(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CUIEnterGame_Text::Clone, Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUIEnterGame_Text::Free()
{
	Super::Free();
}
