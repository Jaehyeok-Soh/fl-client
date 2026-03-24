#include "pch.h"
#include "UIAimDot_Image.h"
#include "Client_Defines.h"
#include "Client_EventDefine.h"
//=================
// Component
//=================
#include "MainPlayer.h"
#include "Gun.h"
#include "StatCom_Player.h"
#include "Texture.h"
#include "Shader.h"
#include "VIBuffer_Rect_Tex.h"
#include "UI_Manager.h"
#include "GameInstance.h"

CUIAimDot_Image::CUIAimDot_Image(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	:CUIDynamic_Image(pDevice, pDeviceContext)
{
}

CUIAimDot_Image::CUIAimDot_Image(const CUIAimDot_Image& rhs)
	:CUIDynamic_Image(rhs)
{
}

HRESULT CUIAimDot_Image::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIAimDot_Image::Initialize(void* pArg)
{
	AIMDOT_IMAGE_DESC* pDesc = static_cast<AIMDOT_IMAGE_DESC*>(pArg);
	m_isInteract = true;
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;
	if (FAILED(Ready_Components(pDesc)))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIAimDot_Image::Attach_Personal_Info()
{
	_bool isRangeAtt = { false };
	_bool isMeeleAtt = { false };
	if (Engine_Utils::Has_Flag(m_pPlayerStatCom->Get_AttState(), CStatCom_Player::Attack_State::Gun))
		isRangeAtt = true;
	else if (Engine_Utils::Has_Flag(m_pPlayerStatCom->Get_AttState(), CStatCom_Player::Attack_State::Melee))
		isMeeleAtt = true;

	_bool isHitScan = m_pGunParts->Get_OnTarget();

	_bool isHit = m_pGunParts;

	switch (m_eDImageSubClass)
	{
	case DTO::EUIDImageSubClassType::BATTLE_UI_BEGIN:
	{
		// 지금 총 들고 있는데
		if (Engine_Utils::Has_Flag(m_pPlayerStatCom->Get_AttState(), CStatCom_Player::Attack_State::Gun))
		{
			// 이전 프레임엔 총 안 들고 있었는데?
			if (!m_isPreRangeAtt)
			{
				Set_Visible();
			}
		}
		// 지금 칼 들고 있는데, 혹은 아무것도 안 들고 있는데
		else if (Engine_Utils::Has_Flag(m_pPlayerStatCom->Get_AttState(), CStatCom_Player::Attack_State::Melee))
		{
			// 이전 프레임엔 총 들고 있었는데?
			if (m_isPreRangeAtt)
			{
				Set_Invisible();
			}
		}
	}
		break;
	case DTO::EUIDImageSubClassType::BATTLE_AIMDOT_COMMON:
	{
		// 원거리 공격 중일 때
		if (isRangeAtt)
		{
			// 플레이어 에임이 적에 맞았는지
			if (isHitScan)
			{
				m_vColorTint			= Vec4{ 1.f, 0.f, 0.f, 1.f };
				m_vGradiantColorTint	= Vec4{ 1.f, 0.f, 0.f, 1.f };

			}
			else
			{
				m_vColorTint			= Vec4{ 1.f, 1.f, 1.f, 1.f };
				m_vGradiantColorTint	= Vec4{ 1.f, 1.f, 1.f, 1.f };
			}
		}
	}
	break;
	case DTO::EUIDImageSubClassType::BATTLE_AIMDOT_CROSSHAIR_RIGHT:
	case DTO::EUIDImageSubClassType::BATTLE_AIMDOT_CROSSHAIR_BOTTOM:
	case DTO::EUIDImageSubClassType::BATTLE_AIMDOT_CROSSHAIR_LEFT:
	case DTO::EUIDImageSubClassType::BATTLE_AIMDOT_CROSSHAIR_TOP:
	{
		// 지금 총 들고 있는데
		if (Engine_Utils::Has_Flag(m_pPlayerStatCom->Get_AttState(), CStatCom_Player::Attack_State::Gun))
		{
			// 이전 프레임엔 총 안 들고 있었는데?
			if (!m_isPreRangeAtt)
			{
				Set_Visible();
			}
			if (m_isShootingTrigger)
			{
				Ready_Lerp_Movement(m_vMoveOffset, m_vMaxOffset, m_fAttSpeed, 1.f, m_fDelay);
				m_isSpreadStart = true;
				m_isSpreadEnd = false;
			}

			if (isHitScan)
			{
				m_vColorTint			= Vec4{ 1.f, 1.f, 1.f, 1.f };
				m_vGradiantColorTint	= Vec4{ 1.f, 0.f, 0.f, 1.f };

			}
			else
			{
				m_vColorTint			= Vec4{ 1.f, 1.f, 1.f, 1.f };
				m_vGradiantColorTint	= Vec4{ 1.f, 1.f, 1.f, 1.f };
			}
		}
		// 지금 칼 들고 있는데, 혹은 아무것도 안 들고 있는데
		else if (Engine_Utils::Has_Flag(m_pPlayerStatCom->Get_AttState(), CStatCom_Player::Attack_State::Melee))
		{
			// 이전 프레임엔 총 들고 있었는데?
			if (m_isPreRangeAtt)
			{
				Set_Invisible();
			}
		}
	}
	break;
	case DTO::EUIDImageSubClassType::BATTLE_AIM_HIT:

		break;
	case DTO::EUIDImageSubClassType::BATTLE_AIM_LOCK:
		break;
	case DTO::EUIDImageSubClassType::BATTLE_UI_END:
		break;
	case DTO::EUIDImageSubClassType::END:
	default:
		return E_FAIL;
	}

	m_isPreRangeAtt = isRangeAtt;
	m_isPreMeeleAtt = isMeeleAtt;
	return S_OK;
}

void CUIAimDot_Image::Bind_Events()
{
	m_vecEventHandles.push_back(
		m_pUIManager->Get_UIEvents().Subscribe([this](const UIEVENT_DESC& Desc)
			{
				if (EUIEventID::MENU_OPEN == Desc.eEventID)
				{
					this->Set_Invisible();
				}
			}));

	// 대화 Event
	m_vecEventHandles.push_back(
		m_pGameInstance->Subscribe<DIALOGUE_BEGIN>([this](_int iId)
			{
				this->m_fAlpha_Ratio = 0.f;
				Set_Active(false);
			}));
	m_vecEventHandles.push_back(
		m_pGameInstance->Subscribe<DIALOGUE_END>([this]()
			{
				this->m_fAlpha_Ratio = 1.f;
				Set_Active(true);
			}));

	// 패널 Events
	m_vecEventHandles.push_back(
		m_pUIManager->Get_UIEvents().Subscribe([this](const UIEVENT_DESC& Desc)
			{
				if (EUIEventID::TUTORIAL_PANNEL_START == Desc.eEventID)
				{
					this->m_fAlpha_Ratio = 0.f;
					Set_Active(false);
				}
			}));
	m_vecEventHandles.push_back(
		m_pUIManager->Get_UIEvents().Subscribe([this](const UIEVENT_DESC& Desc)
			{
				if (EUIEventID::TUTORIAL_PANNEL_END == Desc.eEventID)
				{
					this->m_fAlpha_Ratio = 1.f;
					Set_Active(true);
				}
			}));

	switch (m_eDImageSubClass)
	{
	case DTO::EUIDImageSubClassType::BATTLE_UI_BEGIN:
		m_vecEventHandles.push_back(m_pGameInstance->Subscribe<CINEMATIC_START>([this]() { this->Set_Invisible(); }));
		break;
	case DTO::EUIDImageSubClassType::BATTLE_AIMDOT_COMMON:
		m_vecEventHandles.push_back(
			m_pUIManager->Get_UIEvents().Subscribe([this](const UIEVENT_DESC& Desc)
				{
					if (EUIEventID::MENU_CLOSE == Desc.eEventID)
					{
						this->Set_Visible();
					}
				}));
		m_vecEventHandles.push_back(m_pGameInstance->Subscribe<CINEMATIC_START>([this]() { this->Set_Invisible(); }));
		m_vecEventHandles.push_back(m_pGameInstance->Subscribe<CINEMATIC_END>([this]() { this->Set_Visible(); }));
		break;
	case DTO::EUIDImageSubClassType::BATTLE_AIMDOT_CROSSHAIR_TOP:
		m_vecEventHandles.push_back(m_pGameInstance->Subscribe<CINEMATIC_START>([this]() { this->Set_Invisible(); }));
		break;
	case DTO::EUIDImageSubClassType::BATTLE_AIMDOT_CROSSHAIR_RIGHT:
		m_vecEventHandles.push_back(m_pGameInstance->Subscribe<CINEMATIC_START>([this]() { this->Set_Invisible(); }));
		break;
	case DTO::EUIDImageSubClassType::BATTLE_AIMDOT_CROSSHAIR_BOTTOM:
		m_vecEventHandles.push_back(m_pGameInstance->Subscribe<CINEMATIC_START>([this]() { this->Set_Invisible(); }));
		break;
	case DTO::EUIDImageSubClassType::BATTLE_AIMDOT_CROSSHAIR_LEFT:
		m_vecEventHandles.push_back(m_pGameInstance->Subscribe<CINEMATIC_START>([this]() { this->Set_Invisible(); }));
		break;
	case DTO::EUIDImageSubClassType::BATTLE_AIM_HIT:
		m_vecEventHandles.push_back(m_pGameInstance->Subscribe<CINEMATIC_START>([this]() { this->Set_Invisible(); }));
		m_vecEventHandles.push_back(m_pGameInstance->Subscribe< GUN_ON_HIT>([this]() { this->Set_Visible(); }));
		break;
	case DTO::EUIDImageSubClassType::BATTLE_AIM_LOCK:
		break;
	case DTO::EUIDImageSubClassType::BATTLE_UI_END:
		break;
	case DTO::EUIDImageSubClassType::END:
	default:
		break;
	}

}

HRESULT CUIAimDot_Image::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;

	CGameObject* pResult = m_pGameInstance->Get_GameObject_Front(ENUM_TO_UINT(ELevelType::STATIC), g_wszPlayerLayer);
	if (nullptr == pResult)
		return E_FAIL;

	m_pPlayerStatCom = static_cast<CStatCom_Player*>(pResult->Get_Component<CMyStat>());
	if (nullptr == m_pPlayerStatCom)
		return E_FAIL;

	auto* pPlayer = static_cast<CMainPlayer*>(pResult);
	if (nullptr == pPlayer)
		return E_FAIL;

	m_pGunParts = pPlayer->Get_Part<CGun>(ENUM_TO_UINT(CPlayer::Part::GUN));
	if (nullptr == m_pGunParts)
		return E_FAIL;

	switch (m_eDImageSubClass)
	{
	case DTO::EUIDImageSubClassType::BATTLE_UI_BEGIN:
		break;
	case DTO::EUIDImageSubClassType::BATTLE_AIMDOT_COMMON:
		break;
	case DTO::EUIDImageSubClassType::BATTLE_AIMDOT_CROSSHAIR_TOP:
		m_vMaxOffset = Vec2{ 0.f, -10.f };
		Set_Invisible();
		break;
	case DTO::EUIDImageSubClassType::BATTLE_AIMDOT_CROSSHAIR_RIGHT:
		m_vMaxOffset = Vec2{ 10.f, 0.f };
		Set_Invisible();
		break;
	case DTO::EUIDImageSubClassType::BATTLE_AIMDOT_CROSSHAIR_BOTTOM:
		m_vMaxOffset = Vec2{ 0.f, 10.f };
		Set_Invisible();
		break;
	case DTO::EUIDImageSubClassType::BATTLE_AIMDOT_CROSSHAIR_LEFT:
		m_vMaxOffset = Vec2{ -10.f, 0.f };
		Set_Invisible();
		break;
	case DTO::EUIDImageSubClassType::BATTLE_AIM_HIT:
		break;
	case DTO::EUIDImageSubClassType::BATTLE_AIM_LOCK:
		break;
	case DTO::EUIDImageSubClassType::BATTLE_UI_END:
		break;
	case DTO::EUIDImageSubClassType::END:
	default:
		return E_FAIL;
	}

	return S_OK;
}

void CUIAimDot_Image::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CUIAimDot_Image::Update(const _float fTimeDelta)
{
	m_isShootingTrigger = false;
	if(m_pGunParts->Get_isFire())
		m_isShootingTrigger = true;

	Super::Update(fTimeDelta);
}

void CUIAimDot_Image::Update_Late(const _float fTimeDelta)
{
	if (FAILED(Attach_Personal_Info()))
		return ;

	_bool is = Tick_Lerp_Movement(fTimeDelta);

	// 올라가는 중일때 
	if (m_isSpreadStart)
	{
		if (is)
		{
			m_isSpreadStart = false;
			m_isSpreadEnd = true;
			Ready_Lerp_Movement(m_vMaxOffset, Vec2{ 0.f, 0.f }, m_fAttSpeed, 1.f, m_fDelay);
		}
	}

	Super::Update_Late(fTimeDelta);
}

void CUIAimDot_Image::Ready_Before_Render(const _float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);
}

