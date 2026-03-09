#include "pch.h"
#include "UIWeakness_Image.h"
#include "Client_Defines.h"
#include "Client_EventDefine.h"
//=================
// Component
//=================
#include "MyStat.h"
#include "WorldUI_Component.h"
#include "Texture.h"
#include "Shader.h"
#include "VIBuffer_Rect_Tex.h"
#include "UI_Manager.h"
#include "GameInstance.h"

CUIWeakness_Image::CUIWeakness_Image(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	:CUIDynamic_Image(pDevice, pDeviceContext)
{
}

CUIWeakness_Image::CUIWeakness_Image(const CUIWeakness_Image& rhs)
	:CUIDynamic_Image(rhs)
{
}

HRESULT CUIWeakness_Image::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIWeakness_Image::Initialize(void* pArg)
{
	WEAKNESS_IMAGE_DESC* pDesc = static_cast<WEAKNESS_IMAGE_DESC*>(pArg);

   	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;
	if (FAILED(Ready_Components(pDesc)))
		return E_FAIL;

	m_vOriginSize = {m_fWidth, m_fHeight};
	return S_OK;
}

HRESULT CUIWeakness_Image::Attach_Personal_Info()
{
	switch (m_eDImageSubClass)
	{
	case DTO::EUIDImageSubClassType::BATTLE_WEAKNESS_BEGIN:
		break;
	case DTO::EUIDImageSubClassType::BATTLE_WEAKNESS_EYEICON:
		break;
	case DTO::EUIDImageSubClassType::BATTLE_WEAKNESS_EYEICON_BG:
		break;
	case DTO::EUIDImageSubClassType::BATTLE_WEAKNESS_EYEICON_BG_FX:
	{
		m_vNoiseUVScale = Vec2{ 1.5f, 1.5f };
		m_vNoiseUVScroll = Vec2{ 0.f, -0.5f };
		m_fGlowDistort = 0.02;
		m_fGlowPulseSpeed = 0.f;
		m_fGlowIntensity = 1.6f;
	}
	break;
	case DTO::EUIDImageSubClassType::BSTTLE_WEAKNESS_END:
		break;
	case DTO::EUIDImageSubClassType::END:
	default:
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CUIWeakness_Image::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;

	if (FAILED(Attach_Personal_Info()))
		return E_FAIL;
	return S_OK;
}

void CUIWeakness_Image::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CUIWeakness_Image::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);

	Tick_By_Type(fTimeDelta);
}

void CUIWeakness_Image::Update_Late(const _float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);
}

void CUIWeakness_Image::Ready_Before_Render(const _float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);
}

