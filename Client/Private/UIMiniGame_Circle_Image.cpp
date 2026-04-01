#include "pch.h"
#include "UIMiniGame_Circle_Image.h"
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

#define CURRENT_RADIAN_SLOT 0
#define CURRENT_SPEED_SLOT 1


#define ALL_POINT_CLEAR_SLOT 6
#define FAILED_TO_CLEAR_SLOT 7

// Common Value
// bool		// 0 ~ 5 -> 플레이어가 각도에 맞게 스페이스 잘 눌렀으면 true 
//			// 6 -> 전부 눌렸는지
//			// 7 -> 시간 내에 성공 못했는지
// float	// 0 -> 현재 커서 각도 (Radian)
// float	// 0 -> 현재 커서 각도 (Radian)

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
	switch (m_eDImageSubClass)
	{
	case DTO::EUIDImageSubClassType::MINIGAME_CIRCLE_BG:
		Set_Active(false);
		break;
	case DTO::EUIDImageSubClassType::MINIGAME_CIRCLE_INNER_BG:
		Set_Active(false);
		break;
	case DTO::EUIDImageSubClassType::MINIGAME_CIRCLE_TIMER_BG:
		Set_Active(false);
		break;
	case DTO::EUIDImageSubClassType::MINIGAME_CIRCLE_TIMER_CIRCLE:
		Set_Active(false);
		break;
	case DTO::EUIDImageSubClassType::MINIGAME_CIRCLE_POINT:
	{
		Set_Active(false);
		switch (m_iNumbering)
		{
		case 0: m_vClampAngle = { 20.f, 45.f};
			  m_pParentCanvasCache->Get_CommonParam_bool_Ref().resize(10);
			  break;
		case 1: m_vClampAngle = { 80.f, 100.f };
			  break;
		case 2: m_vClampAngle = { 135.f, 160.f};
			  break;
		case 3: m_vClampAngle = { 200.f, 225.f};
			  break;
		case 4: m_vClampAngle = { 260.f, 280.f };
			  break;
		case 5: m_vClampAngle = { 315.f, 340.f};
			  break;
		}
	}
		break;
	case DTO::EUIDImageSubClassType::MINIGAME_CIRCLE_POINT_BG:
		Set_Active(false);
		break;
	case DTO::EUIDImageSubClassType::MINIGAME_CIRCLE_POINT_FX:
		Set_Active(false);
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
		Set_Active(false);
		break;
	case DTO::EUIDImageSubClassType::MINIGAME_CIRCLE_KEY_ICON:
		Set_Active(false);
		break;
	case DTO::EUIDImageSubClassType::MINIGAME_CIRCLE_SPACE_ICON:
		Set_Active(false);
		break;
	case DTO::EUIDImageSubClassType::MINIGAME_CIRCLE_CLEAR_CIRCLE_FX:
		break;

	}
	return S_OK;
}