HRESULT CUIAimDot_Image::Render()
{
	if (!m_isVisible)
		return S_OK;
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;
	if (FAILED(Super::Render()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIAimDot_Image::Ready_Components(AIMDOT_IMAGE_DESC* pDesc)
{
	Super::Ready_Components(pDesc);
	return S_OK;
}

HRESULT CUIAimDot_Image::Bind_ShaderResources()
{
	CShader* pShader = Get_Component<CShader>();
	if (FAILED(Get_Component<CTransform>()->Bind_ShaderResource(pShader)))
		return E_FAIL;
	if (FAILED(Super::Bind_ShaderResources()))
		return E_FAIL;
	return S_OK;
}

void CUIAimDot_Image::Initialize_Visible_Event()
{
	m_isFin_Event = false;
	m_isActive = false;

	switch (m_eDImageSubClass)
	{
	case DTO::EUIDImageSubClassType::BATTLE_AIMDOT_COMMON:
		break;
	case DTO::EUIDImageSubClassType::BATTLE_AIMDOT_CROSSHAIR_TOP:
		break;
	case DTO::EUIDImageSubClassType::BATTLE_AIMDOT_CROSSHAIR_RIGHT:
		break;
	case DTO::EUIDImageSubClassType::BATTLE_AIMDOT_CROSSHAIR_BOTTOM:
		break;
	case DTO::EUIDImageSubClassType::BATTLE_AIMDOT_CROSSHAIR_LEFT:
		break;
	case DTO::EUIDImageSubClassType::BATTLE_AIM_HIT:
		Ready_LerpChange(0.15f, 0.5f, 1.5f, 1.f, 0.f);
		break;
	case DTO::EUIDImageSubClassType::BATTLE_AIM_LOCK:
		break;
	}
}

void CUIAimDot_Image::Initialize_InVisible_Event()
{
}

_bool CUIAimDot_Image::Tick_Visible_Event(const _float fTimeDelta)
{
	switch (m_eDImageSubClass)
	{
	case DTO::EUIDImageSubClassType::BATTLE_AIMDOT_COMMON:
		break;
	case DTO::EUIDImageSubClassType::BATTLE_AIMDOT_CROSSHAIR_TOP:
		break;
	case DTO::EUIDImageSubClassType::BATTLE_AIMDOT_CROSSHAIR_RIGHT:
		break;
	case DTO::EUIDImageSubClassType::BATTLE_AIMDOT_CROSSHAIR_BOTTOM:
		break;
	case DTO::EUIDImageSubClassType::BATTLE_AIMDOT_CROSSHAIR_LEFT:
		break;
	case DTO::EUIDImageSubClassType::BATTLE_AIM_HIT:
	{
		_bool is = Tick_LerpChange(&m_fScale, fTimeDelta);
		if (is)
		{
			Set_Invisible();
			return true;
		}
		return false;
	}
	break;
	case DTO::EUIDImageSubClassType::BATTLE_AIM_LOCK:
		break;
	}
	return true;
}

_bool CUIAimDot_Image::Tick_InVisible_Event(const _float fTimeDelta)
{
	return true;
}

CUIAimDot_Image* CUIAimDot_Image::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CUIAimDot_Image* pInstance = new CUIAimDot_Image(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CUIAimDot_Image::Create, Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUIAimDot_Image::Clone(void* pArg)
{
	CUIAimDot_Image* pInstance = new CUIAimDot_Image(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CUIAimDot_Image::Clone, Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUIAimDot_Image::Free()
{
	Super::Free();
}
