#include "pch.h"
#include "UIBossAction_Image.h"
#include "Client_Defines.h"
#include "Client_EventDefine.h"
//=================
// Component
//=================
#include "Texture.h"
#include "Shader.h"
#include "VIBuffer_Rect_Tex.h"
#include "GameInstance.h"

CUIBossAction_Image::CUIBossAction_Image(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	:CUIDynamic_Image(pDevice, pDeviceContext)
{
}

CUIBossAction_Image::CUIBossAction_Image(const CUIBossAction_Image& rhs)
	:CUIDynamic_Image(rhs)
{
}

HRESULT CUIBossAction_Image::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIBossAction_Image::Initialize(void* pArg)
{
	BOSS_ACTION_IMAGE_DESC* pDesc = static_cast<BOSS_ACTION_IMAGE_DESC*>(pArg);
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;
	if (FAILED(Ready_Components(pDesc)))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIBossAction_Image::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;

	if (FAILED(Attach_Personal_Info()))
		return E_FAIL;

	return S_OK;
}

void CUIBossAction_Image::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CUIBossAction_Image::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
}

void CUIBossAction_Image::Update_Late(const _float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);
	Tick_By_Type(fTimeDelta);
}

void CUIBossAction_Image::Ready_Before_Render(const _float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);
}