void CUIMiniGame_Circle_Image::Bind_Events()
{
	Super::Bind_Events();

	m_vecEventHandles.push_back(
		m_pGameInstance->Subscribe<MINIGAME_CIRCLE_ON>([this]()
			{
				auto& vecbool = m_pParentCanvasCache->Get_CommonParam_bool_Ref();
				std::fill(vecbool.begin(), vecbool.end(), false);

				auto& vecfloat = m_pParentCanvasCache->Get_CommonParam_float_Ref();
				std::fill(vecfloat.begin(), vecfloat.end(), 1.f);

				Set_Active(true);
				m_isFirstEntered = false;

				switch (m_eDImageSubClass)
				{
				case DTO::EUIDImageSubClassType::MINIGAME_CIRCLE_TIMER_CIRCLE:
				{
					m_pGameInstance->Active_TimeStop();

					Set_Visible();
					m_vColorTint = Vec4{ 1.f, 1.f, 1.f, 1.f };
					m_vGradiantColorTint = Vec4{ 1.f, 1.f, 1.f, 1.f };
				}
				break;
				case DTO::EUIDImageSubClassType::MINIGAME_CIRCLE_KEY_ICON:
				{
					Set_Visible();
					m_vColorTint = Vec4{ 1.f, 1.f, 1.f, 1.f };
					m_vGradiantColorTint = Vec4{ 1.f, 1.f, 1.f, 1.f };
					Move_Rotate(0.f);
				}
				break;
				case DTO::EUIDImageSubClassType::MINIGAME_CIRCLE_BG:
				case DTO::EUIDImageSubClassType::MINIGAME_CIRCLE_INNER_BG:
				case DTO::EUIDImageSubClassType::MINIGAME_CIRCLE_TIMER_BG:
				case DTO::EUIDImageSubClassType::MINIGAME_CIRCLE_POINT:
				case DTO::EUIDImageSubClassType::MINIGAME_CIRCLE_POINT_BG:
				case DTO::EUIDImageSubClassType::MINIGAME_CIRCLE_POINT_FX:
				case DTO::EUIDImageSubClassType::MINIGAME_CIRCLE_CURSOR:
				case DTO::EUIDImageSubClassType::MINIGAME_CIRCLE_SPACE_ICON:
				case DTO::EUIDImageSubClassType::MINIGAME_CIRCLE_CLEAR_CIRCLE_FX:
					Set_Visible();
					break;
				}
			}));

	m_vecEventHandles.push_back(
		m_pGameInstance->Subscribe<MINIGAME_CIRCLE_CLEAR>([this]()
			{
				switch (m_eDImageSubClass)
				{
				case DTO::EUIDImageSubClassType::MINIGAME_CIRCLE_BG:
					m_pGameInstance->Deactive_TimeStop();
				case DTO::EUIDImageSubClassType::MINIGAME_CIRCLE_INNER_BG:
				case DTO::EUIDImageSubClassType::MINIGAME_CIRCLE_TIMER_BG:
				case DTO::EUIDImageSubClassType::MINIGAME_CIRCLE_TIMER_CIRCLE:
				case DTO::EUIDImageSubClassType::MINIGAME_CIRCLE_POINT:
				case DTO::EUIDImageSubClassType::MINIGAME_CIRCLE_POINT_BG:
				case DTO::EUIDImageSubClassType::MINIGAME_CIRCLE_POINT_FX:
				case DTO::EUIDImageSubClassType::MINIGAME_CIRCLE_CLEAR_POINT:
				case DTO::EUIDImageSubClassType::MINIGAME_CIRCLE_CLEAR_POINT_BG:
				case DTO::EUIDImageSubClassType::MINIGAME_CIRCLE_CLEAR_POINT_BG_S:
				case DTO::EUIDImageSubClassType::MINIGAME_CIRCLE_CLEAR_POINT_FX:
				case DTO::EUIDImageSubClassType::MINIGAME_CIRCLE_CURSOR:
				case DTO::EUIDImageSubClassType::MINIGAME_CIRCLE_KEY_ICON:
				case DTO::EUIDImageSubClassType::MINIGAME_CIRCLE_SPACE_ICON:
				case DTO::EUIDImageSubClassType::MINIGAME_CIRCLE_CLEAR_CIRCLE_FX:
					Set_Invisible();
					break;
				}
			}));

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
			}));
}

