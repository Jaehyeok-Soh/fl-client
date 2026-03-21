#include "pch.h"
#include "UITutorial_Pannel_Image.h"
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

// fParam0 -> Prev 버튼		// Hover 이펙트 밝기 조절용으로 사용중 
// fParam1 -> Next 버튼		// Hover 이펙트 밝기 조절용으로 사용중 
// isParam0 -> Prev 버튼	// 누르고 땐 순간 true Trigger
// isParam1 -> Next 버튼	// 누르고 땐 순간 true Trigger
// iParam0 -> Cur Page
// iParam1 -> Max Page

#define PREV_BUTTON 0
#define NEXT_BUTTON 1
#define CUR_PAGE 0
#define MAX_PAGE 1

#define PANNEL_TEXTURE_TAG_1	L"Texture_T_Guide_Img_Hp_Shield_PC"
#define PANNEL_TEXTURE_TAG_2	L"Texture_T_Guide_Img_Gun_PC"
#define PANNEL_TEXTURE_TAG_2_1	L"Texture_T_Guide_Img_Ammo_PC"
#define PANNEL_TEXTURE_TAG_3	L"Texture_T_Guide_Img_Toughness"
#define PANNEL_TEXTURE_TAG_3_1	L"Texture_T_Guide_Img_DestructablePart_04"
#define PANNEL_TEXTURE_TAG_4	L"Texture_T_Guide_Img_Secondskill"

CUITutorial_Pannel_Image::CUITutorial_Pannel_Image(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	:CUIDynamic_Image(pDevice, pDeviceContext)
{
}

CUITutorial_Pannel_Image::CUITutorial_Pannel_Image(const CUITutorial_Pannel_Image& rhs)
	:CUIDynamic_Image(rhs)
{
}

HRESULT CUITutorial_Pannel_Image::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUITutorial_Pannel_Image::Initialize(void* pArg)
{
	TUTORIAL_PANNEL_IMAGE_DESC* pDesc = static_cast<TUTORIAL_PANNEL_IMAGE_DESC*>(pArg);
	m_iNumbering = pDesc->iNumbering;
	
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;
	if (FAILED(Ready_Components(pDesc)))
		return E_FAIL;
	m_vOriginSize = Vec2{ m_fWidth, m_fHeight };

	return S_OK;
}


HRESULT CUITutorial_Pannel_Image::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;
	if (FAILED(Attach_Personal_Info()))
		return E_FAIL;


	return S_OK;
}

void CUITutorial_Pannel_Image::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CUITutorial_Pannel_Image::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);

	Tick_By_Type(fTimeDelta);
}

void CUITutorial_Pannel_Image::Update_Late(const _float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);
}

void CUITutorial_Pannel_Image::Ready_Before_Render(const _float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);
}

