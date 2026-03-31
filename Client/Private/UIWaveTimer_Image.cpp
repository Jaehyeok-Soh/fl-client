#include "pch.h"
#include "UIWaveTimer_Image.h"
#include "Client_Defines.h"
#include "Client_EventDefine.h"
//=================
// Component
//=================
#include "Canvas.h"
#include "Texture.h"
#include "Shader.h"
#include "VIBuffer_Rect_Tex.h"
#include "UI_Manager.h"
#include "QuestManager.h"
#include "TriggerBox_MonsterWaveSpawner.h"
#include "GameInstance.h"

CUIWaveTimer_Image::CUIWaveTimer_Image(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	:CUIDynamic_Image(pDevice, pDeviceContext)
{
}

CUIWaveTimer_Image::CUIWaveTimer_Image(const CUIWaveTimer_Image& rhs)
	:CUIDynamic_Image(rhs)
{
}

HRESULT CUIWaveTimer_Image::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIWaveTimer_Image::Initialize(void* pArg)
{
	WAVE_TIMER_IMAGE_DESC* pDesc = static_cast<WAVE_TIMER_IMAGE_DESC*>(pArg);
	m_iNumbering = pDesc->iNumbering;
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;
	if (FAILED(Ready_Components(pDesc)))
		return E_FAIL;

	return S_OK;
}


HRESULT CUIWaveTimer_Image::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;
	if (FAILED(Attach_Personal_Info()))
		return E_FAIL;

	return S_OK;
}

void CUIWaveTimer_Image::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CUIWaveTimer_Image::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
	Tick_By_Type(fTimeDelta);
}

void CUIWaveTimer_Image::Update_Late(const _float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);
}

void CUIWaveTimer_Image::Ready_Before_Render(const _float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);
}

