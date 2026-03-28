#include "pch.h"
#include "UIMini_Map.h"
#include "Client_Defines.h"
#include "Client_EventDefine.h"
#include "CameraMan.h"

#include "MainPlayer.h"

//=================
// Component
//=================
#include "Texture.h"
#include "Shader.h"
#include "VIBuffer_Rect_Tex.h"
#include "MyStat.h"
#include "UI_Manager.h"
#include "UIMinimap_Manager.h"
#include "GameInstance.h"

CUIMini_Map::CUIMini_Map(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	:CUIDynamic_Image(pDevice, pDeviceContext)
{
}

CUIMini_Map::CUIMini_Map(const CUIMini_Map& rhs)
	:CUIDynamic_Image(rhs)
{
}

HRESULT CUIMini_Map::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIMini_Map::Initialize(void* pArg)
{
	MINIMAP_DESC* pDesc = static_cast<MINIMAP_DESC*>(pArg);
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;
	if (FAILED(Ready_Components(pDesc)))
		return E_FAIL;

	return S_OK;
}


HRESULT CUIMini_Map::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;

	if (FAILED(Attach_Personal_Info()))
		return E_FAIL;
	return S_OK;
}

void CUIMini_Map::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CUIMini_Map::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);

	if (KEY_BUTTON_DOWN(DIK_1))
		m_beAttackEventTrigger = TRUE;

	if (m_eDImageSubClass == DTO::EUIDImageSubClassType::MINIMAP_CAMERA_SIGHT)
	{
		Matrix CamWorldMat = m_pGameInstance->Get_ViewMatrix().Invert();
		Vec3 vCamLook = CamWorldMat.Backward();
		m_fRadian = CUIMinimap_Manager::GetInstance()->WorldLook_Convert_To_Radian(vCamLook);
		Rotate_Translate_CameraSight();
	}
	else if (m_eDImageSubClass == DTO::EUIDImageSubClassType::MINIMAP_PLAYER_ICON)
	{
		m_fRadian = CUIMinimap_Manager::GetInstance()->WorldLook_Convert_To_Radian(m_pPlayerTransform->Get_Info(TRANSFORM_INFO_STATE::LOOK));
		Rotate_PlayerIcon();
	}
	else if (m_eDImageSubClass == DTO::EUIDImageSubClassType::MINIMAP_BGFRAME)
	{
		if (KEY_BUTTON_HOLD(DIK_A))
			TickRotate(-1, fTimeDelta * 0.8f);
		else if (KEY_BUTTON_HOLD(DIK_D))
			TickRotate(1, fTimeDelta * 0.8f);
		else
			TickRotate(0, fTimeDelta * 0.5f);
	}
	else if (m_eDImageSubClass == DTO::EUIDImageSubClassType::MINIMAP_WARNING_FRAME)
	{
		if (m_beAttackEventTrigger)
		{
			if (m_isAttacked)
			{
				m_vColorTint = Vec4{ 1.f, 1.f, 1.f, 1.f };
				m_vGradiantColorTint = Vec4{ 1.f, 1.f, 1.f, 1.f };
				m_isAttacked = false;
			}
			else
			{
				m_vColorTint = Vec4{ 1.f, 0.f, 0.f, 1.f };
				m_vGradiantColorTint = Vec4{ 1.f, 0.f, 0.f, 1.f };
				m_isAttacked = true;
			}
			m_beAttackEventTrigger = false;
		}

		const _float fMinA = 0.3f;
		const _float fMaxA = 1.0f;
		const _float fSpeed = 1.0f;

		if (m_isPulseDown)
		{
			m_fAlpha_Ratio -= fTimeDelta * fSpeed;
			if (m_fAlpha_Ratio <= fMinA)
			{
				m_fAlpha_Ratio = fMinA;
				m_isPulseDown = FALSE;
			}
		}
		else if (m_eDImageSubClass == DTO::EUIDImageSubClassType::MINIMAP_DIR_ICON)
		{
		}
		else
		{
			m_fAlpha_Ratio += fTimeDelta * fSpeed;
			if (m_fAlpha_Ratio >= fMaxA)
			{
				m_fAlpha_Ratio = fMaxA;
				m_isPulseDown = TRUE;
			}
		}
	}
}

