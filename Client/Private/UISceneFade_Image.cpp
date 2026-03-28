#include "pch.h"
#include "UISceneFade_Image.h"
#include "Client_Defines.h"

#include "Level_Loading.h"
#include "Canvas.h"
#include "Texture.h"
#include "Shader.h"
#include "VIBuffer_Rect_Tex.h"
#include "UI_Manager.h"
#include "GameInstance.h"

CUISceneFade_Image::CUISceneFade_Image(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	:CUIDynamic_Image(pDevice, pDeviceContext)
{
}

CUISceneFade_Image::CUISceneFade_Image(const CUISceneFade_Image& rhs)
	:CUIDynamic_Image(rhs)
{
}

HRESULT CUISceneFade_Image::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUISceneFade_Image::Initialize(void* pArg)
{
	SCENEFADE_IMAGE_DESC* pDesc = static_cast<SCENEFADE_IMAGE_DESC*>(pArg);
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;
	if (FAILED(Ready_Components(pDesc)))
		return E_FAIL;
	return S_OK;
}

HRESULT CUISceneFade_Image::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;
	if (FAILED(Attach_Personal_Info()))
		return E_FAIL;
	return S_OK;
}

void CUISceneFade_Image::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CUISceneFade_Image::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
	Tick_By_Type(fTimeDelta);
}

void CUISceneFade_Image::Update_Late(const _float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);
}

void CUISceneFade_Image::Ready_Before_Render(const _float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);

	m_pGameInstance->Push_RenderObject(RENDER_CATEGORY::BLENDUI, this);
}

HRESULT CUISceneFade_Image::Render()
{
	if (!m_isVisible)
		return S_OK;
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;
	if (FAILED(Super::Render()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUISceneFade_Image::Ready_Components(SCENEFADE_IMAGE_DESC* pDesc)
{
	if (FAILED(Super::Ready_Components(pDesc)))
		return E_FAIL;
	return S_OK;
}

HRESULT CUISceneFade_Image::Bind_ShaderResources()
{
	CShader* pShader = Get_Component<CShader>();
	if (FAILED(Get_Component<CTransform>()->Bind_ShaderResource(pShader)))
		return E_FAIL;

	if (FAILED(Super::Bind_ShaderResources()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUISceneFade_Image::Attach_Personal_Info()
{
	if (m_isSpawned)
	{
		m_isVisible = true;
		m_isSpawned = false;
	}
	return S_OK;
}

void CUISceneFade_Image::Bind_Events()
{
	Super::Bind_Events();
}

void CUISceneFade_Image::Tick_By_Type(const _float fTimeDelta)
{
	if (m_pUIManager->Get_ClearDelay())
	{
		m_fSceneFade_DelayTimeAcc = m_fSceneFade_DelayTime;
		m_pUIManager->Set_ClearDelay(false);
	}

	m_fSceneFade_DelayTimeAcc += fTimeDelta;
	if (m_fSceneFade_DelayTimeAcc <= m_fSceneFade_DelayTime)
		return;

	_bool isFade = Tick_Fade(fTimeDelta);
	_bool isLerpChange = Tick_LerpChange(&m_fProgress_Ratio, fTimeDelta);

	if (isFade && isLerpChange)
	{
		m_fEndDelayTimeAcc += fTimeDelta;
		if (m_fEndDelay < m_fEndDelayTimeAcc)
		{
			m_fEndDelayTimeAcc = 0.f;
			Set_Active(false);

			if (m_isChangeLevel)
			{
				m_pGameInstance->Active_TimeStop();
				m_pGameInstance->Request_ChangeLevel(ENUM_TO_UINT(ELevelType::LOADING),
					CLevel_Loading::Create(m_pDevice, m_pDeviceContext, m_eNextLevelID));
				return;
			}

			Request_SetDead();
			m_pGameInstance->Deactive_TimeStop();
		}
	}
}

void CUISceneFade_Image::Initialize_Visible_Event()
{
}

_bool CUISceneFade_Image::Tick_Visible_Event(const _float fTimeDelta)
{
	return true;
}

void CUISceneFade_Image::Initialize_InVisible_Event()
{
}

_bool CUISceneFade_Image::Tick_InVisible_Event(const _float fTimeDelta)
{
	return true;
}

HRESULT CUISceneFade_Image::Spawn_FromPool(void* pArg)
{
	if (FAILED(Super::Spawn_FromPool(pArg)))
		return E_FAIL;

	UI_PREFAB_DATA* pDesc = static_cast<UI_PREFAB_DATA*>(pArg);

	if (auto* pLevelFade = std::get_if<UI_LEVEL_FADE_PREFAB_DATA>(&pDesc->Data))
	{
		m_eNextLevelID = pLevelFade->eNextLevelID;
		m_isDeadRequest = false;
		m_fEndDelay = pLevelFade->fEndDelay;

		m_isFadeIn = pLevelFade->isFadeIn;
		m_isChangeLevel = pLevelFade->isChangeLevel;

		m_fSceneFade_DelayTime = pLevelFade->fDelay;
		m_fSceneFade_DelayTimeAcc = 0.f;

		// 점점 보이게 
		if (pLevelFade->isFadeIn)
		{
			Ready_Fade(pLevelFade->fDuration, 1.f, 0.f, 0.f);
			Ready_LerpChange(pLevelFade->fDuration, 0.f, 1.f, pLevelFade->fEaseValue, 0.f, pLevelFade->isEased);
			m_fProgress_Ratio = 0.f;
		}
		// 점점 안보이게
		else
		{
			Ready_Fade(pLevelFade->fDuration, 0.f, 1.f, 0.f);
			Ready_LerpChange(pLevelFade->fDuration, 1.f, 0.f, pLevelFade->fEaseValue, 0.f, pLevelFade->isEased);
			m_fProgress_Ratio = 1.f;
		}
		m_isSpawned = true;
		Set_Active(true);
	}
	return S_OK;
}

HRESULT CUISceneFade_Image::Despawn_FromPool()
{
	if (FAILED(Super::Despawn_FromPool()))
		return E_FAIL;

	m_isVisible = false;
	m_isVisibleTrigger = false;
	m_isPreVisible = false;
	
	return S_OK;
}

CUISceneFade_Image* CUISceneFade_Image::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CUISceneFade_Image* pInstance = new CUISceneFade_Image(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CUISceneFade_Image::Create, Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUISceneFade_Image::Clone(void* pArg)
{
	CUISceneFade_Image* pInstance = new CUISceneFade_Image(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CUISceneFade_Image::Clone, Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUISceneFade_Image::Free()
{
	Super::Free();
}