HRESULT CUIBossAction_Image::Render()
{
	if (!m_isVisible)
		return S_OK;
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;
	if (FAILED(Super::Render()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIBossAction_Image::Ready_Components(BOSS_ACTION_IMAGE_DESC* pDesc)
{
	if (FAILED(Super::Ready_Components(pDesc)))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIBossAction_Image::Bind_ShaderResources()
{
	CShader* pShader = Get_Component<CShader>();
	if (FAILED(Get_Component<CTransform>()->Bind_ShaderResource(pShader)))
		return E_FAIL;

	if (FAILED(Super::Bind_ShaderResources()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIBossAction_Image::Attach_Personal_Info()
{
	return S_OK;
}

void CUIBossAction_Image::Tick_By_Type(const _float fTimeDelta)
{
}

void CUIBossAction_Image::Bind_Events()
{
	switch (m_eDImageSubClass)
	{
	case DTO::EUIDImageSubClassType::BOSS_CIVILA_ACTION_BEGIN:
		break;
	case DTO::EUIDImageSubClassType::BOSS_CIVILA_ACTION_ICON:
		m_vecEventHandles.push_back(
			m_pGameInstance->Subscribe<XIBILA_BOSS_ACTION_ON>([this]()
				{
					this->Set_Visible();
				}));
		m_vecEventHandles.push_back(
			m_pGameInstance->Subscribe<XIBILA_BOSS_ACTION_OFF>([this]()
				{
					this->Set_Invisible();
				}));
		break;
	case DTO::EUIDImageSubClassType::BOSS_CIVILA_ACTION_TOP_BG:
		m_vecEventHandles.push_back(
			m_pGameInstance->Subscribe<CINEMATIC_START>([this]()
				{
					this->Set_Visible();
				}));
		m_vecEventHandles.push_back(
			m_pGameInstance->Subscribe<CINEMATIC_END>([this]()
				{
					this->Set_Invisible();
				}));
		break;
	case DTO::EUIDImageSubClassType::BOSS_CIVILA_ACTION_BOTTOM_BG:
		m_vecEventHandles.push_back(
			m_pGameInstance->Subscribe<CINEMATIC_START>([this]()
				{
					this->Set_Visible();
				}));
		m_vecEventHandles.push_back(
			m_pGameInstance->Subscribe<CINEMATIC_END>([this]()
				{
					this->Set_Invisible();
				}));
		break;
	case DTO::EUIDImageSubClassType::BOSS_CIVILA_ACTION_END:
		break;
	}
}

void CUIBossAction_Image::Initialize_Visible_Event()
{
	switch (m_eDImageSubClass)
	{
	case DTO::EUIDImageSubClassType::BOSS_CIVILA_ACTION_BEGIN:
		break;
	case DTO::EUIDImageSubClassType::BOSS_CIVILA_ACTION_ICON:
		Ready_LerpChange(1.5f, 1.f, 0.f, 0.7f, m_fDelay);
		Ready_Fade(0.5f, 0.f, 1.f, m_fDelay);
		break;
	case DTO::EUIDImageSubClassType::BOSS_CIVILA_ACTION_TOP_BG:
		Ready_Lerp_Movement(Vec2{ 0.f, -100.f }, Vec2{ 0.f, 0.f }, 1.5f, 0.7f, m_fDelay);
		Ready_Fade(0.5f, 0.f, 1.f, m_fDelay);
		break;
	case DTO::EUIDImageSubClassType::BOSS_CIVILA_ACTION_BOTTOM_BG:
		Ready_Lerp_Movement(Vec2{ 0.f, 100.f }, Vec2{ 0.f, 0.f }, 1.5f, 0.7f, m_fDelay);
		Ready_Fade(0.5f, 0.f, 1.f, m_fDelay);
		break;
	case DTO::EUIDImageSubClassType::BOSS_CIVILA_ACTION_END:
		break;
	}
}

_bool CUIBossAction_Image::Tick_Visible_Event(const _float fTimeDelta)
{
	switch (m_eDImageSubClass)
	{
	case DTO::EUIDImageSubClassType::BOSS_CIVILA_ACTION_BEGIN:
		break;
	case DTO::EUIDImageSubClassType::BOSS_CIVILA_ACTION_ICON:
	{
		_bool is = Tick_LerpChange(&m_fProgress_Ratio, fTimeDelta);
		_bool isFade = Tick_Fade(fTimeDelta);

		if (is && isFade)
		{
			m_isFin_Event = true;
			return true;
		}
	}
		break;
	case DTO::EUIDImageSubClassType::BOSS_CIVILA_ACTION_TOP_BG:
	{
		_bool isMove = Tick_Lerp_Movement(fTimeDelta);
		_bool isFade = Tick_Fade(fTimeDelta);
		if (isMove && isFade)
		{
			m_isFin_Event = true;
			return true;
		}
	}
		break;
	case DTO::EUIDImageSubClassType::BOSS_CIVILA_ACTION_BOTTOM_BG:
	{
		_bool isMove = Tick_Lerp_Movement(fTimeDelta);
		_bool isFade = Tick_Fade(fTimeDelta);
		if (isMove && isFade)
		{
			m_isFin_Event = true;
			return true;
		}
	}
		break;
	case DTO::EUIDImageSubClassType::BOSS_CIVILA_ACTION_END:
		break;
	}
	return false;
}

void CUIBossAction_Image::Initialize_InVisible_Event()
{
	switch (m_eDImageSubClass)
	{
	case DTO::EUIDImageSubClassType::BOSS_CIVILA_ACTION_BEGIN:
		break;
	case DTO::EUIDImageSubClassType::BOSS_CIVILA_ACTION_ICON:
		Ready_LerpChange(1.f, 0.f, 1.f, 0.7f, m_fDelay);
		Ready_Fade(0.5f, 1.f, 0.f, m_fDelay);
		break;
	case DTO::EUIDImageSubClassType::BOSS_CIVILA_ACTION_TOP_BG:
		Ready_Lerp_Movement(Vec2{ 0.f, 0.f }, Vec2{ 0.f, -100.f }, 1.f, 0.7f, m_fDelay);
		Ready_Fade(0.5f, 1.f, 0.f, m_fDelay);
		break;
	case DTO::EUIDImageSubClassType::BOSS_CIVILA_ACTION_BOTTOM_BG:
		Ready_Lerp_Movement(Vec2{ 0.f, 0.f}, Vec2{ 0.f, 100.f }, 1.f, 0.7f, m_fDelay);
		Ready_Fade(0.5f, 1.f, 0.f, m_fDelay);
		break;
	case DTO::EUIDImageSubClassType::BOSS_CIVILA_ACTION_END:
		break;
	}
}

_bool CUIBossAction_Image::Tick_InVisible_Event(const _float fTimeDelta)
{
	switch (m_eDImageSubClass)
	{
	case DTO::EUIDImageSubClassType::BOSS_CIVILA_ACTION_BEGIN:
		break;
	case DTO::EUIDImageSubClassType::BOSS_CIVILA_ACTION_ICON:
	{
		_bool is = Tick_LerpChange(&m_fProgress_Ratio, fTimeDelta);
		_bool isFade = Tick_Fade(fTimeDelta);

		if (is && isFade)
		{
			m_isFin_Event = true;
			return true;
		}
	}
		break;
	case DTO::EUIDImageSubClassType::BOSS_CIVILA_ACTION_TOP_BG:
	{
		_bool isMove = Tick_Lerp_Movement(fTimeDelta);
		_bool isFade = Tick_Fade(fTimeDelta);
		if (isMove && isFade)
		{
			m_isFin_Event = true;
			return true;
		}
	}
		break;
	case DTO::EUIDImageSubClassType::BOSS_CIVILA_ACTION_BOTTOM_BG:
	{
		_bool isMove = Tick_Lerp_Movement(fTimeDelta);
		_bool isFade = Tick_Fade(fTimeDelta);
		if (isMove && isFade)
		{
			m_isFin_Event = true;
			return true;
		}
	}
		break;
	}
	return false;
}

CUIBossAction_Image* CUIBossAction_Image::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CUIBossAction_Image* pInstance = new CUIBossAction_Image(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CUIBossAction_Image::Create, Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUIBossAction_Image::Clone(void* pArg)
{
	CUIBossAction_Image* pInstance = new CUIBossAction_Image(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CUIBossAction_Image::Clone, Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUIBossAction_Image::Free()
{
	Super::Free();
}