HRESULT CUIWaveTimer_Image::Render()
{
	if (!m_isVisible)
		return S_OK;
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;
	if (FAILED(Super::Render()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIWaveTimer_Image::Ready_Components(WAVE_TIMER_IMAGE_DESC* pDesc)
{
	if (FAILED(Super::Ready_Components(pDesc)))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIWaveTimer_Image::Bind_ShaderResources()
{
	CShader* pShader = Get_Component<CShader>();
	if (FAILED(Get_Component<CTransform>()->Bind_ShaderResource(pShader)))
		return E_FAIL;

	switch (m_eDImageSubClass)
	{
	case DTO::EUIDImageSubClassType::MONSTER_WAVE_NODE:
	{
		if (FAILED(pShader->Get_Variable("g_fGlowIntensity")->SetRawValue(&m_fGlowIntensity, 0, sizeof(_float))))
			return E_FAIL;
	}
		break;
	case DTO::EUIDImageSubClassType::MONSTER_WAVE_NODE_LINE:
		break;
	case DTO::EUIDImageSubClassType::MONSTER_WAVE_TIMER_BG:
		break;
	}


	if (FAILED(Super::Bind_ShaderResources()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIWaveTimer_Image::Attach_Personal_Info()
{
	switch (m_eDImageSubClass)
	{
	case DTO::EUIDImageSubClassType::MONSTER_WAVE_NODE:
		break;
	case DTO::EUIDImageSubClassType::MONSTER_WAVE_NODE_LINE:
		break;
	case DTO::EUIDImageSubClassType::MONSTER_WAVE_TIMER_BG:
		break;
	}
	return S_OK;
}

void CUIWaveTimer_Image::Bind_Events()
{
	Super::Bind_Events();

	m_vecEventHandles.push_back(
		m_pGameInstance->Subscribe<QUEST_CHANGE_CHAPTER_NOTIFY>([this]()
			{
				auto desc = CQuestManager::GetInstance()->Get_QuestInfo();
				if (desc.tChapterInfo.eEvent == DTO::EQuestEvent::MONSTER_WAVE)
				{
					m_pSpawner = static_cast<CTriggerBox_MonsterWaveSpawner*>(desc.tChapterInfo.pObject);

					this->Set_Visible();
				}
				else if(desc.tChapterInfo.eEvent != DTO::EQuestEvent::MONSTER_KILL)
				{
					this->Set_Invisible();
				}
				else if (desc.tChapterInfo.eEvent == DTO::EQuestEvent::MONSTER_KILL && this->m_isVisible)
				{
					m_pGameInstance->Broadcast<BOSS_UI_ON>();
				}
			}));
}

void CUIWaveTimer_Image::Tick_By_Type(const _float fTimeDelta)
{
	auto desc = CQuestManager::GetInstance()->Get_QuestInfo();

	switch (m_eDImageSubClass)
	{
	case DTO::EUIDImageSubClassType::MONSTER_WAVE_NODE:
		if (nullptr == m_pSpawner)
			break;

		if((m_pSpawner->GetWaveData()->iCurrentWaveCount) == m_iNumbering)
		{
			m_isWaveTrigger = true;
		}

		if (m_isWaveTrigger)
		{
			m_fGlowIntensity = 3.f;
		}
		else
		{
			m_fGlowIntensity = 0.5f;
		}

		break;
	case DTO::EUIDImageSubClassType::MONSTER_WAVE_NODE_LINE:
		if (nullptr == m_pSpawner)
			break;

		if ((m_pSpawner->GetWaveData()->iCurrentWaveCount) == m_iNumbering)
		{
			m_isWaveTrigger = true;
		}

		if (m_isWaveTrigger)
		{
			m_fAlpha_Ratio = 1.f;
		}
		else
		{
			m_fAlpha_Ratio = 0.7f;
		}
		break;
	case DTO::EUIDImageSubClassType::MONSTER_WAVE_TIMER_BG:
		break;
	}
}

void CUIWaveTimer_Image::Initialize_Visible_Event()
{
	switch (m_eDImageSubClass)
	{
	case DTO::EUIDImageSubClassType::MONSTER_WAVE_NODE:
		Ready_Fade(0.5f, 0.f, 1.f, m_fDelay);
		break;
	case DTO::EUIDImageSubClassType::MONSTER_WAVE_NODE_LINE:
		Ready_Fade(0.5f, 0.f, 0.7f, m_fDelay);
		break;
	case DTO::EUIDImageSubClassType::MONSTER_WAVE_TIMER_BG:
		Ready_Fade(0.5f, 0.f, 1.f, m_fDelay);
		break;
	}
	Ready_Lerp_Movement(Vec2{ 100.f, 0.f }, Vec2{ 0.f, 0.f }, 0.5f, 3.f, m_fDelay, true);
}

_bool CUIWaveTimer_Image::Tick_Visible_Event(const _float fTimeDelta)
{
	switch (m_eDImageSubClass)
	{
	case DTO::EUIDImageSubClassType::MONSTER_WAVE_NODE:
		break;
	case DTO::EUIDImageSubClassType::MONSTER_WAVE_NODE_LINE:
		break;
	case DTO::EUIDImageSubClassType::MONSTER_WAVE_TIMER_BG:
		break;
	}

	_bool isMove = Tick_Lerp_Movement(fTimeDelta);
	_bool isFade = Tick_Fade(fTimeDelta);
	if (isMove && isFade)
	{
		return true;
	}
	return false;
}

void CUIWaveTimer_Image::Initialize_InVisible_Event()
{
	switch (m_eDImageSubClass)
	{
	case DTO::EUIDImageSubClassType::MONSTER_WAVE_NODE:
		break;
	case DTO::EUIDImageSubClassType::MONSTER_WAVE_NODE_LINE:
		break;
	case DTO::EUIDImageSubClassType::MONSTER_WAVE_TIMER_BG:
		break;
	}
	Ready_Fade(0.5f, m_fAlpha_Ratio, 0.f, m_fDelay);
	Ready_Lerp_Movement(Vec2{ 0.f, 0.f }, Vec2{ -100.f, 0.f }, 0.5f, 3.f, m_fDelay, true);
}

_bool CUIWaveTimer_Image::Tick_InVisible_Event(const _float fTimeDelta)
{
	switch (m_eDImageSubClass)
	{
	case DTO::EUIDImageSubClassType::MONSTER_WAVE_NODE:
		break;
	case DTO::EUIDImageSubClassType::MONSTER_WAVE_NODE_LINE:
		break;
	case DTO::EUIDImageSubClassType::MONSTER_WAVE_TIMER_BG:
		break;
	}

	_bool isMove = Tick_Lerp_Movement(fTimeDelta);
	_bool isFade = Tick_Fade(fTimeDelta);
	if (isMove && isFade)
	{
		return true;
	}
	return false;
}

CUIWaveTimer_Image* CUIWaveTimer_Image::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CUIWaveTimer_Image* pInstance = new CUIWaveTimer_Image(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CUIWaveTimer_Image::Create, Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUIWaveTimer_Image::Clone(void* pArg)
{
	CUIWaveTimer_Image* pInstance = new CUIWaveTimer_Image(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CUIWaveTimer_Image::Clone, Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUIWaveTimer_Image::Free()
{
	Super::Free();
}