void CUIMini_Map::Update_Late(const _float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);
}

void CUIMini_Map::Ready_Before_Render(const _float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);
}

HRESULT CUIMini_Map::Render()
{
	if (!m_isVisible)
		return S_OK;

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(Super::Render()))
		return E_FAIL;

	return S_OK;
}

void CUIMini_Map::Rotate_Translate_CameraSight()
{
	Get_Component<CTransform>()->Rotation(Vec3{ 0.f, 0.f, -1.f }, m_fRadian);

	const _float c = cosf(m_fRadian);
	const _float s = sinf(m_fRadian);

	Vec2 v = m_vPivotToOrigin;
	Vec2 vRot;
	vRot.x = v.x * c - v.y * s;
	vRot.y = v.x * s + v.y * c;

	m_fX = m_vPivotPos.x + vRot.x;
	m_fY = m_vPivotPos.y + vRot.y;
}

void CUIMini_Map::Rotate_PlayerIcon()
{
	Get_Component<CTransform>()->Rotation(Vec3{ 0.f, 0.f, -1.f }, m_fRadian);
}

_float CUIMini_Map::Clamp(_float v, _float lo, _float hi)
{
	return (v < lo) ? lo : (v > hi) ? hi : v;
}

_float CUIMini_Map::WrapPi(_float a)
{
	const _float PI		= 3.14159265358979323846f;
	const _float TWO_PI = 6.2831853071795864769f;
	while (a > PI) a	-= TWO_PI;
	while (a < -PI) a	+= TWO_PI;
	return a;
}

void CUIMini_Map::TickRotate(_int dir, _float dt)
{
	if (dir != 0)
	{
		m_fOmega += (_float)dir * m_fAcc * dt;
		m_fOmega = Clamp(m_fOmega, -m_fMaxOmega, m_fMaxOmega);
	}
	else
	{
		m_fOmega *= expf(-m_fDrag * dt);
		if (fabsf(m_fOmega) < 0.0005f) m_fOmega = 0.f;
	}
	m_fAngle = WrapPi(m_fAngle + m_fOmega * dt);

	Get_Component<CTransform>()->Rotation(Vec3{ 0.f, 0.f, -1.f }, m_fAngle);
}

HRESULT CUIMini_Map::Ready_Components(MINIMAP_DESC* pDesc)
{
	if (FAILED(Super::Ready_Components(pDesc)))
		return E_FAIL;
	return S_OK;
}
 