void CUIMiniGame_Circle_Image::Tick_By_Type(const _float fTimeDelta)
{
	if (m_pParentCanvasCache->Get_CommonParam_bool()[ALL_POINT_CLEAR_SLOT])
	{
		m_pGameInstance->Broadcast<MINIGAME_CIRCLE_CLEAR>();
		m_pParentCanvasCache->Get_CommonParam_bool_Ref()[ALL_POINT_CLEAR_SLOT] = false;
	}

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
		Tick_For_Point(fTimeDelta);
		break;
	case DTO::EUIDImageSubClassType::MINIGAME_CIRCLE_POINT_BG:
	case DTO::EUIDImageSubClassType::MINIGAME_CIRCLE_POINT_FX:
		if (m_pParentCanvasCache->Get_CommonParam_bool()[m_iNumbering])
		{
			if (!m_isFirstEntered)
			{
				Set_Invisible();
				m_isFirstEntered = true;
				m_pGameInstance->Play_OneShot(0, TO_HASH("UI_MINIGAME_CIRCLE"), 0.5f);
			}
		}
		break;
	case DTO::EUIDImageSubClassType::MINIGAME_CIRCLE_CLEAR_POINT:
	case DTO::EUIDImageSubClassType::MINIGAME_CIRCLE_CLEAR_POINT_BG:
	case DTO::EUIDImageSubClassType::MINIGAME_CIRCLE_CLEAR_POINT_BG_S:
	case DTO::EUIDImageSubClassType::MINIGAME_CIRCLE_CLEAR_POINT_FX:
		if (m_pParentCanvasCache->Get_CommonParam_bool()[m_iNumbering])
		{
			if (!m_isFirstEntered)
			{
				Set_Visible();
				m_isFirstEntered = true;
			}
		}
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

void CUIMiniGame_Circle_Image::Tick_For_Point(const _float fTimeDelta)
{
	_float fCurAngle = XMConvertToDegrees(
		m_pParentCanvasCache->Get_CommonParam_float()[CURRENT_RADIAN_SLOT]);

	CLOG_INFO(std::to_wstring(fCurAngle));

	if (fCurAngle >= m_vClampAngle.x && fCurAngle <= m_vClampAngle.y)
	{
		if (KEY_BUTTON_DOWN(DIK_SPACE))
		{
			m_pParentCanvasCache->Get_CommonParam_bool_Ref()[m_iNumbering] = true;
			if (!m_isFirstEntered)
			{
				m_fRotateSpeed = m_pGameInstance->Rand_Float(1.f, 10.f);
				m_pParentCanvasCache->Get_CommonParam_float_Ref()[CURRENT_SPEED_SLOT] = m_fRotateSpeed;

				Set_Invisible();
				m_isFirstEntered = true;
			}
		}
	}
}

void CUIMiniGame_Circle_Image::Tick_For_Cursor(const _float fTimeDelta)
{
	if (m_pParentCanvasCache->Get_CommonParam_bool()[FAILED_TO_CLEAR_SLOT])
		m_fRotateSpeed = 30.f;

	m_fRotateSpeed = m_pParentCanvasCache->Get_CommonParam_float()[CURRENT_SPEED_SLOT];
	m_fRad += fTimeDelta * m_fRotateSpeed;

	if (m_fRad >= (XM_PI * 2.f))
		m_fRad = 0.f;

	// 원래 0도이면 오른쪽에서 시작하는걸 위쪽에서 시작하게 바꾼 것 
	_float fDrawRad = m_fRad - XM_PIDIV2;

	_float x = cos(fDrawRad) * 150.f;
	_float y = sin(fDrawRad) * 150.f;

	m_vMoveOffset = Vec2{ x, y };

	Move_Rotate(fDrawRad - XM_PI);

	m_pParentCanvasCache->Get_CommonParam_float_Ref()[CURRENT_RADIAN_SLOT] = m_fRad;
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
	case DTO::EUIDImageSubClassType::MINIGAME_CIRCLE_CLEAR_POINT_BG:
	case DTO::EUIDImageSubClassType::MINIGAME_CIRCLE_CLEAR_POINT_BG_S:
	case DTO::EUIDImageSubClassType::MINIGAME_CIRCLE_CLEAR_POINT_FX:
		Ready_Fade(0.3f, 0.f, 1.f, m_fDelay);
		Ready_LerpChange(0.1f, 1.f, 2.f, 3.f, m_fDelay, true);
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
	{
		_bool isFade = Tick_Fade(fTimeDelta);
		_bool isChange = Tick_LerpChange(&m_fBrightness, fTimeDelta);

		if (isChange)
		{
			Ready_LerpChange(0.2f, 2.f, 1.f, 3.f, m_fDelay, true);
		}

		if (isFade && isChange)
		{
			for (_uint i = 0; i < 6; ++i)
			{
				if (!m_pParentCanvasCache->Get_CommonParam_bool()[i])
 					return true;
			}
			m_pGameInstance->Play_OneShot(0, TO_HASH("UI_MINIGAME_CIRCLE_CLEAR"), 0.5f);
			m_pParentCanvasCache->Get_CommonParam_bool_Ref()[ALL_POINT_CLEAR_SLOT] = true;
			return true;
		}
	}
	break;
	case DTO::EUIDImageSubClassType::MINIGAME_CIRCLE_CLEAR_POINT_BG:
	case DTO::EUIDImageSubClassType::MINIGAME_CIRCLE_CLEAR_POINT_BG_S:
	case DTO::EUIDImageSubClassType::MINIGAME_CIRCLE_CLEAR_POINT_FX:
	{
		_bool isFade = Tick_Fade(fTimeDelta);
		_bool isChange = Tick_LerpChange(&m_fBrightness, fTimeDelta);
		
		if (isChange)
		{
			Ready_LerpChange(0.2f, 2.f, 1.f, 3.f, m_fDelay, true);
		}

		if (isFade && isChange)
		{
			return true;
		}
	}
		break;

	case DTO::EUIDImageSubClassType::MINIGAME_CIRCLE_CLEAR_CIRCLE_FX:
	{
		m_fAlpha_Ratio = 0.f;
		return true;
	}
		break;
	}

	return false;
}

