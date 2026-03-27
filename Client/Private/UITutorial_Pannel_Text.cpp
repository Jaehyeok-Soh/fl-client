#include "pch.h"
#include "UITutorial_Pannel_Text.h"
#include "Client_Defines.h"
#include "Client_EventDefine.h"
#include "Canvas.h"
//=================
// Component
//=================
#include "WorldUI_Component.h"
#include "MyStat.h"
#include "Texture.h"
#include "Shader.h"
#include "VIBuffer_Rect_Tex.h"
#include "GameInstance.h"
#include <UI_Manager.h>

#define CUR_PAGE 0
#define MAX_PAGE 1

#define PANNEL_TEXTURE_TAG_1	L"캐릭터가 공격받을 시 기본적으로 하단의 쉴드가 우선적으로 소모되며, 쉴드가 소진되면, HP가 소모됩니다."
#define PANNEL_TEXTURE_TAG_2	L"원거리 무기로 멀리 있는 적과 높은 곳을 처치할 수 있습니다. 원거리 무기는 장전할 수 있는 탄환 수와 탄약에 제한이 있습니다."
#define PANNEL_TEXTURE_TAG_2_1	L"적을 처치하거나, 수납함을 열거나, 주변의 물체를 파괴하는 등의 방법으로 원거리 무기의 탄약을 보충할 수 있는 탄약을 획득할 수 있습니다."
#define PANNEL_TEXTURE_TAG_3	L"Texture_T_Guide_Img_Toughness"
#define PANNEL_TEXTURE_TAG_3_1	L"Texture_T_Guide_Img_DestructablePart_04"
#define PANNEL_TEXTURE_TAG_4	L"Texture_T_Guide_Img_Secondskill"

CUITutorial_Pannel_Text::CUITutorial_Pannel_Text(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	:CUIText(pDevice, pDeviceContext)
{
}

CUITutorial_Pannel_Text::CUITutorial_Pannel_Text(const CUITutorial_Pannel_Text& rhs)
	:CUIText(rhs)
{
}

HRESULT CUITutorial_Pannel_Text::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUITutorial_Pannel_Text::Initialize(void* pArg)
{
	TUTORIAL_PANNEL_TEXT_DESC* pDesc = static_cast<TUTORIAL_PANNEL_TEXT_DESC*>(pArg);
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;
	if (FAILED(Ready_Components(pDesc)))
		return E_FAIL;

	m_vOriginFontColor = m_vFontColor;
	return S_OK;
}

HRESULT CUITutorial_Pannel_Text::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;

	if (FAILED(Attach_Personal_Info()))
		return E_FAIL;

	return S_OK;
}


void CUITutorial_Pannel_Text::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CUITutorial_Pannel_Text::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);

	Tick_By_Type(fTimeDelta);
}

void CUITutorial_Pannel_Text::Update_Late(const _float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);
}

void CUITutorial_Pannel_Text::Ready_Before_Render(const _float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);
	if (FAILED(Convert_Stat_To_Text()))
		return;
}

