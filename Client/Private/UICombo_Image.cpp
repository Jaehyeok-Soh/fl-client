#include "pch.h"
#include "UICombo_Image.h"
#include "Client_Defines.h"
#include "Client_EventDefine.h"
//=================
// Component
//=================
#include "MainPlayer.h"
#include "StatCom_Player.h"
#include "Texture.h"
#include "Shader.h"
#include "VIBuffer_Rect_Tex.h"
#include "GameInstance.h"

#define C_RANK_IMAGE_TAG L"Texture_T_Map_Rank_C"
#define B_RANK_IMAGE_TAG L"Texture_T_Map_Rank_B"
#define A_RANK_IMAGE_TAG L"Texture_T_Map_Rank_A"
#define S_RANK_IMAGE_TAG L"Texture_T_Map_Rank_S"

#define C_RANK_GLOW_COLOR Vec4{0.f, 1.f, 0.f, 1.f}
#define B_RANK_GLOW_COLOR Vec4{0.f, 0.f, 1.f, 1.f}
#define A_RANK_GLOW_COLOR Vec4{1.f, 0.f, 1.f, 1.f}
#define S_RANK_GLOW_COLOR Vec4{1.f, 1.f, 0.f, 1.f}

CUICombo_Image::CUICombo_Image(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	:CUIDynamic_Image(pDevice, pDeviceContext)
{
}

CUICombo_Image::CUICombo_Image(const CUICombo_Image& rhs)
	:CUIDynamic_Image(rhs)
{
}

HRESULT CUICombo_Image::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUICombo_Image::Initialize(void* pArg)
{
	COMBO_IMAGE_DESC* pDesc = static_cast<COMBO_IMAGE_DESC*>(pArg);
	m_isInteract = true;
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;
	if (FAILED(Ready_Components(pDesc)))
		return E_FAIL;
	return S_OK;
}


HRESULT CUICombo_Image::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;

	if (FAILED(Attach_Personal_Info()))
		return E_FAIL;

	return S_OK;
}

void CUICombo_Image::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CUICombo_Image::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
}

void CUICombo_Image::Update_Late(const _float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);
	Tick_By_Type(fTimeDelta);
}

void CUICombo_Image::Ready_Before_Render(const _float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);
}

