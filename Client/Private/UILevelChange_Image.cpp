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

HRESULT CUILevelChange_Image::Attach_Personal_Info()
{
	switch (m_eDImageSubClass)
	{
	case DTO::EUIDImageSubClassType::LEVEL_CHAGE_1:
		break;
	case DTO::EUIDImageSubClassType::LEVEL_CHAGE_2:
		m_eNextLevelID = ELevelType::TUTORIAL_VILLAGE;
		break;
	case DTO::EUIDImageSubClassType::LEVEL_CHAGE_3:
		m_eNextLevelID = ELevelType::TUTORIAL_BOSS;
		break;
	case DTO::EUIDImageSubClassType::LEVEL_CHAGE_4:
		m_eNextLevelID = ELevelType::SQUARE;
		break;
	case DTO::EUIDImageSubClassType::LEVEL_CHAGE_5:
		m_eNextLevelID = ELevelType::TEST;
		break;
	case DTO::EUIDImageSubClassType::END:
	default:
		return E_FAIL;
	}
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
	Acting_By_InteractState();
	Super::Update_Priority(fTimeDelta);
}

void CUILevelChange_Image::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
}

void CUILevelChange_Image::Update_Late(const _float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);
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

void CUILevelChange_Image::Acting_By_InteractState()
{
	if (Engine_Utils::Has_Flag(m_iInteractState, EUIEvent_Flag::PRESS_EXIT))
	{
		if (m_eNextLevelID == ELevelType::END)
			return;

		m_pGameInstance->Request_ChangeLevel(ENUM_TO_UINT(ELevelType::LOADING), CLevel_Loading::Create(m_pDevice, m_pDeviceContext, m_eNextLevelID));
		m_pUIManager->Request_Clear();
	}

}

void CUILevelChange_Image::OnUIEvent(ETriggerEventType eEvent, CGenericUI* pSender)
{
	if (!m_isActive)
		return;
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
