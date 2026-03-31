#include "pch.h"
#include "UIWaveTimer_Text.h"
#include "Client_Defines.h"
#include "Client_EventDefine.h"

//=================
// Component
//=================
#include "Texture.h"
#include "Shader.h"
#include "VIBuffer_Rect_Tex.h"
#include "UI_Manager.h"
#include "TriggerBox_MonsterWaveSpawner.h"
#include "QuestManager.h"
#include "GameInstance.h"

CUIWaveTimer_Text::CUIWaveTimer_Text(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	:CUIText(pDevice, pDeviceContext)
{
}

CUIWaveTimer_Text::CUIWaveTimer_Text(const CUIWaveTimer_Text& rhs)
	:CUIText(rhs)
{
}

HRESULT CUIWaveTimer_Text::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIWaveTimer_Text::Initialize(void* pArg)
{
	WAVE_TIMER_TEXT_DESC* pDesc = static_cast<WAVE_TIMER_TEXT_DESC*>(pArg);
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;
	if (FAILED(Ready_Components(pDesc)))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIWaveTimer_Text::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;

	if (FAILED(Attach_Personal_Info()))
		return E_FAIL;

	return S_OK;
}

void CUIWaveTimer_Text::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CUIWaveTimer_Text::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
	Tick_By_Type(fTimeDelta);
}

void CUIWaveTimer_Text::Update_Late(const _float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);
}

void CUIWaveTimer_Text::Ready_Before_Render(const _float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);

	if (FAILED(Convert_Stat_To_Text()))
		return;
}