HRESULT CUIMini_Map::Bind_ShaderResources()
{
	CShader* pShader = Get_Component<CShader>();
	if (FAILED(Get_Component<CTransform>()->Bind_ShaderResource(pShader)))
		return E_FAIL;
	if (FAILED(Super::Bind_ShaderResources()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIMini_Map::Attach_Personal_Info()
{
	switch (m_eDImageSubClass)
	{
	case DTO::EUIDImageSubClassType::MINIMAP_PLAYER_ICON:
	{
		CGameObject* pResult = m_pGameInstance->Get_GameObject(/* static */ 0, g_wszPlayerLayer, 0);
		if (nullptr == pResult)
			return E_FAIL;
		CMainPlayer* pPlayer = dynamic_cast<CMainPlayer*>(pResult);
		if (nullptr == pPlayer)
			return E_FAIL;

		CTransform* pPlayerTransform = pPlayer->Get_Component<CTransform>();
		if (nullptr == pPlayerTransform)
			return E_FAIL;

		m_pPlayerTransform = pPlayerTransform;
	}
	break;
	case DTO::EUIDImageSubClassType::MINIMAP_CAMERA_SIGHT:
	{
		m_vPivotPos = Vec2{ m_fX, m_fY + m_fHeight * 0.5f };
		m_vOriginPos = Vec2{ m_fX, m_fY };
		m_vPivotToOrigin = m_vOriginPos - m_vPivotPos;
	}
	break;
	case DTO::EUIDImageSubClassType::MINIMAP_BGFRAME:
	{

	}
	break;
	case DTO::EUIDImageSubClassType::MINIMAP_WARNING_FRAME:
	{
		// * 외부 변수 바인딩 *
		m_beAttackEventTrigger;
	}
	break;
	case DTO::EUIDImageSubClassType::MINIMAP_DIR_ICON:
	{

	}
	break;
	case DTO::EUIDImageSubClassType::END:
		break;
	default:
		return E_FAIL;
	}

	return S_OK;
}

void CUIMini_Map::Bind_Events()
{
	Super::Bind_Events();

	m_vecEventHandles.push_back(
		m_pUIManager->Get_UIEvents().Subscribe([this](const UIEVENT_DESC& Desc)
			{
				if (EUIEventID::MENU_CLOSE == Desc.eEventID)
				{
					this->Set_Active(true);
					this->Set_Visible();
				}
			}));
	m_vecEventHandles.push_back(
		m_pUIManager->Get_UIEvents().Subscribe([this](const UIEVENT_DESC& Desc)
			{
				if (EUIEventID::MENU_OPEN == Desc.eEventID)
				{
					this->Set_Invisible();
				}
			}));


	m_vecEventHandles.push_back(
		m_pGameInstance->Subscribe<CINEMATIC_START>(
			[this]()
			{
				this->Set_Invisible();
			}));
	m_vecEventHandles.push_back(
		m_pGameInstance->Subscribe<CINEMATIC_END>([this]()
			{
				this->Set_Active(true);
				this->Set_Visible();
			}));


	m_vecEventHandles.push_back(
		m_pGameInstance->Subscribe<DIALOGUE_BEGIN>([this](_int iId)
			{
				this->Set_Invisible();
			}));
	m_vecEventHandles.push_back(
		m_pGameInstance->Subscribe<DIALOGUE_END>([this]()
			{
				this->Set_Active(true);
				this->Set_Visible();
			}));

	// 패널 Events
	m_vecEventHandles.push_back(
		m_pUIManager->Get_UIEvents().Subscribe([this](const UIEVENT_DESC& Desc)
			{
				if (EUIEventID::TUTORIAL_PANNEL_START == Desc.eEventID)
				{
					this->Set_Invisible();
				}
			}));
	m_vecEventHandles.push_back(
		m_pUIManager->Get_UIEvents().Subscribe([this](const UIEVENT_DESC& Desc)
			{
				if (EUIEventID::TUTORIAL_PANNEL_END == Desc.eEventID)
				{
					this->Set_Visible();
					this->Set_Active(true);
				}
			}));
}

void CUIMini_Map::Initialize_Visible_Event()
{
	m_isFin_Event = false;
	m_isActive = false;
	m_fAlpha_Ratio = 0.f;
	m_fTimeAcc = 0.f;
	Ready_Lerp_Movement(Vec2{ -10.f,0.f }, Vec2{ 10.f, 0.f }, 0.5f, 1.f, 0.f);
}

void CUIMini_Map::Initialize_InVisible_Event()
{
	m_isFin_Event = false;
	m_isActive = false;
	Ready_Fade(0.5f, 1.f, 0.f, 0.f);
	Ready_Lerp_Movement(Vec2{ 0.f,0.f }, Vec2{ -10.f, 0.f }, 0.5f, 1.f, 0.f);
}

_bool CUIMini_Map::Tick_Visible_Event(const _float fTimeDelta)
{
	m_fTimeAcc += fTimeDelta;
	_float t = m_fTimeAcc / 2.f;

	_bool is = Tick_Lerp_Movement(fTimeDelta);
	if (t >= 1.f)
	{
		m_fAlpha_Ratio = 1.f;
		if (is)
		{
			m_isFin_Event = true;
			m_isActive = true;
			return true;
		}
	}

	m_fAlpha_Ratio = t;
	return false;
}

_bool CUIMini_Map::Tick_InVisible_Event(const _float fTimeDelta)
{
	_bool isFade = Tick_Fade(fTimeDelta);
	_bool isLerp = Tick_Lerp_Movement(fTimeDelta);
	if (isFade && isLerp)
	{
		Set_Active(false);
		return true;
	}

	return false;
}

CUIMini_Map* CUIMini_Map::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CUIMini_Map* pInstance = new CUIMini_Map(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CUIMini_Map::Create, Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUIMini_Map::Clone(void* pArg)
{
	CUIMini_Map* pInstance = new CUIMini_Map(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CUIMini_Map::Clone, Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUIMini_Map::Free()
{
	Super::Free();
}