HRESULT CUITutorial_Pannel_Text::Render()
{
	if (!m_isVisible)
		return S_OK;
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;
	if (FAILED(Super::Render()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUITutorial_Pannel_Text::Ready_Components(TUTORIAL_PANNEL_TEXT_DESC* pDesc)
{
	if (FAILED(Super::Ready_Components(pDesc)))
		return E_FAIL;
	return S_OK;
}

HRESULT CUITutorial_Pannel_Text::Bind_ShaderResources()
{
	CShader* pShader = Get_Component<CShader>();
	if (FAILED(Get_Component<CTransform>()->Bind_ShaderResource(pShader)))
		return E_FAIL;
	if (FAILED(Super::Bind_ShaderResources()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUITutorial_Pannel_Text::Attach_Personal_Info()
{
	if (m_isSpawned)
	{
		Set_Visible();
		m_isSpawned = false;
	}
	return S_OK;
}

HRESULT CUITutorial_Pannel_Text::Convert_Stat_To_Text()
{
	return S_OK;
}

void CUITutorial_Pannel_Text::Bind_Events()
{
	Super::Bind_Events();

	m_vecEventHandles.push_back(
		m_pUIManager->Get_UIEvents().Subscribe([this](const UIEVENT_DESC& Desc)
			{
				if (EUIEventID::TUTORIAL_PANNEL_END == Desc.eEventID)
				{
					this->Set_Invisible();
				}
			}));
}

void CUITutorial_Pannel_Text::Tick_By_Type(const _float fTimeDelta)
{
	m_iCurPageIdx = m_pParentCanvasCache->Get_CommonParam_uint()[CUR_PAGE];
	m_iMaxPageIdx = m_pParentCanvasCache->Get_CommonParam_uint()[MAX_PAGE];

	switch (m_eTextSubClassType)
	{
	case DTO::EUITextSubClassType::TUTORIAL_PANNEL_TITLE_TEXT:
		m_wstrText = m_vecTexts[m_iCurPageIdx];
		break;
	case DTO::EUITextSubClassType::TUTORIAL_PANNEL_CONTENTS_TEXT:
		m_wstrText = m_vecTexts[m_iCurPageIdx];
		break;
	case DTO::EUITextSubClassType::TUTORIAL_PANNEL_PREV_BTN_TEXT:
	{
		if (m_iCurPageIdx <= 0)
		{
			if(m_isFin_Event)
				m_vFontColor = Vec4{ 0.7f, 0.7f, 0.7f, 1.f };
		}
		else
		{
			m_vFontColor = m_vOriginFontColor;
		}
	}
		break;
	case DTO::EUITextSubClassType::TUTORIAL_PANNEL_NEXT_BTN_TEXT:
	{
		if (m_iCurPageIdx == m_iMaxPageIdx)
		{
			m_wstrText = L"닫기";
		}
		else
		{
			m_wstrText = L"다음 페이지";
		}
	}
		break;
	}

}

void CUITutorial_Pannel_Text::Initialize_Visible_Event()
{
	m_isActive = false;
	m_isFin_Event = false;

	switch (m_eTextSubClassType)
	{
	case DTO::EUITextSubClassType::TUTORIAL_PANNEL_TITLE_TEXT:
		Ready_Lerp_Movement(Vec2{ 0.f, 235.5 }, Vec2{ 0.f, 0.f }, 1.f, 3.f, 0.5f, true);
		Ready_Fade_Text(0.4f, 0.f, 1.f, m_fDelay);
		break;
	case DTO::EUITextSubClassType::TUTORIAL_PANNEL_CONTENTS_TEXT:
		Ready_Fade_Text(0.4f, 0.f, 1.f, 1.f);
		break;
	case DTO::EUITextSubClassType::TUTORIAL_PANNEL_PREV_BTN_TEXT:
		Ready_Fade_Text(0.4f, 0.f, 1.f, 1.f);
		break;
	case DTO::EUITextSubClassType::TUTORIAL_PANNEL_NEXT_BTN_TEXT:
		Ready_Fade_Text(0.4f, 0.f, 1.f, 1.f);
		break;
	}
}

void CUITutorial_Pannel_Text::Initialize_InVisible_Event()
{
	m_isActive = false;
	m_isFin_Event = false;
	Ready_Fade_Text(0.4f, 1.f, 0.f, m_fDelay);
}

_bool CUITutorial_Pannel_Text::Tick_Visible_Event(const _float fTimeDelta)
{
	switch (m_eTextSubClassType)
	{
	case DTO::EUITextSubClassType::TUTORIAL_PANNEL_TITLE_TEXT:
	{
		_bool is = Tick_Lerp_Movement(fTimeDelta);
		_bool isFade = Tick_Fade_Text(fTimeDelta);

		if (is && isFade)
		{
			m_isFin_Event = true;
			m_isActive = true;

			return true;
		}
	}
		break;
	case DTO::EUITextSubClassType::TUTORIAL_PANNEL_CONTENTS_TEXT:
	{
		_bool isFade = Tick_Fade_Text(fTimeDelta);

		if (isFade)
		{
			m_isFin_Event = true;
			m_isActive = true;

			return true;
		}
	}
	break;
	case DTO::EUITextSubClassType::TUTORIAL_PANNEL_PREV_BTN_TEXT:
	{
		_bool isFade = Tick_Fade_Text(fTimeDelta);

		if (isFade)
		{
			m_isFin_Event = true;
			m_isActive = true;

			return true;
		}
	}
	break;
	case DTO::EUITextSubClassType::TUTORIAL_PANNEL_NEXT_BTN_TEXT:
	{
		_bool isFade = Tick_Fade_Text(fTimeDelta);

		if (isFade)
		{
			m_isFin_Event = true;
			m_isActive = true;

			return true;
		}
	}
	break;
	}
	return false;
}

_bool CUITutorial_Pannel_Text::Tick_InVisible_Event(const _float fTimeDelta)
{
	if (Tick_Fade_Text(fTimeDelta))
	{
		Request_SetDead();
		m_isFin_Event = true;
		m_isActive = true;
		return true;
	}
	return false;
}

HRESULT CUITutorial_Pannel_Text::Spawn_FromPool(void* pArg)
{
	if (FAILED(Super::Spawn_FromPool(pArg)))
		return E_FAIL;
	
	m_vecTexts.clear();

	UI_PREFAB_DATA* pDesc = static_cast<UI_PREFAB_DATA*>(pArg);
	if (auto* pTutorialPannel = std::get_if<UI_TUTORIAL_PANNEL_PREFAB_DATA>(&pDesc->Data))
	{
		m_pParentCanvasCache = pDesc->pCanvas;

		m_eTutorialID = pTutorialPannel->eTutorialTypeID;

		switch (m_eTutorialID)
		{
		case Client::EUITutorialPannelTypeID::TUTORIAL_PANNEL_1:
			switch (m_eTextSubClassType)
			{
			case DTO::EUITextSubClassType::TUTORIAL_PANNEL_TITLE_TEXT:
				m_vecTexts.push_back(L"쉴드/HP");
				break;
			case DTO::EUITextSubClassType::TUTORIAL_PANNEL_CONTENTS_TEXT:
				m_vecTexts.push_back(L"캐릭터가 공격받을 시 기본적으로 하단의 쉴드가 우선적으로 소모되며, 쉴드가 소진되면, HP가 소모됩니다.");
				break;
			}
			break;
		case Client::EUITutorialPannelTypeID::TUTORIAL_PANNEL_2:
			switch (m_eTextSubClassType)
			{
			case DTO::EUITextSubClassType::TUTORIAL_PANNEL_TITLE_TEXT:
				m_vecTexts.push_back(L"원거리 무기");
				m_vecTexts.push_back(L"탄약");
				break;
			case DTO::EUITextSubClassType::TUTORIAL_PANNEL_CONTENTS_TEXT:
				m_vecTexts.push_back(L"원거리 무기로 멀리 있는 적과 높은 곳에 있는 적을 처치할 수 있습니다. 원거리 무기는 장전할 수 있는 탄환 수와 탄약에 제한이 있습니다.");
				m_vecTexts.push_back(L"적을 처치하거나, 주변의 물체를 파괴하는 등의 방법으로 원거리 무기의 탄약을 보충할 수 있는 탄약을 획득할 수 있습니다.");
				break;
			}
			break;
		case Client::EUITutorialPannelTypeID::TUTORIAL_PANNEL_3:
			switch (m_eTextSubClassType)
			{
			case DTO::EUITextSubClassType::TUTORIAL_PANNEL_TITLE_TEXT:
				m_vecTexts.push_back(L"캐릭터 매커니즘: '아스크의 징벌'");
				m_vecTexts.push_back(L"전투 매커니즘: 전투 태세");
				break;
			case DTO::EUITextSubClassType::TUTORIAL_PANNEL_CONTENTS_TEXT:
				m_vecTexts.push_back(L"행동 능력을 잃은 적은 약점을 드러내며, 이때 적에게 접근해서 강력한 일격을 입힐 수 있습니다. 일부 다이몬은 이러한 일격을 파멸과\n기회를 가져다주는 '아스크의 징벌'이라고 부릅니다.");
				m_vecTexts.push_back(L"강력한 적은 전투 태세를 유지하며, 아군의 공격으로 전투 태세를 약화시킬 수 있습니다. 전투 태세가 일정 수준으로 약화되면,\n적이 균형을 잃으며, 전투 태세가 완전히 약화되어 무너지면 적이 일시적으로 행동 능력을 잃습니다..");
				break;
			}
			break;
		case Client::EUITutorialPannelTypeID::TUTORIAL_PANNEL_4:
			switch (m_eTextSubClassType)
			{
			case DTO::EUITextSubClassType::TUTORIAL_PANNEL_TITLE_TEXT:
				m_vecTexts.push_back(L"캐릭터 종결 스킬: 빛보라");
				break;
			case DTO::EUITextSubClassType::TUTORIAL_PANNEL_CONTENTS_TEXT:
				m_vecTexts.push_back(L"'레조넌스'의 영향으로 플레이어가 종결 스킬을 각성했습니다.\n Q 버튼을 누르면 종결스킬 [빛보라]을 시전할 수 있습니다.\n[빛보라]를 사용하면 추가 쉴드와 강력한 데미지를 줄 수 있습니다.");
				break;
			}
			break;
		case Client::EUITutorialPannelTypeID::END:
		default:
			break;
		}
	}

	m_isSpawned			= true;
	m_isDeadRequest		= false;
	return S_OK;
}

HRESULT CUITutorial_Pannel_Text::Despawn_FromPool()
{
	if (FAILED(Super::Despawn_FromPool()))
		return E_FAIL;

	m_isVisible			= false;
	m_isVisibleTrigger	= false;
	m_isPreVisible		= false;
	return S_OK;
}

CUITutorial_Pannel_Text* CUITutorial_Pannel_Text::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CUITutorial_Pannel_Text* pInstance = new CUITutorial_Pannel_Text(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CUITutorial_Pannel_Text::Create, Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUITutorial_Pannel_Text::Clone(void* pArg)
{
	CUITutorial_Pannel_Text* pInstance = new CUITutorial_Pannel_Text(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CUITutorial_Pannel_Text::Clone, Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUITutorial_Pannel_Text::Free()
{
	Super::Free();
}
