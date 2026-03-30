#include "pch.h"
#include "UIQTE_Text.h"
#include "Client_Defines.h"
#include "Client_EventDefine.h"

//=================
// Component
//=================
#include "Texture.h"
#include "Shader.h"
#include "VIBuffer_Rect_Tex.h"
#include "UI_Manager.h"
#include "UIQTE_Manager.h"
#include "GameInstance.h"

CUIQTE_Text::CUIQTE_Text(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	:CUIText(pDevice, pDeviceContext)
{
}

CUIQTE_Text::CUIQTE_Text(const CUIQTE_Text& rhs)
	:CUIText(rhs)
{
}

HRESULT CUIQTE_Text::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIQTE_Text::Initialize(void* pArg)
{
	QTE_TEXT_DESC* pDesc = static_cast<QTE_TEXT_DESC*>(pArg);
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;
	if (FAILED(Ready_Components(pDesc)))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIQTE_Text::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;

	if (FAILED(Attach_Personal_Info()))
		return E_FAIL;

	if (m_isSpawned)
	{
		m_isSpawned = false;
	}
	return S_OK;
}


void CUIQTE_Text::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CUIQTE_Text::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
	Tick_By_Type(fTimeDelta);
}

void CUIQTE_Text::Update_Late(const _float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);
}

void CUIQTE_Text::Ready_Before_Render(const _float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);
}

HRESULT CUIQTE_Text::Render()
{
	if (!m_isVisible)
		return S_OK;
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;
	if (FAILED(Super::Render()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIQTE_Text::Ready_Components(QTE_TEXT_DESC* pDesc)
{
	if (FAILED(Super::Ready_Components(pDesc)))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIQTE_Text::Bind_ShaderResources()
{
	CShader* pShader = Get_Component<CShader>();
	if (FAILED(Get_Component<CTransform>()->Bind_ShaderResource(pShader)))
		return E_FAIL;
	if (FAILED(Super::Bind_ShaderResources()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIQTE_Text::Attach_Personal_Info()
{
	return S_OK;
}

HRESULT CUIQTE_Text::Convert_Stat_To_Text()
{
	return S_OK;
}

void CUIQTE_Text::Bind_Events()
{
	Super::Bind_Events();

	m_vecEventHandles.push_back(
		m_pUIManager->Get_UIEvents().Subscribe([this](const UIEVENT_DESC& Desc)
			{
				if (EUIEventID::QTE_SUCCESS == Desc.eEventID)
				{
					if (Desc.iParam0 == m_iNumbering)
					{
						m_iTimingType = Desc.iParam2;
						this->Set_Visible();

					}
					else if (Desc.iParam1 == m_iNumbering)
					{
					}
				}
			}));
}

void CUIQTE_Text::Tick_By_Type(const _float fTimeDelta)
{
	CUIQTE_Manager::EQTETiming eType = static_cast<CUIQTE_Manager::EQTETiming>(m_iTimingType);

	switch (eType)
	{
	case Client::CUIQTE_Manager::EQTETiming::FAST:
		m_wstrText = L"Fast!";
		break;
	case Client::CUIQTE_Manager::EQTETiming::PERFECT:
		m_wstrText = L"Perfect!";
		break;
	case Client::CUIQTE_Manager::EQTETiming::SLOW:
		m_wstrText = L"Slow!";
		break;
	case Client::CUIQTE_Manager::EQTETiming::FAIL:
		m_wstrText = L"Fail!";
		break;
	}
}

void CUIQTE_Text::Initialize_Visible_Event()
{
	Ready_LerpChange(0.3f, 2.f, 1.f, 2.f, m_fDelay, true);

}

void CUIQTE_Text::Initialize_InVisible_Event()
{
}

_bool CUIQTE_Text::Tick_Visible_Event(const _float fTimeDelta)
{
	_bool isChange = Tick_LerpChange(&m_fScaleOffset, fTimeDelta);

	if (isChange)
	{
		Set_Invisible();
		return true;
	}
	return false;
}

_bool CUIQTE_Text::Tick_InVisible_Event(const _float fTimeDelta)
{
	Request_SetDead();
	return true;
}

HRESULT CUIQTE_Text::Spawn_FromPool(void* pArg)
{
	if (FAILED(Super::Spawn_FromPool(pArg)))
		return E_FAIL;

	UI_PREFAB_DATA* pDesc = static_cast<UI_PREFAB_DATA*>(pArg);
	if (auto* pQTEData = std::get_if<UI_QTE_PREFAB_DATA>(&pDesc->Data))
	{
		m_isDeadRequest = false;
		m_iNumbering = pQTEData->iNodeIndex;
		m_vMoveOffset = pQTEData->vSpawnOffset;
	}

	m_isSpawned = true;
	return S_OK;
}

HRESULT CUIQTE_Text::Despawn_FromPool()
{
	if (FAILED(Super::Despawn_FromPool()))
		return E_FAIL;

	m_isVisible = false;
	m_isVisibleTrigger = false;
	m_isPreVisible = false;
	return S_OK;
}

CUIQTE_Text* CUIQTE_Text::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CUIQTE_Text* pInstance = new CUIQTE_Text(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CUIQTE_Text::Create, Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUIQTE_Text::Clone(void* pArg)
{
	CUIQTE_Text* pInstance = new CUIQTE_Text(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CUIQTE_Text::Clone, Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUIQTE_Text::Free()
{
	Super::Free();
}