HRESULT CUICombo_Image::Render()
{
	if (!m_isVisible)
		return S_OK;
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;
	if (FAILED(Super::Render()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUICombo_Image::Ready_Components(COMBO_IMAGE_DESC* pDesc)
{
	if (FAILED(Super::Ready_Components(pDesc)))
		return E_FAIL;
	return S_OK;
}

HRESULT CUICombo_Image::Bind_ShaderResources()
{
	CShader* pShader = Get_Component<CShader>();
	if (FAILED(Get_Component<CTransform>()->Bind_ShaderResource(pShader)))
		return E_FAIL;

	if (m_eDImageSubClass == DTO::EUIDImageSubClassType::BATTLE_COMBO_BG_GLOW)
	{
		if (FAILED(pShader->Get_Variable("g_vNoiseUVScale")->SetRawValue(&m_vNoiseUVScale, 0, sizeof(Vec2))))
			return E_FAIL;
		if (FAILED(pShader->Get_Variable("g_vNoiseUVScroll")->SetRawValue(&m_vNoiseUVScroll, 0, sizeof(Vec2))))
			return E_FAIL;
		if (FAILED(pShader->Get_Variable("g_fTime")->SetRawValue(&m_fTime, 0, sizeof(_float))))
			return E_FAIL;
		if (FAILED(pShader->Get_Variable("g_fGlowDistort")->SetRawValue(&m_fGlowDistort, 0, sizeof(_float))))
			return E_FAIL;
		if (FAILED(pShader->Get_Variable("g_fGlowPulseSpeed")->SetRawValue(&m_fGlowPulseSpeed, 0, sizeof(_float))))
			return E_FAIL;
		if (FAILED(pShader->Get_Variable("g_fGlowIntensity")->SetRawValue(&m_fGlowIntensity, 0, sizeof(_float))))
			return E_FAIL;
	}

	if (FAILED(Super::Bind_ShaderResources()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUICombo_Image::Attach_Personal_Info()
{
	CGameObject* pResult = m_pGameInstance->Get_GameObject_Front(ENUM_TO_UINT(ELevelType::STATIC), g_wszPlayerLayer);
	if (nullptr == pResult)
		return E_FAIL;

	m_pPlayerStatCom = static_cast<CStatCom_Player*>(pResult->Get_Component<CMyStat>());
	if (nullptr == m_pPlayerStatCom)
		return E_FAIL;

	switch (m_eDImageSubClass)
	{
	case DTO::EUIDImageSubClassType::BATTLE_COMBO_BEGIN:
		break;
	case DTO::EUIDImageSubClassType::BATTLE_COMBO_RANK:
		break;
	case DTO::EUIDImageSubClassType::BATTLE_COMBO_BG:
		m_fProgress_Ratio	= 0.f;
		break;
	case DTO::EUIDImageSubClassType::BATTLE_COMBO_BG_GLOW:
		m_vNoiseUVScale		= Vec2{ 1.f, 1.f };
		m_vNoiseUVScroll	= Vec2{ 0.f, 1.f};
		m_fGlowDistort		= 0.05f;
		m_fGlowPulseSpeed	= 1.f;
		m_fGlowIntensity	= 1.f;
		m_vColorTint		= Vec4{ 0.f, 1.f, 0.f, 1.f };
		m_vGradiantColorTint = Vec4{ 0.f, 1.f, 0.f, 1.f };
		break;
	case DTO::EUIDImageSubClassType::BATTLE_COMBO_END:
		break;
	case DTO::EUIDImageSubClassType::END:
	default:
		return E_FAIL;
	}


	return S_OK;
}

void CUICombo_Image::Tick_By_Type(const _float fTimeDelta)
{
	switch (m_eDImageSubClass)
	{
	case DTO::EUIDImageSubClassType::BATTLE_COMBO_BEGIN:
		break;
	case DTO::EUIDImageSubClassType::BATTLE_COMBO_RANK:
	{
		if (m_pPlayerStatCom->Get_Count(CStatCom_Player::TIMER_TYPE::COMBO) == 0)
		{
			m_pGameInstance->Broadcast<COMBO_ATTACK_EVENT_END>();
		}

		_uint i = m_pPlayerStatCom->Get_Count(CStatCom_Player::TIMER_TYPE::COMBO);

		if (m_iPreComboCount != i)
			m_isComboChange = true;

		m_iCurComboCount = i;

		Convert_Count_To_Rank();
		if (m_isComboChange)
		{
			switch (m_eCurComboType)
			{
			case Client::ECombotype::C:
				if (FAILED(Get_Component<CTexture>()->Add_DefaultTexture(C_RANK_IMAGE_TAG, ENUM_TO_UINT(EUITextureSlot::DEFAULT))))
					break;
				break;
			case Client::ECombotype::B:
				if (FAILED(Get_Component<CTexture>()->Add_DefaultTexture(B_RANK_IMAGE_TAG, ENUM_TO_UINT(EUITextureSlot::DEFAULT))))
					break;
				break;
			case Client::ECombotype::A:
				if (FAILED(Get_Component<CTexture>()->Add_DefaultTexture(A_RANK_IMAGE_TAG, ENUM_TO_UINT(EUITextureSlot::DEFAULT))))
					break;
				break;
			case Client::ECombotype::S:
				if (FAILED(Get_Component<CTexture>()->Add_DefaultTexture(S_RANK_IMAGE_TAG, ENUM_TO_UINT(EUITextureSlot::DEFAULT))))
					break;
				break;
			case Client::ECombotype::END:
			default:
				break;

				m_isComboChange = false;
			}
			Ready_LerpChange(0.3f, 3.f, 1.f, 1.f, m_fDelay);
		}
		Tick_LerpChange(&m_fScale, fTimeDelta);
	}
	break;
	case DTO::EUIDImageSubClassType::BATTLE_COMBO_BG:
		break;
	case DTO::EUIDImageSubClassType::BATTLE_COMBO_BG_GLOW:
	{
		m_fTime += fTimeDelta;

		_uint i = m_pPlayerStatCom->Get_Count(CStatCom_Player::TIMER_TYPE::COMBO);

		if (m_iPreComboCount != i)
			m_isComboChange = true;

		m_iCurComboCount = i;

		Convert_Count_To_Rank();
		if (m_isComboChange)
		{
			switch (m_eCurComboType)
			{
			case Client::ECombotype::C:
				m_vColorTint			= C_RANK_GLOW_COLOR;
				m_vGradiantColorTint	= C_RANK_GLOW_COLOR;
				break;
			case Client::ECombotype::B:
				m_vColorTint			= B_RANK_GLOW_COLOR;
				m_vGradiantColorTint	= B_RANK_GLOW_COLOR;
				break;
			case Client::ECombotype::A:
				m_vColorTint			= A_RANK_GLOW_COLOR;
				m_vGradiantColorTint	= A_RANK_GLOW_COLOR;
				break;
			case Client::ECombotype::S:
				m_vColorTint			= S_RANK_GLOW_COLOR;
				m_vGradiantColorTint	= S_RANK_GLOW_COLOR;
				break;
			case Client::ECombotype::END:
			default:
				break;
			}
		}
	}
	break;
	case DTO::EUIDImageSubClassType::BATTLE_COMBO_END:
		break;
	case DTO::EUIDImageSubClassType::END:
	default:
		break;
	}
	m_iPreComboCount = m_iCurComboCount;
}

void CUICombo_Image::Bind_Events()
{
	m_vecEventHandles.push_back(
		m_pGameInstance->Subscribe<COMBO_ATTACK_EVENT_START>([this]()
			{
				this->Set_Visible();
			}));

	m_vecEventHandles.push_back(
		m_pGameInstance->Subscribe<COMBO_ATTACK_EVENT_END>([this]()
			{
				this->Set_Invisible();
			}));
}

void CUICombo_Image::Initialize_Visible_Event()
{
	m_isFin_Event = false;
	m_isActive = false;
}

_bool CUICombo_Image::Tick_Visible_Event(const _float fTimeDelta)
{
	return true;
}

void CUICombo_Image::Initialize_InVisible_Event()
{
	m_iCurComboCount = 0;
	m_eCurComboType = ECombotype::END;
}

_bool CUICombo_Image::Tick_InVisible_Event(const _float fTimeDelta)
{
	return true;
}

void CUICombo_Image::Convert_Count_To_Rank()
{
	m_isComboChange = false;

	if (m_iCurComboCount < 40 && m_iCurComboCount >= 0)
	{
		m_eCurComboType = ECombotype::C;
	}
	else if (m_iCurComboCount < 70 && m_iCurComboCount >= 40)
	{
		m_eCurComboType = ECombotype::B;
	}
	else if (m_iCurComboCount < 120 && m_iCurComboCount >= 70)
	{
		m_eCurComboType = ECombotype::A;
	}
	else if (m_iCurComboCount >= 120)
	{
		m_eCurComboType = ECombotype::S;
	}

	if (m_eCurComboType != m_ePreComboType)
		m_isComboChange = true;

	m_ePreComboType = m_eCurComboType;
}

CUICombo_Image* CUICombo_Image::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CUICombo_Image* pInstance = new CUICombo_Image(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CUICombo_Image::Create, Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUICombo_Image::Clone(void* pArg)
{
	CUICombo_Image* pInstance = new CUICombo_Image(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CUICombo_Image::Clone, Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUICombo_Image::Free()
{
	Super::Free();
}