HRESULT CUITutorial_Pannel_Image::Render()
{
	if (!m_isVisible)
		return S_OK;
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;
	if (FAILED(Super::Render()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUITutorial_Pannel_Image::Ready_Components(TUTORIAL_PANNEL_IMAGE_DESC* pDesc)
{
	if (FAILED(Super::Ready_Components(pDesc)))
		return E_FAIL;
	return S_OK;
}

HRESULT CUITutorial_Pannel_Image::Bind_ShaderResources()
{
	CShader* pShader = Get_Component<CShader>();
	if (FAILED(Get_Component<CTransform>()->Bind_ShaderResource(pShader)))
		return E_FAIL;
	if (FAILED(Super::Bind_ShaderResources()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUITutorial_Pannel_Image::Attach_Personal_Info()
{
	switch (m_eDImageSubClass)
	{
	case DTO::EUIDImageSubClassType::TUTORIAL_PANNEL_TOP_BG:		// 윗 배경
		break;
	case DTO::EUIDImageSubClassType::TUTORIAL_PANNEL_BG:			// 디졸브 배경
		break;
	case DTO::EUIDImageSubClassType::TUTORIAL_PANNEL_BOTTOM_BG:		// 투명한 아래 배경
		break;
	case DTO::EUIDImageSubClassType::TUTORIAL_PANNEL_TOP_ICON:		// ? 아이콘
		break;
	case DTO::EUIDImageSubClassType::TUTORIAL_PANNEL_ICON:			// 설명 이미지
		break;
	case DTO::EUIDImageSubClassType::TUTORIAL_PANNEL_BUTTON:		// 버튼 0이 Prev, 1이 Next
		break;
	case DTO::EUIDImageSubClassType::TUTORIAL_PANNEL_BUTTON_FX:
		break;
	case DTO::EUIDImageSubClassType::TUTORIAL_PANNEL_BUTTON_OUTLINE:
		break;
	case DTO::EUIDImageSubClassType::TUTORIAL_PANNEL_NUM_PAGE_ICON:
		break;
	}

	if (m_isSpawned)
	{
		Set_Visible();
		m_isSpawned = false;
	}

	return S_OK;
}

void CUITutorial_Pannel_Image::Bind_Events()
{
	m_vecEventHandles.push_back(
		m_pUIManager->Get_UIEvents().Subscribe([this](const UIEVENT_DESC& Desc)
			{
				if (EUIEventID::TUTORIAL_PANNEL_START == Desc.eEventID)
				{
					this->Set_Visible();
					this->Set_Active(true);
				}
			})
	);

	m_vecEventHandles.push_back(
		m_pUIManager->Get_UIEvents().Subscribe([this](const UIEVENT_DESC& Desc)
			{
				if (EUIEventID::TUTORIAL_PANNEL_END == Desc.eEventID)
				{
					this->Set_Invisible();
				}
			})
	);
}

void CUITutorial_Pannel_Image::Tick_By_Type(const _float fTimeDelta)
{
	m_pParentCanvasCache->Get_CommonParam_bool_Ref()[PREV_BUTTON] = false;
	m_pParentCanvasCache->Get_CommonParam_bool_Ref()[NEXT_BUTTON] = false;

	// 부모 캔버스의 현재 페이지를 받아옴
	m_iCurPage = m_pParentCanvasCache->Get_CommonParam_uint()[CUR_PAGE];

	switch (m_eDImageSubClass)
	{
	case DTO::EUIDImageSubClassType::TUTORIAL_PANNEL_TOP_BG:		// 윗 배경
		break;
	case DTO::EUIDImageSubClassType::TUTORIAL_PANNEL_BG:			// 디졸브 배경
		break;
	case DTO::EUIDImageSubClassType::TUTORIAL_PANNEL_BOTTOM_BG:		// 투명한 아래 배경
		break;
	case DTO::EUIDImageSubClassType::TUTORIAL_PANNEL_TOP_ICON:		// ? 아이콘
		break;
	case DTO::EUIDImageSubClassType::TUTORIAL_PANNEL_ICON:			// 설명 이미지
	{
		if (m_eDImageSubClass == DTO::EUIDImageSubClassType::TUTORIAL_PANNEL_ICON)
		{
			if (FAILED(Get_Component<CTexture>()->Add_DefaultTexture(m_vecTextureTags[m_iCurPage], ENUM_TO_UINT(EUITextureSlot::DEFAULT))))
				return;
		}
	}
	break;
	case DTO::EUIDImageSubClassType::TUTORIAL_PANNEL_BUTTON:		// 버튼 0이 Prev, 1이 Next
	{
		//호버 됐을 떄 
		{
			if (Engine_Utils::Has_Flag(m_iInteractState, EUIInteract_Flag::HOVER_ENTER))
			{
				m_isHoverEnter = true;
				m_isHoverExit = false;
			}
			else if (Engine_Utils::Has_Flag(m_iInteractState, EUIInteract_Flag::HOVER_EXIT))
			{
				m_isHoverEnter = false;
				m_isHoverExit = true;
			}
			if (m_isHoverEnter)
			{
				m_fPannelBrightNess += fTimeDelta * 10.f;

				if (m_fPannelBrightNess >= 3.f)
					m_fPannelBrightNess = 3.f;
			}
			else if (m_isHoverExit)
			{
				m_fPannelBrightNess -= fTimeDelta * 10.f;

				if (m_fPannelBrightNess <= 0.5f)
					m_fPannelBrightNess = 0.5f;
			}

			if (m_iNumbering == PREV_BUTTON)
			{
				m_pParentCanvasCache->Get_CommonParam_float_Ref()[PREV_BUTTON] = m_fPannelBrightNess;
			}
			else if (m_iNumbering == NEXT_BUTTON)
			{
				m_pParentCanvasCache->Get_CommonParam_float_Ref()[NEXT_BUTTON] = m_fPannelBrightNess;
			}
		}


		// 눌렀다 땠을 때 
		{
			if (Engine_Utils::Has_Flag(m_iInteractState, EUIInteract_Flag::PRESS_EXIT))
			{
				if (m_iNumbering == PREV_BUTTON)
				{
					m_pParentCanvasCache->Get_CommonParam_bool_Ref()[PREV_BUTTON] = true;

					if (0 <= m_iCurPage)
						m_iCurPage = 0;
					else
						m_iCurPage--;
				}
				else if (m_iNumbering == NEXT_BUTTON)
				{
					m_pParentCanvasCache->Get_CommonParam_bool_Ref()[NEXT_BUTTON] = true;

					if (m_iCurPage == m_iMaxPage)
					{
						m_iCurPage = m_iMaxPage;

						UIEVENT_DESC Desc = {};
						Desc.eEventID = EUIEventID::TUTORIAL_PANNEL_END;
						CUI_Manager::GetInstance()->Get_UIEvents().Broadcast(Desc);
					}
					else
						m_iCurPage++;
				}

				// 바뀌었다면 바꾸기
				m_pParentCanvasCache->Get_CommonParam_uint_Ref()[CUR_PAGE] = m_iCurPage;
			}
		}
		// 페이지별 상태
		{
			_uint i = m_iCurPage;

			if (i == 0)
			{
				if (m_iNumbering == PREV_BUTTON)
				{
					m_vColorTint = Vec4{ 0.5f, 0.5f, 0.5f, 0.5f };
					m_isInteract = false;
				}
			}
			else
			{
				if (m_iNumbering == PREV_BUTTON)
				{
					m_vColorTint = Vec4{ 0.f, 0.f, 0.f, 0.f };
					m_isInteract = true;
				}
			}
		}
	}
	break;
	case DTO::EUIDImageSubClassType::TUTORIAL_PANNEL_BUTTON_FX:
	case DTO::EUIDImageSubClassType::TUTORIAL_PANNEL_BUTTON_OUTLINE:
	{
		if (m_iNumbering == PREV_BUTTON)
		{
			m_fBrightness = m_pParentCanvasCache->Get_CommonParam_float()[PREV_BUTTON];
		}
		else if (m_iNumbering == NEXT_BUTTON)
		{
			m_fBrightness = m_pParentCanvasCache->Get_CommonParam_float()[NEXT_BUTTON];
		}
	}
	break;
	case DTO::EUIDImageSubClassType::TUTORIAL_PANNEL_NUM_PAGE_ICON:
	{
		if (m_iNumbering == m_iCurPage)
			m_fBrightness = 3.f;
		else
			m_fBrightness = 1.f;
	}
		break;
	}
}

void CUITutorial_Pannel_Image::Initialize_Visible_Event()
{
	m_isFin_Event = false;
	m_isActive = false;

	switch (m_eDImageSubClass)
	{
	case DTO::EUIDImageSubClassType::TUTORIAL_PANNEL_TOP_BG:		// 윗 배경
		Ready_Lerp_Movement(Vec2{ 0.f, 235.5 }, Vec2{ 0.f, 0.f }, 1.f, 3.f, 0.5f, true);
		Ready_LerpChange(0.2f, m_vOriginSize.x - 50.f, m_vOriginSize.x, 2.f, 0.f, true);
		m_fAlpha_Ratio = 1.f;
		break;

	case DTO::EUIDImageSubClassType::TUTORIAL_PANNEL_BG:			// 디졸브 배경
		Ready_LerpChange(0.5f, 1.f, 0.f, 2.f, 1.f);
		m_fAlpha_Ratio = 1.f;
		break;

	case DTO::EUIDImageSubClassType::TUTORIAL_PANNEL_BOTTOM_BG:		// 투명한 아래 배경
		Ready_Fade(0.5f, 0.f, 0.6f, 1.f);
		break;

	case DTO::EUIDImageSubClassType::TUTORIAL_PANNEL_TOP_ICON:		// ? 아이콘
		Ready_Lerp_Movement(Vec2{ 0.f, 235.5 }, Vec2{ 0.f, 0.f }, 1.f, 3.f, 0.5f, true);
		Ready_LerpChange(0.3f, 2.f, 1.f, 1.f, 0.f);
		m_fAlpha_Ratio = 1.f;
		break;

	case DTO::EUIDImageSubClassType::TUTORIAL_PANNEL_ICON:			// 설명 이미지
		Ready_Fade(0.5f, 0.f, 1.f, 1.f);
		break;

	case DTO::EUIDImageSubClassType::TUTORIAL_PANNEL_BUTTON:		// 버튼 0이 Prev, 1이 Next
		Ready_Fade(0.5f, 0.f, 1.f, 1.f);
		break;

	case DTO::EUIDImageSubClassType::TUTORIAL_PANNEL_BUTTON_FX:
		Ready_Fade(0.5f, 0.f, 1.f, 1.f);
		break;

	case DTO::EUIDImageSubClassType::TUTORIAL_PANNEL_BUTTON_OUTLINE:
		Ready_Fade(0.5f, 0.f, 1.f, 1.f);
		break;

	case DTO::EUIDImageSubClassType::TUTORIAL_PANNEL_NUM_PAGE_ICON:
	{
		if (m_isNotVisible)
		{
			Ready_Fade(0.5f, 0.f, 0.f, 1.f);
		}
		else
		{
			Ready_Fade(0.5f, 0.f, 1.f, 1.f);
		}
		break;
	}
		
	}
}

_bool CUITutorial_Pannel_Image::Tick_Visible_Event(const _float fTimeDelta)
{
	m_isActive = true;

	switch (m_eDImageSubClass)
	{
	case DTO::EUIDImageSubClassType::TUTORIAL_PANNEL_TOP_BG:		// 윗 배경
	{
		_bool is = Tick_Lerp_Movement(fTimeDelta);
		_bool isChange = Tick_LerpChange(&m_fWidth, fTimeDelta);

		if (is)
		{
			m_isFin_Event	= true;
			m_isActive		= true;

			return true;
		}
	}
	break;
	case DTO::EUIDImageSubClassType::TUTORIAL_PANNEL_BG:			// 디졸브 배경
	{
		_bool is = Tick_LerpChange(&m_fProgress_Ratio, fTimeDelta);
		if (is)
		{
			m_isFin_Event = true;
			m_isActive = true;

			return true;
		}
	}
	break;
	case DTO::EUIDImageSubClassType::TUTORIAL_PANNEL_BOTTOM_BG:		// 투명한 아래 배경
	{
		_bool is = Tick_Fade(fTimeDelta);
		if (is)
		{
			m_isFin_Event = true;
			m_isActive = true;

			return true;
		}
	}
	break;
	case DTO::EUIDImageSubClassType::TUTORIAL_PANNEL_TOP_ICON:		// ? 아이콘
	{
		_bool is = Tick_Lerp_Movement(fTimeDelta);
		_bool isScale = Tick_LerpChange(&m_fScale, fTimeDelta);
		if (is && isScale)
		{
			m_isFin_Event = true;
			m_isActive = true;

			return true;
		}
	}
	break;
	case DTO::EUIDImageSubClassType::TUTORIAL_PANNEL_ICON:			// 설명 이미지
	{
		_bool is = Tick_Fade(fTimeDelta);
		if (is)
		{
			m_isFin_Event = true;
			m_isActive = true;

			return true;
		}
	}
	break;
	case DTO::EUIDImageSubClassType::TUTORIAL_PANNEL_BUTTON:		// 버튼 0이 Prev, 1이 Next
	{
		_bool is = Tick_Fade(fTimeDelta);
		if (is)
		{
			m_isFin_Event = true;
			m_isActive = true;

			return true;
		}
	}
	break;
	case DTO::EUIDImageSubClassType::TUTORIAL_PANNEL_BUTTON_FX:
	{
		_bool is = Tick_Fade(fTimeDelta);
		if (is)
		{
			m_isFin_Event = true;
			m_isActive = true;

			return true;
		}
	}
	break;
	case DTO::EUIDImageSubClassType::TUTORIAL_PANNEL_BUTTON_OUTLINE:
	{
		_bool is = Tick_Fade(fTimeDelta);
		if (is)
		{
			m_isFin_Event = true;
			m_isActive = true;

			return true;
		}
	}
	break;
	case DTO::EUIDImageSubClassType::TUTORIAL_PANNEL_NUM_PAGE_ICON:
	{
		_bool is = Tick_Fade(fTimeDelta);
		if (is)
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

void CUITutorial_Pannel_Image::Initialize_InVisible_Event()
{
	m_isActive = false;
	m_isFin_Event = false;

	switch (m_eDImageSubClass)
	{
	case DTO::EUIDImageSubClassType::TUTORIAL_PANNEL_BG:			// 디졸브 배경
		Ready_Fade(0.5f, m_fAlpha_Ratio, 0.f, m_fDelay);
		Ready_LerpChange(0.5f, 0.f, 1.f, 1.f, m_fDelay);
		break;

	case DTO::EUIDImageSubClassType::TUTORIAL_PANNEL_TOP_BG:		// 윗 배경
	case DTO::EUIDImageSubClassType::TUTORIAL_PANNEL_BOTTOM_BG:		// 투명한 아래 배경
	case DTO::EUIDImageSubClassType::TUTORIAL_PANNEL_TOP_ICON:		// ? 아이콘
	case DTO::EUIDImageSubClassType::TUTORIAL_PANNEL_ICON:			// 설명 이미지
	case DTO::EUIDImageSubClassType::TUTORIAL_PANNEL_BUTTON:		// 버튼 0이 Prev, 1이 Next
	case DTO::EUIDImageSubClassType::TUTORIAL_PANNEL_BUTTON_FX:
	case DTO::EUIDImageSubClassType::TUTORIAL_PANNEL_BUTTON_OUTLINE:
	case DTO::EUIDImageSubClassType::TUTORIAL_PANNEL_NUM_PAGE_ICON:
		Ready_Fade(0.5f, m_fAlpha_Ratio, 0.f, m_fDelay);
		break;
	}
}

_bool CUITutorial_Pannel_Image::Tick_InVisible_Event(const _float fTimeDelta)
{
	switch (m_eDImageSubClass)
	{
	case DTO::EUIDImageSubClassType::TUTORIAL_PANNEL_BG:			// 디졸브 배경
	{
		_bool is = Tick_LerpChange(&m_fProgress_Ratio, fTimeDelta);
		_bool isFade = Tick_Fade(fTimeDelta);

		if (is && isFade)
		{
			m_isFin_Event = true;
			m_isActive = true;
			Request_SetDead();
			return true;
		}
	}
	break;

	case DTO::EUIDImageSubClassType::TUTORIAL_PANNEL_TOP_BG:		// 윗 배경
	case DTO::EUIDImageSubClassType::TUTORIAL_PANNEL_BOTTOM_BG:		// 투명한 아래 배경
	case DTO::EUIDImageSubClassType::TUTORIAL_PANNEL_TOP_ICON:		// ? 아이콘
	case DTO::EUIDImageSubClassType::TUTORIAL_PANNEL_ICON:			// 설명 이미지
	case DTO::EUIDImageSubClassType::TUTORIAL_PANNEL_BUTTON:		// 버튼 0이 Prev, 1이 Next
	case DTO::EUIDImageSubClassType::TUTORIAL_PANNEL_BUTTON_FX:
	case DTO::EUIDImageSubClassType::TUTORIAL_PANNEL_BUTTON_OUTLINE:
	case DTO::EUIDImageSubClassType::TUTORIAL_PANNEL_NUM_PAGE_ICON:
	{
		_bool isFade = Tick_Fade(fTimeDelta);

		if (isFade)
		{
			m_isFin_Event = true;
			m_isActive = true;
			Request_SetDead();
			return true;
		}
	}
	break;
	}

	return false;
}

HRESULT CUITutorial_Pannel_Image::Spawn_FromPool(void* pArg)
{
	if (FAILED(Super::Spawn_FromPool(pArg)))
		return E_FAIL;

	UI_PREFAB_DATA* pDesc = static_cast<UI_PREFAB_DATA*>(pArg);

	if (auto* pPannel = std::get_if<UI_TUTORIAL_PANNEL_PREFAB_DATA>(&pDesc->Data))
	{
		m_pParentCanvasCache = pDesc->pCanvas;
		m_eTutorialID = pPannel->eTutorialTypeID;

		switch (m_eTutorialID)
		{
		case Client::EUITutorialPannelTypeID::TUTORIAL_PANNEL_1:
			m_iMaxPage = 0;
			m_vecTextureTags.push_back(PANNEL_TEXTURE_TAG_1);
			break;
		case Client::EUITutorialPannelTypeID::TUTORIAL_PANNEL_2:
			m_iMaxPage = 1;
			m_vecTextureTags.push_back(PANNEL_TEXTURE_TAG_2);
			m_vecTextureTags.push_back(PANNEL_TEXTURE_TAG_2_1);
			break;
		case Client::EUITutorialPannelTypeID::TUTORIAL_PANNEL_3:
			m_vecTextureTags.push_back(PANNEL_TEXTURE_TAG_3);
			m_vecTextureTags.push_back(PANNEL_TEXTURE_TAG_3_1);
			m_iMaxPage = 1;
			break;
		case Client::EUITutorialPannelTypeID::TUTORIAL_PANNEL_4:
			m_vecTextureTags.push_back(PANNEL_TEXTURE_TAG_4);
			m_iMaxPage = 0;
			break;
		case Client::EUITutorialPannelTypeID::END:
		default:
			break;
		}

		m_iCurPage = 0;
	
		m_pParentCanvasCache->Get_CommonParam_uint_Ref()[CUR_PAGE] = m_iCurPage;
		m_pParentCanvasCache->Get_CommonParam_uint_Ref()[MAX_PAGE] = m_iMaxPage;
		if (m_eDImageSubClass == DTO::EUIDImageSubClassType::TUTORIAL_PANNEL_ICON)
		{
			if (FAILED(Get_Component<CTexture>()->Add_DefaultTexture(m_vecTextureTags.front(), ENUM_TO_UINT(EUITextureSlot::DEFAULT))))
				return E_FAIL;
		}

		switch (m_iMaxPage)
		{
		case 0:
		{
			if (m_eDImageSubClass == DTO::EUIDImageSubClassType::TUTORIAL_PANNEL_NUM_PAGE_ICON)
			{
				if (0 == m_iNumbering)
				{
				}
				else if (1 == m_iNumbering)
				{
					m_isNotVisible = true;
				}
			}
		}
		break;
		case 1:
		{
			if (m_eDImageSubClass == DTO::EUIDImageSubClassType::TUTORIAL_PANNEL_NUM_PAGE_ICON)
			{
				if (0 == m_iNumbering)
				{
					m_vMoveOffset = Vec2{ -20.f, 0.f };
				}
				else if (1 == m_iNumbering)
				{
					m_vMoveOffset = Vec2{ 20.f, 0.f };
				}
			}
		}
		break;
		}
	}

	m_isSpawned = true;
	m_isDeadRequest = false;
	return S_OK;
}

HRESULT CUITutorial_Pannel_Image::Despawn_FromPool()
{
	if (FAILED(Super::Despawn_FromPool()))
		return E_FAIL;

	m_isVisible = false;
	m_isVisibleTrigger = false;
	m_isPreVisible = false;
	m_isNotVisible = false;

	this->Set_Active(false);
	m_vecTextureTags.clear();
	return S_OK;
}

CUITutorial_Pannel_Image* CUITutorial_Pannel_Image::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CUITutorial_Pannel_Image* pInstance = new CUITutorial_Pannel_Image(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CUITutorial_Pannel_Image::Create, Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUITutorial_Pannel_Image::Clone(void* pArg)
{
	CUITutorial_Pannel_Image* pInstance = new CUITutorial_Pannel_Image(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CUITutorial_Pannel_Image::Clone, Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUITutorial_Pannel_Image::Free()
{
	Super::Free();
}
