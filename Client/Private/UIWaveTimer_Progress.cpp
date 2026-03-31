#include "pch.h"
#include "UIWaveTimer_Progress.h"
#include "Client_Defines.h"
#include "Client_EventDefine.h"
//=================
// Component
//=================
#include "Texture.h"
#include "Shader.h"
#include "VIBuffer_Rect_Tex.h"
#include "GameInstance.h"
#include "TriggerBox_MonsterWaveSpawner.h"
#include "QuestManager.h"
#include <UI_Manager.h>

CUIWaveTimer_Progress::CUIWaveTimer_Progress(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	:CUIProgress_Bar(pDevice, pDeviceContext)
{
}

CUIWaveTimer_Progress::CUIWaveTimer_Progress(const CUIWaveTimer_Progress& rhs)
	:CUIProgress_Bar(rhs)
{
}

HRESULT CUIWaveTimer_Progress::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIWaveTimer_Progress::Initialize(void* pArg)
{
	WAVE_TIMER_PROGRESS_DESC* pDesc = static_cast<WAVE_TIMER_PROGRESS_DESC*>(pArg);
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;
	if (FAILED(Ready_Components(pDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CUIWaveTimer_Progress::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;

	if (FAILED(Attach_Personal_Info()))
		return E_FAIL;

	return S_OK;
}

void CUIWaveTimer_Progress::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CUIWaveTimer_Progress::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);

	// CurRatio 갱신
	Convert_Stat_To_Ratio();
}

void CUIWaveTimer_Progress::Update_Late(const _float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);
}

void CUIWaveTimer_Progress::Ready_Before_Render(const _float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);
}

HRESULT CUIWaveTimer_Progress::Render()
{
	if (!m_isVisible)
		return S_OK;
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;
	if (FAILED(Super::Render()))
		return E_FAIL;
	return S_OK;
}

void CUIWaveTimer_Progress::Initialize_Visible_Event()
{
	Ready_Lerp_Movement(Vec2{ 100.f, 0.f }, Vec2{ 0.f, 0.f }, 0.5f, 3.f, m_fDelay, true);
	Ready_Fade(0.5f, 0.f, 1.f, m_fDelay);
}

void CUIWaveTimer_Progress::Initialize_InVisible_Event()
{
	Ready_Lerp_Movement(Vec2{ 0.f, 0.f }, Vec2{ -100.f, 0.f }, 0.5f, 3.f, m_fDelay, true);
	Ready_Fade(0.5f, 1.f, 0.f, m_fDelay);
}

_bool CUIWaveTimer_Progress::Tick_Visible_Event(const _float fTimeDelta)
{

	_bool isMove = Tick_Lerp_Movement(fTimeDelta);
	_bool isFade = Tick_Fade(fTimeDelta);

	if (isMove && isFade)
	{
		return true;
	}
	return false;
}

_bool CUIWaveTimer_Progress::Tick_InVisible_Event(const _float fTimeDelta)
{

	_bool isMove = Tick_Lerp_Movement(fTimeDelta);

	if (isMove)
	{
		return true;
	}
	return false;
}

void CUIWaveTimer_Progress::Bind_Events()
{
	Super::Bind_Events();

	m_vecEventHandles.push_back(
		m_pGameInstance->Subscribe<QUEST_CHANGE_CHAPTER_NOTIFY>([this]()
			{
				auto desc = CQuestManager::GetInstance()->Get_QuestInfo();
				if (desc.tChapterInfo.eEvent == DTO::EQuestEvent::MONSTER_WAVE)
				{
					m_pSpawner = static_cast<CTriggerBox_MonsterWaveSpawner*>(desc.tChapterInfo.pObject);
					m_fProgress_Ratio = 0.f;
					m_fCurRatio = 0.f;
					this->Set_Visible();
				}
				else if (desc.tChapterInfo.eEvent != DTO::EQuestEvent::MONSTER_KILL)
				{
					if (m_isVisible)
					this->Set_Invisible();
				}
			}));
}

HRESULT CUIWaveTimer_Progress::Ready_Components(WAVE_TIMER_PROGRESS_DESC* pDesc)
{
	if (FAILED(Super::Ready_Components(pDesc)))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIWaveTimer_Progress::Bind_ShaderResources()
{
	CShader* pShader = Get_Component<CShader>();
	if (FAILED(Get_Component<CTransform>()->Bind_ShaderResource(pShader)))
		return E_FAIL;

	if (FAILED(Super::Bind_ShaderResources()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUIWaveTimer_Progress::Attach_Personal_Info()
{
	return S_OK;
}

HRESULT CUIWaveTimer_Progress::Convert_Stat_To_Ratio()
{
	if (nullptr == m_pSpawner)
		return E_FAIL;

	auto* pWaveData = m_pSpawner->GetWaveData();
	if (nullptr == pWaveData)
		return E_FAIL;


	m_fCurRatio = 0.f;

	_float fSectionStartTime = 0.f;
	_float fSectionEndTime = 0.f;
	_float fCurrentTime = max(0.f, pWaveData->fCurrentWaveTime);

	switch (pWaveData->iCurrentWaveCount)
	{
	case 0: // 개
		fSectionStartTime = 0.f;
		fSectionEndTime = pWaveData->vecWaveInfo[1].fSpawnTime;
		break;

	case 1: // 오징어 
		fSectionStartTime = pWaveData->vecWaveInfo[1].fSpawnTime;
		fSectionEndTime = pWaveData->vecWaveInfo[2].fSpawnTime;
		break;

	case 2: // 부머
		fSectionStartTime = pWaveData->vecWaveInfo[2].fSpawnTime;
		fSectionEndTime = pWaveData->vecWaveInfo[3].fSpawnTime;
		break;

	case 3: // 엘리트
		fSectionStartTime = pWaveData->vecWaveInfo[3].fSpawnTime;
		fSectionEndTime = pWaveData->fWaveTime;
		break;

	default:
		m_fCurRatio = 0.f;
		return S_OK;
	}

	fSectionStartTime = max(0.f, fSectionStartTime);
	fSectionEndTime = max(0.f, fSectionEndTime);

	if (fCurrentTime < fSectionStartTime)
		fCurrentTime = fSectionStartTime;

	_float fSectionTime = fSectionEndTime - fSectionStartTime;
	_float fRemainTime = fSectionEndTime - fCurrentTime;

	fSectionTime = max(0.f, fSectionTime);
	fRemainTime = max(0.f, fRemainTime);

	m_fCurRatio = (fSectionTime > 0.f) ? (fRemainTime / fSectionTime) : 0.f;
	m_fCurRatio = max(0.f, std::min(1.f, m_fCurRatio));

	return S_OK;
}

CUIWaveTimer_Progress* CUIWaveTimer_Progress::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CUIWaveTimer_Progress* pInstance = new CUIWaveTimer_Progress(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CUIWaveTimer_Progress::Create, Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUIWaveTimer_Progress::Clone(void* pArg)
{
	CUIWaveTimer_Progress* pInstance = new CUIWaveTimer_Progress(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CUIWaveTimer_Progress::Clone, Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUIWaveTimer_Progress::Free()
{
	Super::Free();
}