HRESULT CUIWaveTimer_Text::Render()
{
	if (!m_isVisible)
		return S_OK;
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;
	if (FAILED(Super::Render()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIWaveTimer_Text::Ready_Components(WAVE_TIMER_TEXT_DESC* pDesc)
{
	if (FAILED(Super::Ready_Components(pDesc)))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIWaveTimer_Text::Bind_ShaderResources()
{
	CShader* pShader = Get_Component<CShader>();
	if (FAILED(Get_Component<CTransform>()->Bind_ShaderResource(pShader)))
		return E_FAIL;
	if (FAILED(Super::Bind_ShaderResources()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIWaveTimer_Text::Attach_Personal_Info()
{
	switch (m_eTextSubClassType)
	{
	case DTO::EUITextSubClassType::MONSTER_WAVE_INFO_TEXT:
		break;
	case DTO::EUITextSubClassType::MONSTER_WAVE_TIMER_TEXT:
		break;
	}

	return S_OK;
}


void CUIWaveTimer_Text::Bind_Events()
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
				else if (desc.tChapterInfo.eEvent != DTO::EQuestEvent::MONSTER_KILL)
				{
					this->Set_Invisible();
				}
			}));
}

void CUIWaveTimer_Text::Tick_By_Type(const _float fTimeDelta)
{
	switch (m_eTextSubClassType)
	{
	case DTO::EUITextSubClassType::MONSTER_WAVE_INFO_TEXT:
		break;

	case DTO::EUITextSubClassType::MONSTER_WAVE_TIMER_TEXT:
	{
		if (nullptr == m_pSpawner)
			break;

		auto* pWaveData = m_pSpawner->GetWaveData();
		if (nullptr == pWaveData)
			break;

		_float fWaveEnd = {};

		switch (pWaveData->iCurrentWaveCount)
		{
		case 0: // °³
			fWaveEnd = pWaveData->vecWaveInfo[1].fSpawnTime;
			break;

		case 1: // ¿ÀÂ¡¾î
			fWaveEnd = pWaveData->vecWaveInfo[2].fSpawnTime;
			break;

		case 2: // ºÎ¸Ó
			fWaveEnd = pWaveData->vecWaveInfo[3].fSpawnTime;
			break;

		case 3: // ¿¤¸®Æ®
			m_wstrText = L"00:00";
			break;

		default:
			m_wstrText = L"00:00";
			break;
		}

		if (3 <= pWaveData->iCurrentWaveCount || 0 > pWaveData->iCurrentWaveCount)
			break;

		_float fCurrentTime = max(0.f, pWaveData->fCurrentWaveTime);
		_float fRemainTime = fWaveEnd - fCurrentTime;

		if (fRemainTime < 0.f)
			fRemainTime = 0.f;

		_int iRemainTime = static_cast<_int>(fRemainTime);
		_int iMinute = iRemainTime / 60;
		_int iSecond = iRemainTime % 60;

		_wstring wstrMinute = (iMinute < 10 ? L"0" : L"") + std::to_wstring(iMinute);
		_wstring wstrSecond = (iSecond < 10 ? L"0" : L"") + std::to_wstring(iSecond);

		m_wstrText = wstrMinute + L":" + wstrSecond;
	}
	break;
	}
}
HRESULT CUIWaveTimer_Text::Convert_Stat_To_Text()
{
	return S_OK;
}

void CUIWaveTimer_Text::Initialize_Visible_Event()
{
	switch (m_eTextSubClassType)
	{
	case DTO::EUITextSubClassType::MONSTER_WAVE_INFO_TEXT:
		break;
	case DTO::EUITextSubClassType::MONSTER_WAVE_TIMER_TEXT:
		break;
	}
	Ready_Fade_Text(0.5f, 0.f, 1.f, m_fDelay);
	Ready_Lerp_Movement(Vec2{ 100.f, 0.f }, Vec2{ 0.f, 0.f }, 0.5f, 3.f, m_fDelay, true);
}

void CUIWaveTimer_Text::Initialize_InVisible_Event()
{
	switch (m_eTextSubClassType)
	{
	case DTO::EUITextSubClassType::MONSTER_WAVE_INFO_TEXT:
		break;
	case DTO::EUITextSubClassType::MONSTER_WAVE_TIMER_TEXT:
		break;
	}
	Ready_Fade_Text(0.5f, 1.f, 0.f, m_fDelay);
	Ready_Lerp_Movement(Vec2{ 0.f, 0.f }, Vec2{ -100.f, 0.f }, 0.5f, 3.f, m_fDelay, true);
}

_bool CUIWaveTimer_Text::Tick_Visible_Event(const _float fTimeDelta)
{
	switch (m_eTextSubClassType)
	{
	case DTO::EUITextSubClassType::MONSTER_WAVE_INFO_TEXT:
		break;
	case DTO::EUITextSubClassType::MONSTER_WAVE_TIMER_TEXT:
		break;
	}

	_bool isMove = Tick_Lerp_Movement(fTimeDelta);
	_bool isFade = Tick_Fade_Text(fTimeDelta);

	if (isMove && isFade)
	{
		return true;
	}
	return false;
}

_bool CUIWaveTimer_Text::Tick_InVisible_Event(const _float fTimeDelta)
{
	switch (m_eTextSubClassType)
	{
	case DTO::EUITextSubClassType::MONSTER_WAVE_INFO_TEXT:
		break;
	case DTO::EUITextSubClassType::MONSTER_WAVE_TIMER_TEXT:
		break;
	}

	_bool isMove = Tick_Lerp_Movement(fTimeDelta);
	_bool isFade = Tick_Fade_Text(fTimeDelta);

	if (isMove && isFade)
	{
		return true;
	}
	return false;
}

CUIWaveTimer_Text* CUIWaveTimer_Text::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CUIWaveTimer_Text* pInstance = new CUIWaveTimer_Text(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CUIWaveTimer_Text::Create, Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUIWaveTimer_Text::Clone(void* pArg)
{
	CUIWaveTimer_Text* pInstance = new CUIWaveTimer_Text(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CUIWaveTimer_Text::Clone, Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUIWaveTimer_Text::Free()
{
	Super::Free();
}