HRESULT CUIWeakness_Image::Render()
{
	if (!m_isVisible)
		return S_OK;
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;
	if (FAILED(Super::Render()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIWeakness_Image::Ready_Components(WEAKNESS_IMAGE_DESC* pDesc)
{
	if (FAILED(Super::Ready_Components(pDesc)))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIWeakness_Image::Bind_ShaderResources()
{
	CShader* pShader = Get_Component<CShader>();
	if (FAILED(Get_Component<CTransform>()->Bind_ShaderResource(pShader)))
		return E_FAIL;

	if (m_eDImageSubClass == DTO::EUIDImageSubClassType::BATTLE_WEAKNESS_EYEICON_BG_FX)
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

void CUIWeakness_Image::Bind_Events()
{
	m_tEventHandle0 = (m_pGameInstance->Subscribe<BOSS_GROGGY>([this]()
		{
			this->Set_Visible();
		})
	);


	m_vecEventHandles.push_back(m_pUIManager->Get_UIEvents().Subscribe([this](const UIEVENT_DESC& Desc)
		{
			if (EUIEventID::WEAKNESS_FIN == Desc.eEventID)
				this->Set_Invisible();
		})
	);

	switch (m_eDImageSubClass)
	{
	case DTO::EUIDImageSubClassType::BATTLE_WEAKNESS_BEGIN:
		break;
	case DTO::EUIDImageSubClassType::BATTLE_WEAKNESS_EYEICON:
		break;
	case DTO::EUIDImageSubClassType::BATTLE_WEAKNESS_EYEICON_BG:
		break;
	case DTO::EUIDImageSubClassType::BATTLE_WEAKNESS_EYEICON_BG_FX:
		break;
	case DTO::EUIDImageSubClassType::BSTTLE_WEAKNESS_END:
		break;
	}
}

void CUIWeakness_Image::Tick_By_Type(const _float fTimeDelta)
{
	switch (m_eDImageSubClass)
	{
	case DTO::EUIDImageSubClassType::BATTLE_WEAKNESS_BEGIN:
		break;
	case DTO::EUIDImageSubClassType::BATTLE_WEAKNESS_EYEICON:
		break;
	case DTO::EUIDImageSubClassType::BATTLE_WEAKNESS_EYEICON_BG:
		break;
	case DTO::EUIDImageSubClassType::BATTLE_WEAKNESS_EYEICON_BG_FX:
		m_fTime += fTimeDelta;
		break;
	case DTO::EUIDImageSubClassType::BSTTLE_WEAKNESS_END:
		break;
	}
}

void CUIWeakness_Image::Initialize_Visible_Event()
{
	switch (m_eDImageSubClass)
	{
	case DTO::EUIDImageSubClassType::BATTLE_WEAKNESS_BEGIN:
		break;
	case DTO::EUIDImageSubClassType::BATTLE_WEAKNESS_EYEICON:
		Ready_LerpChange(1.f, 0.f, 1.f, 3.f, 0.3f);
		m_fAlpha_Ratio = 0.f;
		m_fWidth = 0.f;
		m_fHeight = 0.f;
		Ready_Fade(0.5f, 0.f, 1.f, 0.5f);
		break;
	case DTO::EUIDImageSubClassType::BATTLE_WEAKNESS_EYEICON_BG:
		Ready_LerpChange(0.5f, 0.f, 1.f, 1.f, m_fDelay);
		m_fAlpha_Ratio = 0.f;
		m_fHeight = 0.f;
		Ready_Fade(1.f, 0.f, 1.f, m_fDelay);
		break;
	case DTO::EUIDImageSubClassType::BATTLE_WEAKNESS_EYEICON_BG_FX:
		Ready_Fade(1.f, 0.f, 1.f, 0.5f);
		break;
	case DTO::EUIDImageSubClassType::BSTTLE_WEAKNESS_END:
		break;
	}
}

void CUIWeakness_Image::Initialize_InVisible_Event()
{
	switch (m_eDImageSubClass)
	{
	case DTO::EUIDImageSubClassType::BATTLE_WEAKNESS_BEGIN:
		break;
	case DTO::EUIDImageSubClassType::BATTLE_WEAKNESS_EYEICON:
		Ready_LerpChange(0.5f, 1.f, 0.f, 3.f, 1.f);
		Ready_Fade(0.5f, 1.f, 0.f, 1.f);
		break;
	case DTO::EUIDImageSubClassType::BATTLE_WEAKNESS_EYEICON_BG:
		Ready_LerpChange(0.5f, 1.f, 0.f, 1.f, 1.f);
		Ready_Fade(1.f, 1.f, 0.f, 1.f);
		break;
	case DTO::EUIDImageSubClassType::BATTLE_WEAKNESS_EYEICON_BG_FX:
		Ready_Fade(0.3f, 1.f, 0.f, 1.f);
		break;
	case DTO::EUIDImageSubClassType::BSTTLE_WEAKNESS_END:
		break;
	}
}

_bool CUIWeakness_Image::Tick_Visible_Event(const _float fTimeDelta)
{
	switch (m_eDImageSubClass)
	{
	case DTO::EUIDImageSubClassType::BATTLE_WEAKNESS_BEGIN:
		break;
	case DTO::EUIDImageSubClassType::BATTLE_WEAKNESS_EYEICON:
	{
		_bool is = Tick_LerpChange(&m_fScale, fTimeDelta);
		_bool isFade = Tick_Fade(fTimeDelta);

		if (is && isFade)
		{
			UIEVENT_DESC Desc = {};
			Desc.eEventID = EUIEventID::WEAKNESS_FIN;
			m_pUIManager->Get_UIEvents().Broadcast(Desc);

			return true;
		}
	}
	break;
	case DTO::EUIDImageSubClassType::BATTLE_WEAKNESS_EYEICON_BG:
	{
		_bool is = Tick_LerpChange(&m_fScale, fTimeDelta);
		_bool isFade = Tick_Fade(fTimeDelta);

		if (is && isFade)
		{
			return true;
		}
	}
	break;
	case DTO::EUIDImageSubClassType::BATTLE_WEAKNESS_EYEICON_BG_FX:
		return Tick_Fade(fTimeDelta);
		break;
	case DTO::EUIDImageSubClassType::BSTTLE_WEAKNESS_END:
		break;
	}
	return false;
}

_bool CUIWeakness_Image::Tick_InVisible_Event(const _float fTimeDelta)
{
	switch (m_eDImageSubClass)
	{
	case DTO::EUIDImageSubClassType::BATTLE_WEAKNESS_BEGIN:
		break;
	case DTO::EUIDImageSubClassType::BATTLE_WEAKNESS_EYEICON:
	{
		_bool is = Tick_LerpChange(&m_fScale, fTimeDelta);
		_bool isFade = Tick_Fade(fTimeDelta);

		if (is && isFade)
		{
			m_fWidth = 0.f;
			m_fHeight = 0.f;
			return true;
		}
		m_fWidth = m_vOriginSize.x * m_fScale;
		m_fHeight = m_vOriginSize.y * m_fScale;
	}
	break;
	case DTO::EUIDImageSubClassType::BATTLE_WEAKNESS_EYEICON_BG:
	{
		_bool is = Tick_LerpChange(&m_fScale, fTimeDelta);
		_bool isFade = Tick_Fade(fTimeDelta);

		if (is && isFade)
		{
			return true;
		}
	}
	break;
	case DTO::EUIDImageSubClassType::BATTLE_WEAKNESS_EYEICON_BG_FX:
		return Tick_Fade(fTimeDelta);
		break;
	case DTO::EUIDImageSubClassType::BSTTLE_WEAKNESS_END:
		break;
	}
	return false	;
}

CUIWeakness_Image* CUIWeakness_Image::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CUIWeakness_Image* pInstance = new CUIWeakness_Image(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CUIWeakness_Image::Create, Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUIWeakness_Image::Clone(void* pArg)
{
	CUIWeakness_Image* pInstance = new CUIWeakness_Image(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CUIWeakness_Image::Clone, Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUIWeakness_Image::Free()
{
	Super::Free();

	m_pGameInstance->Unsubscribe<BOSS_GROGGY>(m_tEventHandle0);
}