void CUIMiniGame_Circle_Image::Initialize_InVisible_Event()
{
	switch (m_eDImageSubClass)
	{
	case DTO::EUIDImageSubClassType::MINIGAME_CIRCLE_BG:
	case DTO::EUIDImageSubClassType::MINIGAME_CIRCLE_INNER_BG:
	case DTO::EUIDImageSubClassType::MINIGAME_CIRCLE_TIMER_BG:
		Ready_Fade(0.5f, 1.f, 0.f, 0.3f);
		Ready_LerpChange(0.5f, 1.f, 2.f, 3.f, 0.3f, true);
		switch (m_iNumbering)
		{
		case 0: // 왼쪽 아래
			Ready_Lerp_Movement(Vec2{ 0.f, 0.f }, Vec2{ m_fWidth * 0.5f, (-m_fHeight * 0.5f) }, 0.5f, 3.f, 0.3f, true);
			break;
		case 1: // 왼쪽 위
			Ready_Lerp_Movement(Vec2{ 0.f, 0.f }, Vec2{ m_fWidth * 0.5f, m_fHeight * 0.5f }, 0.5f, 3.f, 0.3f, true);
			break;
		case 2: // 오른쪽 위
			Ready_Lerp_Movement(Vec2{ 0.f, 0.f }, Vec2{ (-m_fWidth * 0.5f), m_fHeight * 0.5f }, 0.5f, 3.f, 0.3f, true);
			break;
		case 3: // 오른쪽 아래
			Ready_Lerp_Movement(Vec2{ 0.f, 0.f }, Vec2{ (-m_fWidth * 0.5f), (-m_fHeight * 0.5f) }, 0.5f, 3.f, 0.3f, true);
			break;
		}
		break;
	case DTO::EUIDImageSubClassType::MINIGAME_CIRCLE_TIMER_CIRCLE:
		Ready_Fade(0.5f, 1.f, 0.f, 0.3f);
		Ready_LerpChange(0.5f, 1.f, 2.f, 3.f, 0.3f, true);

		m_vColorTint = Vec4{ 1.f, 0.97f, 0.8f, 1.f };
		m_vGradiantColorTint = Vec4{ 1.f, 0.97f, 0.8f, 1.f };

		switch (m_iNumbering)
		{
		case 0: // 왼쪽 아래
			Ready_Lerp_Movement(Vec2{ 0.f, 0.f }, Vec2{ m_fWidth * 0.5f, (-m_fHeight * 0.5f) }, 0.5f, 3.f, 0.3f, true);
			break;
		case 1: // 왼쪽 위
			Ready_Lerp_Movement(Vec2{ 0.f, 0.f }, Vec2{ m_fWidth * 0.5f, m_fHeight * 0.5f }, 0.5f, 3.f, 0.3f, true);
			break;
		case 2: // 오른쪽 위
			Ready_Lerp_Movement(Vec2{ 0.f, 0.f }, Vec2{ (-m_fWidth * 0.5f), m_fHeight * 0.5f }, 0.5f, 3.f, 0.3f, true);
			break;
		case 3: // 오른쪽 아래
			Ready_Lerp_Movement(Vec2{ 0.f, 0.f }, Vec2{ (-m_fWidth * 0.5f), (-m_fHeight * 0.5f) }, 0.5f, 3.f, 0.3f, true);
			break;
		}
		break;

	case DTO::EUIDImageSubClassType::MINIGAME_CIRCLE_CURSOR:
	case DTO::EUIDImageSubClassType::MINIGAME_CIRCLE_SPACE_ICON:
		Ready_Fade(0.5f, 1.f, 0.f, m_fDelay);
		Ready_LerpChange(0.5f, 1.f, 2.f, 3.f, m_fDelay, true);
		break;
	case DTO::EUIDImageSubClassType::MINIGAME_CIRCLE_KEY_ICON:
	{
		Ready_LerpChange(0.5f, 1.f, 2.f, 3.f, m_fDelay, true);
		Ready_Fade(0.5f, 1.f, 0.f, m_fDelay);

		m_vColorTint = Vec4{ 1.f, 0.97f, 0.8f, 1.f };
		m_vGradiantColorTint = Vec4{ 1.f, 0.97f, 0.8f, 1.f };

		m_fMiniGameCircle_TimeAcc = 0.f;
		m_fMiniGameCircle_Duration = 0.5f;

	}
	break;
	case DTO::EUIDImageSubClassType::MINIGAME_CIRCLE_POINT:
	case DTO::EUIDImageSubClassType::MINIGAME_CIRCLE_POINT_BG:
	case DTO::EUIDImageSubClassType::MINIGAME_CIRCLE_POINT_FX:
		Ready_Fade(0.5f, 0.f, 0.f, m_fDelay);
		break;

	case DTO::EUIDImageSubClassType::MINIGAME_CIRCLE_CLEAR_POINT:
	{
		// 디버그 편하게 하려고 
		Ready_Fade(0.3f, 1.f, 0.f, m_fDelay);
		Ready_LerpChange(0.3f, 1.f, 3.f, 3.f, m_fDelay);
	}
	break;
	case DTO::EUIDImageSubClassType::MINIGAME_CIRCLE_CLEAR_POINT_BG:
	case DTO::EUIDImageSubClassType::MINIGAME_CIRCLE_CLEAR_POINT_BG_S:
	case DTO::EUIDImageSubClassType::MINIGAME_CIRCLE_CLEAR_POINT_FX:
		Ready_Fade(0.3f, 1.f, 0.f, m_fDelay);
		Ready_LerpChange(0.3f, 1.f, 3.f, 3.f, m_fDelay);
		break;

	case DTO::EUIDImageSubClassType::MINIGAME_CIRCLE_CLEAR_CIRCLE_FX:
	{
		Ready_Fade(0.5f, 1.f, 0.f, m_fDelay);
		Ready_LerpChange(0.5f, 1.f, 5.f, 3.f, m_fDelay, true);
		m_fProgress_Ratio = 1.f;
		m_fMiniGameCircle_TimeAcc = 0.f;
		m_fMiniGameCircle_Duration = 0.5f;
	}
		break;
	}
}

