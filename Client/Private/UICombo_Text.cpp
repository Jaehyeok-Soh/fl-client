#include "pch.h"
#include "UICombo_Text.h"
#include "Client_Defines.h"
#include "Client_EventDefine.h"

//=================
// Component
//=================
#include "StatCom_Player.h"
#include "Texture.h"
#include "Shader.h"
#include "VIBuffer_Rect_Tex.h"
#include "GameInstance.h"

CUICombo_Text::CUICombo_Text(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	:CUIText(pDevice, pDeviceContext)
{
}

CUICombo_Text::CUICombo_Text(const CUICombo_Text& rhs)
	:CUIText(rhs)
{
}

HRESULT CUICombo_Text::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUICombo_Text::Initialize(void* pArg)
{
	COMBO_TEXT_DESC* pDesc = static_cast<COMBO_TEXT_DESC*>(pArg);
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;
	if (FAILED(Ready_Components(pDesc)))
		return E_FAIL;
	return S_OK;
}

HRESULT CUICombo_Text::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;

	if (FAILED(Attach_Personal_Info()))
		return E_FAIL;

	return S_OK;
}


void CUICombo_Text::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CUICombo_Text::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
	if (KEY_BUTTON_HOLD(DIK_UP))
	{
		m_iCurComboCount += 1;
	}
	if (KEY_BUTTON_HOLD(DIK_DOWN))
	{
		m_iCurComboCount -= 1;
	}
}

void CUICombo_Text::Update_Late(const _float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);
}


void CUICombo_Text::Ready_Before_Render(const _float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);
	
	Tick_By_Type(fTimeDelta);

	if (FAILED(Convert_Stat_To_Text()))
		return;
}

