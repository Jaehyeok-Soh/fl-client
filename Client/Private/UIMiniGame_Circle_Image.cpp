#include "pch.h"
#include "UIMiniGame_Circle_Image.h"
#include "Client_Defines.h"
#include "Client_EventDefine.h"
//=================
// Component
//=================
#include "WorldUI_Component.h"
#include "Texture.h"
#include "Shader.h"
#include "VIBuffer_Rect_Tex.h"
#include "UI_Manager.h"
#include "GameInstance.h"

CUIMiniGame_Circle_Image::CUIMiniGame_Circle_Image(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	:CUIDynamic_Image(pDevice, pDeviceContext)
{
}

CUIMiniGame_Circle_Image::CUIMiniGame_Circle_Image(const CUIMiniGame_Circle_Image& rhs)
	:CUIDynamic_Image(rhs)
{
}

HRESULT CUIMiniGame_Circle_Image::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIMiniGame_Circle_Image::Initialize(void* pArg)
{
	MINIGAME_CIRCLE_IMAGE_DESC* pDesc = static_cast<MINIGAME_CIRCLE_IMAGE_DESC*>(pArg);
	m_iNumbering = pDesc->iNumber;

	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;
	if (FAILED(Ready_Components(pDesc)))
		return E_FAIL;

	return S_OK;
}


HRESULT CUIMiniGame_Circle_Image::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;
	if (FAILED(Attach_Personal_Info()))
		return E_FAIL;

	return S_OK;
}

void CUIMiniGame_Circle_Image::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CUIMiniGame_Circle_Image::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
	Tick_By_Type(fTimeDelta);
}

void CUIMiniGame_Circle_Image::Update_Late(const _float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);
}

void CUIMiniGame_Circle_Image::Ready_Before_Render(const _float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);
}