_bool CUIMiniGame_Circle_Image::Tick_InVisible_Event(const _float fTimeDelta)
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
			Set_Active(false);
			return true;
		}
	}
	break;
	case DTO::EUIDImageSubClassType::MINIGAME_CIRCLE_CURSOR:
	case DTO::EUIDImageSubClassType::MINIGAME_CIRCLE_SPACE_ICON:
	{
		_bool isFade = Tick_Fade(fTimeDelta);
		_bool isChange = Tick_LerpChange(&m_fScale, fTimeDelta);

		if (isFade && isChange)
		{
			m_fRad = 0.f;
			Set_Active(false);
			return true;
		}
	}
	break;
	case DTO::EUIDImageSubClassType::MINIGAME_CIRCLE_KEY_ICON:
	{
		_bool isFade = Tick_Fade(fTimeDelta);
		_bool isChange = Tick_LerpChange(&m_fScale, fTimeDelta);

		m_fMiniGameCircle_TimeAcc += fTimeDelta;
		_float fRad = std::lerp(0.f, XM_PIDIV2, m_fMiniGameCircle_TimeAcc / m_fMiniGameCircle_Duration);
		Move_Rotate(fRad);

		if (isFade && isChange)
		{
			m_fRad = 0.f;
			Set_Active(false);
			return true;
		}
	}
	break;
	case DTO::EUIDImageSubClassType::MINIGAME_CIRCLE_POINT:
	{
		_bool isFade = Tick_Fade(fTimeDelta);

		if (isFade)
		{
			Set_Active(false);
			return true;
		}
	}
	break;
	case DTO::EUIDImageSubClassType::MINIGAME_CIRCLE_POINT_BG:
	case DTO::EUIDImageSubClassType::MINIGAME_CIRCLE_POINT_FX:
	{
		_bool isFade = Tick_Fade(fTimeDelta);

		if (isFade)
		{
			Set_Active(false);
			return true;
		}
	}
	break;
	case DTO::EUIDImageSubClassType::MINIGAME_CIRCLE_CLEAR_POINT:
	{
		_bool isFade = Tick_Fade(fTimeDelta);
		_bool isChange = Tick_LerpChange(&m_fBrightness, fTimeDelta);

		if (isFade && isChange)
		{
			Set_Active(false);
			return true;
		}
	}
	break;
	case DTO::EUIDImageSubClassType::MINIGAME_CIRCLE_CLEAR_POINT_BG:
	case DTO::EUIDImageSubClassType::MINIGAME_CIRCLE_CLEAR_POINT_BG_S:
	case DTO::EUIDImageSubClassType::MINIGAME_CIRCLE_CLEAR_POINT_FX:
	{
		_bool isFade = Tick_Fade(fTimeDelta);
		_bool isChange = Tick_LerpChange(&m_fBrightness, fTimeDelta);

		if (isFade && isChange)
		{
			Set_Active(false);
			return true;
		}
	}
		break;
	case DTO::EUIDImageSubClassType::MINIGAME_CIRCLE_CLEAR_CIRCLE_FX:
	{
		_bool isFade = Tick_Fade(fTimeDelta);
		_bool isChange = Tick_LerpChange(&m_fScale, fTimeDelta);

		_bool isProgress = { false };
		m_fMiniGameCircle_TimeAcc += fTimeDelta;
		m_fProgress_Ratio = std::lerp(1.f, 0.f, m_fMiniGameCircle_TimeAcc / m_fMiniGameCircle_Duration);
		if (m_fProgress_Ratio <= 0.f)
			isProgress = true;

		if (isFade && isChange && isProgress)
		{
			Set_Active(false);
			return true;
		}

	}
		break;
	}

	return false;
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