HRESULT CUICombo_Text::Render()
{
	if (!m_isVisible)
		return S_OK;
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;
	if (FAILED(Super::Render()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUICombo_Text::Ready_Components(COMBO_TEXT_DESC* pDesc)
{
	if (FAILED(Super::Ready_Components(pDesc)))
		return E_FAIL;
	return S_OK;
}

HRESULT CUICombo_Text::Bind_ShaderResources()
{
	CShader* pShader = Get_Component<CShader>();
	if (FAILED(Get_Component<CTransform>()->Bind_ShaderResource(pShader)))
		return E_FAIL;
	if (FAILED(Super::Bind_ShaderResources()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUICombo_Text::Attach_Personal_Info()
{
	CGameObject* pResult = m_pGameInstance->Get_GameObject_Front(ENUM_TO_UINT(ELevelType::STATIC), g_wszPlayerLayer);
	if (nullptr == pResult)
		return E_FAIL;

	m_pPlayerStatCom = static_cast<CStatCom_Player*>(pResult->Get_Component<CMyStat>());
	if (nullptr == m_pPlayerStatCom)
		return E_FAIL;

	switch (m_eTextSubClassType)
	{
	case DTO::EUITextSubClassType::BATTLE_COMBO_TEXT_BEGIN:
		break;
	case DTO::EUITextSubClassType::BATTLE_COMBO_COMBO_TEXT:
		break;
	case DTO::EUITextSubClassType::BATTLE_COMBO_CUR_COUNT_TEXT:
		break;
	case DTO::EUITextSubClassType::BATTLE_COMBO_NEXT_COUNT_TEXT:
		break;
	case DTO::EUITextSubClassType::BATTLE_COMBO_TEXT_END:
		break;
	case DTO::EUITextSubClassType::END:
	default:
		break;
	}

	m_pGameInstance->Subscribe<COMBO_ATTACK_EVENT_START>([this]()
		{
			if (!this->m_isVisible)
				this->Set_Visible();
		});

	m_pGameInstance->Subscribe<COMBO_ATTACK_EVENT_END>([this]()
		{
			this->Set_Invisible();
		});

	return S_OK;
}

void CUICombo_Text::Tick_By_Type(const _float fTimeDelta)
{
	m_isCountChange = false;

	if (m_iPreComboCount != m_iCurComboCount)
		m_isCountChange = true;

	switch (m_eTextSubClassType)
	{
	case DTO::EUITextSubClassType::BATTLE_COMBO_TEXT_BEGIN:
		break;
	case DTO::EUITextSubClassType::BATTLE_COMBO_COMBO_TEXT:
		break;
	case DTO::EUITextSubClassType::BATTLE_COMBO_CUR_COUNT_TEXT:
	{
		// m_iCurComboCount = m_pPlayerStatCom->Get_Count(CStatCom_Player::TIMER_TYPE::COMBO);

		if (m_isCountChange)
		{
			Ready_LerpChange(0.3f, 3.f, 1.f, 1.f, 0.f);
			Ready_Fade_Text(0.3f, 0.f, 1.f, 0.f);
		}

		Tick_LerpChange(&m_fScaleOffset	, fTimeDelta);
		Tick_Fade_Text(fTimeDelta);
	}
	break;
	case DTO::EUITextSubClassType::BATTLE_COMBO_NEXT_COUNT_TEXT:
		// m_iCurComboCount = m_pPlayerStatCom->Get_Count(CStatCom_Player::TIMER_TYPE::COMBO);
		Convert_Count_To_Rank();
		break;
	case DTO::EUITextSubClassType::BATTLE_COMBO_TEXT_END:
		break;
	case DTO::EUITextSubClassType::END:
	default:
		break;
	}

	m_iPreComboCount = m_iCurComboCount;
}

HRESULT CUICombo_Text::Convert_Stat_To_Text()
{
	switch (m_eTextSubClassType)
	{
	case DTO::EUITextSubClassType::BATTLE_COMBO_TEXT_BEGIN:
		break;
	case DTO::EUITextSubClassType::BATTLE_COMBO_COMBO_TEXT:
		break;
	case DTO::EUITextSubClassType::BATTLE_COMBO_CUR_COUNT_TEXT:
		m_wstrText = std::to_wstring(m_iCurComboCount);
		break;
	case DTO::EUITextSubClassType::BATTLE_COMBO_NEXT_COUNT_TEXT:
	{
		switch (m_eCurComboType)
		{
		case Client::ECombotype::C:
			m_wstrText = L"/40";
			break;
		case Client::ECombotype::B:
			m_wstrText = L"/70";
			break;
		case Client::ECombotype::A:
			m_wstrText = L"/120";
			break;
		case Client::ECombotype::S:
			m_wstrText = L"/120";
			break;
		case Client::ECombotype::END:
			break;
		default:
			break;
		}
	}
	break;
	case DTO::EUITextSubClassType::BATTLE_COMBO_TEXT_END:
		break;
	case DTO::EUITextSubClassType::END:
	default:
		break;
	}
	return S_OK;
}

void CUICombo_Text::OnUIEvent(ETriggerEventType eEvent, CGenericUI* pSender)
{
	if (!m_isActive)
		return;
}

void CUICombo_Text::Initialize_Visible_Event()
{
	m_isActive = false;
	m_isFin_Event = false;
}

void CUICombo_Text::Initialize_InVisible_Event()
{
}

_bool CUICombo_Text::Tick_Visible_Event(const _float fTimeDelta)
{
	m_isActive = true;
	m_isFin_Event = true;
	return true;
}

_bool CUICombo_Text::Tick_InVisible_Event(const _float fTimeDelta)
{
	return true;
}

void CUICombo_Text::Convert_Count_To_Rank()
{
	if (m_iCurComboCount < 40 || m_iCurComboCount >= 0)
	{
		m_eCurComboType = ECombotype::C;
	}
	else if (m_iCurComboCount < 70 || m_iCurComboCount >= 40)
	{
		m_eCurComboType = ECombotype::B;
	}
	else if (m_iCurComboCount < 120 || m_iCurComboCount >= 70)
	{
		m_eCurComboType = ECombotype::A;
	}
	else if (m_iCurComboCount == 120)
	{
		m_eCurComboType = ECombotype::S;
	}
}

CUICombo_Text* CUICombo_Text::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CUICombo_Text* pInstance = new CUICombo_Text(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CUICombo_Text::Create, Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUICombo_Text::Clone(void* pArg)
{
	CUICombo_Text* pInstance = new CUICombo_Text(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CUICombo_Text::Clone, Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUICombo_Text::Free()
{
	Super::Free();
}