HRESULT CUIMiniGame_Circle_Image::Render()
{
	if (!m_isVisible)
		return S_OK;
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;
	if (FAILED(Super::Render()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIMiniGame_Circle_Image::Ready_Components(MINIGAME_CIRCLE_IMAGE_DESC* pDesc)
{
	if (FAILED(Super::Ready_Components(pDesc)))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIMiniGame_Circle_Image::Bind_ShaderResources()
{
	CShader* pShader = Get_Component<CShader>();
	if (FAILED(Get_Component<CTransform>()->Bind_ShaderResource(pShader)))
		return E_FAIL;
	if (FAILED(Super::Bind_ShaderResources()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIMiniGame_Circle_Image::Attach_Personal_Info()
{
	return S_OK;
}

void CUIMiniGame_Circle_Image::Bind_Events()
{
	m_vecEventHandles.push_back(
		m_pGameInstance->Subscribe<MINIGAME_CIRCLE_ON>([this]()
			{
				switch (m_eDImageSubClass)
				{
				case DTO::EUIDImageSubClassType::MINIGAME_CIRCLE_BG:
				case DTO::EUIDImageSubClassType::MINIGAME_CIRCLE_INNER_BG:
				case DTO::EUIDImageSubClassType::MINIGAME_CIRCLE_TIMER_BG:
				case DTO::EUIDImageSubClassType::MINIGAME_CIRCLE_TIMER_CIRCLE:
				case DTO::EUIDImageSubClassType::MINIGAME_CIRCLE_POINT:
				case DTO::EUIDImageSubClassType::MINIGAME_CIRCLE_POINT_BG:
				case DTO::EUIDImageSubClassType::MINIGAME_CIRCLE_POINT_FX:
				case DTO::EUIDImageSubClassType::MINIGAME_CIRCLE_CURSOR:
				case DTO::EUIDImageSubClassType::MINIGAME_CIRCLE_KEY_ICON:
				case DTO::EUIDImageSubClassType::MINIGAME_CIRCLE_SPACE_ICON:
					Set_Visible();
					Set_Active(true);
					break;
				}
			})
	);

	m_vecEventHandles.push_back(
		m_pGameInstance->Subscribe<MINIGAME_CIRCLE_CLEAR>([this]()
			{

			})
	);

	m_vecEventHandles.push_back(
		m_pGameInstance->Subscribe<MINIGAME_CIRCLE_OFF>([this]()
			{
				switch (m_eDImageSubClass)
				{
				case DTO::EUIDImageSubClassType::MINIGAME_CIRCLE_BG:
				case DTO::EUIDImageSubClassType::MINIGAME_CIRCLE_INNER_BG:
				case DTO::EUIDImageSubClassType::MINIGAME_CIRCLE_TIMER_BG:
				case DTO::EUIDImageSubClassType::MINIGAME_CIRCLE_TIMER_CIRCLE:
				case DTO::EUIDImageSubClassType::MINIGAME_CIRCLE_POINT:
				case DTO::EUIDImageSubClassType::MINIGAME_CIRCLE_POINT_BG:
				case DTO::EUIDImageSubClassType::MINIGAME_CIRCLE_POINT_FX:
				case DTO::EUIDImageSubClassType::MINIGAME_CIRCLE_CURSOR:
				case DTO::EUIDImageSubClassType::MINIGAME_CIRCLE_KEY_ICON:
				case DTO::EUIDImageSubClassType::MINIGAME_CIRCLE_SPACE_ICON:
					Set_Invisible();
					break;
				}
			})
	);
}

void CUIMiniGame_Circle_Image::Tick_By_Type(const _float fTimeDelta)
{
	switch (m_eDImageSubClass)
	{
	case DTO::EUIDImageSubClassType::MINIGAME_CIRCLE_BG:
		break;
	case DTO::EUIDImageSubClassType::MINIGAME_CIRCLE_INNER_BG:
		break;
	case DTO::EUIDImageSubClassType::MINIGAME_CIRCLE_TIMER_BG:
		break;
	case DTO::EUIDImageSubClassType::MINIGAME_CIRCLE_TIMER_CIRCLE:
		break;
	case DTO::EUIDImageSubClassType::MINIGAME_CIRCLE_POINT:
		break;
	case DTO::EUIDImageSubClassType::MINIGAME_CIRCLE_POINT_BG:
		break;
	case DTO::EUIDImageSubClassType::MINIGAME_CIRCLE_POINT_FX:
		break;
	case DTO::EUIDImageSubClassType::MINIGAME_CIRCLE_CLEAR_POINT:
		break;
	case DTO::EUIDImageSubClassType::MINIGAME_CIRCLE_CLEAR_POINT_BG:
		break;
	case DTO::EUIDImageSubClassType::MINIGAME_CIRCLE_CLEAR_POINT_BG_S:
		break;
	case DTO::EUIDImageSubClassType::MINIGAME_CIRCLE_CLEAR_POINT_FX:
		break;
	case DTO::EUIDImageSubClassType::MINIGAME_CIRCLE_CURSOR:
		Tick_For_Cursor(fTimeDelta);
		break;
	case DTO::EUIDImageSubClassType::MINIGAME_CIRCLE_KEY_ICON:
		break;
	case DTO::EUIDImageSubClassType::MINIGAME_CIRCLE_SPACE_ICON:
		break;
	case DTO::EUIDImageSubClassType::MINIGAME_CIRCLE_CLEAR_CIRCLE_FX:
		break;
	}
}

void CUIMiniGame_Circle_Image::Tick_For_Cursor(const _float fTimeDelta)
{
	m_fRad += fTimeDelta;

	_float x = cos(m_fRad) * 150;
	_float y = sin(m_fRad) * 150;

	m_vMoveOffset = Vec2{x, y};

	Move_Rotate(m_fRad - XM_PI);
}

void CUIMiniGame_Circle_Image::Initialize_Visible_Event()
{
	switch (m_eDImageSubClass)
	{
	case DTO::EUIDImageSubClassType::MINIGAME_CIRCLE_BG:
	case DTO::EUIDImageSubClassType::MINIGAME_CIRCLE_INNER_BG:
	case DTO::EUIDImageSubClassType::MINIGAME_CIRCLE_TIMER_BG:
	case DTO::EUIDImageSubClassType::MINIGAME_CIRCLE_TIMER_CIRCLE:
		Ready_Fade(0.5f, 0.f, 1.f, m_fDelay);
		Ready_LerpChange(0.5f, 2.f, 1.f, 3.f, m_fDelay, true);

		switch (m_iNumbering)
		{
		case 0: // 왼쪽 아래
			Ready_Lerp_Movement(Vec2{ m_fWidth * 0.5f, (-m_fHeight * 0.5f) }, Vec2{ 0.f, 0.f }, 0.5f, 3.f, m_fDelay, true);
			break;

		case 1: // 왼쪽 위
			Ready_Lerp_Movement(Vec2{ m_fWidth * 0.5f, m_fHeight * 0.5f }, Vec2{ 0.f, 0.f }, 0.5f, 3.f, m_fDelay, true);
			break;

		case 2: // 오른쪽 위
			Ready_Lerp_Movement(Vec2{ (-m_fWidth * 0.5f), m_fHeight * 0.5f }, Vec2{ 0.f, 0.f }, 0.5f, 3.f, m_fDelay, true);
			break;

		case 3: // 오른쪽 아래
			Ready_Lerp_Movement(Vec2{ (-m_fWidth * 0.5f), (-m_fHeight * 0.5f) }, Vec2{ 0.f, 0.f }, 0.5f, 3.f, m_fDelay, true);
			break;
		}
		break;

	case DTO::EUIDImageSubClassType::MINIGAME_CIRCLE_CURSOR:
	case DTO::EUIDImageSubClassType::MINIGAME_CIRCLE_KEY_ICON:
	case DTO::EUIDImageSubClassType::MINIGAME_CIRCLE_SPACE_ICON:
		Ready_Fade(0.5f, 0.f, 1.f, m_fDelay);
		Ready_LerpChange(0.5f, 2.f, 1.f, 3.f, m_fDelay, true);
		break;

	case DTO::EUIDImageSubClassType::MINIGAME_CIRCLE_POINT:
	case DTO::EUIDImageSubClassType::MINIGAME_CIRCLE_POINT_BG:
	case DTO::EUIDImageSubClassType::MINIGAME_CIRCLE_POINT_FX:
		Ready_Fade(0.5f, 0.f, 1.f, m_fDelay);
		break;

	case DTO::EUIDImageSubClassType::MINIGAME_CIRCLE_CLEAR_POINT:
		break;
	case DTO::EUIDImageSubClassType::MINIGAME_CIRCLE_CLEAR_POINT_BG:
		break;
	case DTO::EUIDImageSubClassType::MINIGAME_CIRCLE_CLEAR_POINT_BG_S:
		break;
	case DTO::EUIDImageSubClassType::MINIGAME_CIRCLE_CLEAR_POINT_FX:
		break;

	case DTO::EUIDImageSubClassType::MINIGAME_CIRCLE_CLEAR_CIRCLE_FX:
		break;
	}
}

_bool CUIMiniGame_Circle_Image::Tick_Visible_Event(const _float fTimeDelta)
{
	switch (m_eDImageSubClass)
	{
	case DTO::EUIDImageSubClassType::MINIGAME_CIRCLE_BG:
	case DTO::EUIDImageSubClassType::MINIGAME_CIRCLE_INNER_BG:
	case DTO::EUIDImageSubClassType::MINIGAME_CIRCLE_TIMER_BG:
	case DTO::EUIDImageSubClassType::MINIGAME_CIRCLE_TIMER_CIRCLE:
	{
		_bool isFade = Tick_Fade(fTimeDelta);
		_bool isChange = Tick_LerpChange(&m_fScale, fTimeDelta);
		_bool isMove = Tick_Lerp_Movement(fTimeDelta);

		if (isFade && isChange && isMove)
		{
			return true;
		}
	}
	break;
	case DTO::EUIDImageSubClassType::MINIGAME_CIRCLE_CURSOR:
	case DTO::EUIDImageSubClassType::MINIGAME_CIRCLE_KEY_ICON:
	case DTO::EUIDImageSubClassType::MINIGAME_CIRCLE_SPACE_ICON:
	{
		_bool isFade = Tick_Fade(fTimeDelta);
		_bool isChange = Tick_LerpChange(&m_fScale, fTimeDelta);

		if (isFade && isChange)
		{
			return true;
		}
	}
		break;

	case DTO::EUIDImageSubClassType::MINIGAME_CIRCLE_POINT:
	case DTO::EUIDImageSubClassType::MINIGAME_CIRCLE_POINT_BG:
	case DTO::EUIDImageSubClassType::MINIGAME_CIRCLE_POINT_FX:
	{
		_bool isFade = Tick_Fade(fTimeDelta);

		if (isFade)
		{
			return true;
		}
	}
		break;

	case DTO::EUIDImageSubClassType::MINIGAME_CIRCLE_CLEAR_POINT:
		break;
	case DTO::EUIDImageSubClassType::MINIGAME_CIRCLE_CLEAR_POINT_BG:
		break;
	case DTO::EUIDImageSubClassType::MINIGAME_CIRCLE_CLEAR_POINT_BG_S:
		break;
	case DTO::EUIDImageSubClassType::MINIGAME_CIRCLE_CLEAR_POINT_FX:
		break;

	case DTO::EUIDImageSubClassType::MINIGAME_CIRCLE_CLEAR_CIRCLE_FX:
		break;
	}

	return false;
}

void CUIMiniGame_Circle_Image::Initialize_InVisible_Event()
{

}

_bool CUIMiniGame_Circle_Image::Tick_InVisible_Event(const _float fTimeDelta)
{
	Set_Active(false);
	return true;
}

CUIMiniGame_Circle_Image* CUIMiniGame_Circle_Image::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CUIMiniGame_Circle_Image* pInstance = new CUIMiniGame_Circle_Image(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CUIMiniGame_Circle_Image::Create, Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUIMiniGame_Circle_Image::Clone(void* pArg)
{
	CUIMiniGame_Circle_Image* pInstance = new CUIMiniGame_Circle_Image(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CUIMiniGame_Circle_Image::Clone, Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUIMiniGame_Circle_Image::Free()
{
	Super::Free();
}
