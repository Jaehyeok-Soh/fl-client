#include "pch.h"
#include "UILevelChange_Image.h"
#include "Client_Defines.h"
//=================
// Component
//=================
#include "StatCom_Player.h"
#include "Texture.h"
#include "Shader.h"
#include "VIBuffer_Rect_Tex.h"

#include "Level_Loading.h"
#include "UI_Manager.h"
#include "GameInstance.h"

CUILevelChange_Image::CUILevelChange_Image(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	:CUIDynamic_Image(pDevice, pDeviceContext)
{
}

CUILevelChange_Image::CUILevelChange_Image(const CUILevelChange_Image& rhs)
	:CUIDynamic_Image(rhs)
{
}

HRESULT CUILevelChange_Image::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUILevelChange_Image::Initialize(void* pArg)
{
	LEVEL_CHANGE_DESC* pDesc = static_cast<LEVEL_CHANGE_DESC*>(pArg);
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;
	if (FAILED(Ready_Components(pDesc)))
		return E_FAIL;
	return S_OK;
}


HRESULT CUILevelChange_Image::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;
	if (FAILED(Attach_Personal_Info()))
		return E_FAIL;
	return S_OK;
}

void CUILevelChange_Image::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CUILevelChange_Image::Update(const _float fTimeDelta)
{
	if (m_isHover)
	{
		FONT_DESC Desc = {};
		Desc.eFontShaderType	= EFontShaderType::OUTLINE_NOISE;
		Desc.strFontTag			= L"ContentsKO24";
		Desc.strText			= m_wstrText;
		Desc.vPosition			= Vec2{ m_vRenderPos.x - 50.f, m_vRenderPos.y };
		Desc.fRotate			= 0.f;
		Desc.fScale				= 0.7f;
		Desc.vColor				= m_vFontColor;
		Desc.ePivot				= EFontPivotType::RIGHT;
		m_pGameInstance->Request_DrawFont(Desc);
	}

	Super::Update(fTimeDelta);
}

void CUILevelChange_Image::Update_Late(const _float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);
	Trigger_By_InteractState();
}

void CUILevelChange_Image::Ready_Before_Render(const _float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);
}

HRESULT CUILevelChange_Image::Render()
{
	if (!m_isVisible)
		return S_OK;
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;
	if (FAILED(Super::Render()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUILevelChange_Image::Ready_Components(LEVEL_CHANGE_DESC* pDesc)
{
	Super::Ready_Components(pDesc);
	return S_OK;
}

HRESULT CUILevelChange_Image::Bind_ShaderResources()
{
	CShader* pShader = Get_Component<CShader>();
	if (FAILED(Get_Component<CTransform>()->Bind_ShaderResource(pShader)))
		return E_FAIL;
	if (FAILED(Super::Bind_ShaderResources()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUILevelChange_Image::Attach_Personal_Info()
{
	switch (m_eDImageSubClass)
	{
	case DTO::EUIDImageSubClassType::LEVEL_CHAGE_1:
		break;
	case DTO::EUIDImageSubClassType::LEVEL_CHAGE_2:
		m_eNextLevelID = ELevelType::TUTORIAL_VILLAGE;
		m_wstrText = L"튜토리얼 빌리지";
		m_vFontColor = Vec4{ 0.0f, 222.0f / 255.0f, 165.0f / 255.0f, 1.f };
		break;
	case DTO::EUIDImageSubClassType::LEVEL_CHAGE_3:
		m_eNextLevelID = ELevelType::TUTORIAL_BOSS;
		m_wstrText = L"튜토리얼 보스";
		m_vFontColor = Vec4{ 0.78f, 0.28f, 0.90f, 1.f };
		break;
	case DTO::EUIDImageSubClassType::LEVEL_CHAGE_4:
		m_eNextLevelID = ELevelType::SQUARE;
		m_wstrText = L"광장";
		m_vFontColor = Vec4{ 0.20f, 0.65f, 1.00f, 1.f };
		break;
	case DTO::EUIDImageSubClassType::LEVEL_CHAGE_5:
		m_eNextLevelID = ELevelType::TEST;
		m_wstrText = L"테스트";
		m_vFontColor = Vec4{ 1.0f, 0.423f, 0.051f, 1.f };
		break;
	case DTO::EUIDImageSubClassType::LEVEL_CHAGE_6:
		m_eNextLevelID = ELevelType::TAVERN;
		m_wstrText = L"술집"; 
		m_vFontColor = Vec4{ 0.92f, 0.71f, 0.18f, 1.f };
		break;
	case DTO::EUIDImageSubClassType::LEVEL_CHAGE_7:
		m_eNextLevelID = ELevelType::KUANGKENG;
		m_wstrText = L"갱도";
		m_vFontColor = Vec4{ 0.45f, 0.85f, 0.52f, 1.f };
		break;
	case DTO::EUIDImageSubClassType::END:
	default:
		return E_FAIL;
	}
	return S_OK;
}

void CUILevelChange_Image::Trigger_By_InteractState()
{
	if (Engine_Utils::Has_Flag(m_iInteractState, EUIEvent_Flag::HOVER_ENTER))
	{
		m_isHover = true;
	}
	if (Engine_Utils::Has_Flag(m_iInteractState, EUIEvent_Flag::HOVER_EXIT))
	{
		m_isHover = false;
	}

	if (Engine_Utils::Has_Flag(m_iInteractState, EUIEvent_Flag::PRESS_EXIT))
	{
		if (m_eNextLevelID == ELevelType::END)
			return;
		m_pGameInstance->Request_ChangeLevel(ENUM_TO_UINT(ELevelType::LOADING), CLevel_Loading::Create(m_pDevice, m_pDeviceContext, m_eNextLevelID));
	}
}

void CUILevelChange_Image::Initialize_Visible_Event()
{
	m_isFin_Event = false;
	m_isActive = false;
}

_bool CUILevelChange_Image::Tick_Visible_Event(const _float fTimeDelta)
{
	return true;
}

CUILevelChange_Image* CUILevelChange_Image::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CUILevelChange_Image* pInstance = new CUILevelChange_Image(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CUILevelChange_Image::Create, Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUILevelChange_Image::Clone(void* pArg)
{
	CUILevelChange_Image* pInstance = new CUILevelChange_Image(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CUILevelChange_Image::Clone, Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUILevelChange_Image::Free()
{
	Super::Free();
}
