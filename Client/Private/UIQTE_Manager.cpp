#include "pch.h"
#include "UIQTE_Manager.h"
#include "Engine_Utils.h"
#include "Canvas.h"
#include "GenericUI.h"
#include "UI_Manager.h"
#include "GameInstance.h"

NS_BEGIN(Client)

IMPLEMENT_SINGLETON(CUIQTE_Manager)

CUIQTE_Manager::CUIQTE_Manager()
	:m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}

void CUIQTE_Manager::Start_QTE(_uint iNodeCount)
{
	if (0 == iNodeCount)
	{
		MSG_BOX("iNodeCount is Zero");
		return;
	}
	m_pGameInstance->Active_SloMo(0.1f);

	m_vecQTEData.clear();

	// Clear Values // 
	{
		Init_Vaules(); 
		m_iMaxNodeCount = iNodeCount;
	}

	for (_uint i = 0; i < iNodeCount; ++i)
	{
		UI_QTE_PREFAB_DATA Data = {};
		Data.iNodeIndex = i;
		Data.iKeyType = Get_Random_Key();

		if (0 == i)
		{
			Vec2 vSpawnPos			= Vec2{ m_pGameInstance->Rand_Float(-100.f, 100.f), m_pGameInstance->Rand_Float(-100.f, 100.f) };
			Data.vSpawnOffset		= vSpawnPos;
			Data.vPreSpawnOffset	= vSpawnPos;
		}
		else
		{
			Vec2 vPrev				= m_vecQTEData[i - 1].vSpawnOffset;

			_float fAngle			= XMConvertToRadians(m_pGameInstance->Rand_Float(0.f, 90.f));
			_float fDistance		= m_pGameInstance->Rand_Float(90.f, 100.f);

			_float fOffsetX			= cosf(fAngle) * fDistance;
			_float fOffsetY			= sinf(fAngle) * fDistance;

			Data.vSpawnOffset		= Vec2{ vPrev.x + fOffsetX,vPrev.y + fOffsetY };
			Data.vPreSpawnOffset	= vPrev;
		}

		Data.fTimingTime = m_pGameInstance->Rand_Float(1.f, 1.5f);
		Data.fDelayTime = m_pGameInstance->Rand_Float(0.8f, 1.f);
		m_vecQTEData.push_back(Data);
	}

	_uint iShowCount = std::min(2u, (_uint)m_vecQTEData.size());

	for (_uint i = 0; i < iShowCount; ++i)
	{
		_uint iCurrentLevel = m_pGameInstance->Get_CurrentLevelIndex();
		UI_PREFAB_DATA tPrefabData = {};
		tPrefabData.Data = m_vecQTEData[m_iNextShowedNodeIndex++];

		CUI_Manager::GetInstance()->Request_Add_Prefab(iCurrentLevel, EUIPrefabType::QTE, iCurrentLevel, &tPrefabData);
	}

	m_isStartQTE = true;
}

void CUIQTE_Manager::Tick_QTE(const _float fTimeDelta)
{
	if (!m_isStartQTE)
		return;

	Tick_Show_NextNode(fTimeDelta);

	if (m_iCurrentNodeIndex >= m_iMaxNodeCount)
		return;

	const UI_QTE_PREFAB_DATA& Data = m_vecQTEData[m_iCurrentNodeIndex];

	m_fTimeAcc += fTimeDelta;
	if (m_fTimeAcc >= (Data.fTimingTime))
	{
		m_fTimeAcc = 0.f;
		Success_QTE();
	}
	else if (m_fTimeAcc >= (Data.fTimingTime * 0.5f))
	{
		if (KEY_BUTTON_DOWN(Data.iKeyType))
		{
			m_fTimeAcc = 0.f;
			Success_QTE();
		}
	}

	//CLOG_INFO(L"Current Node Index : " + std::to_wstring(m_iCurrentNodeIndex));
}

void CUIQTE_Manager::Success_QTE()
{
	UIEVENT_DESC Desc = {};
	Desc.eEventID = EUIEventID::QTE_SUCCESS;
	Desc.iParam0 = m_iCurrentNodeIndex;
	Desc.iParam1 = ++m_iCurrentNodeIndex;
	Desc.iParam2 = ENUM_TO_UINT(m_eCurrentQTETIming);
	CUI_Manager::GetInstance()->Get_UIEvents().Broadcast(Desc);

	if (m_iMaxNodeCount <= m_iCurrentNodeIndex)
	{
		// QTE 이벤트 끝 //
		m_isStartQTE = false;
		m_pGameInstance->Broadcast<QTE_CLEAR>();
		m_pGameInstance->Deactivate_SloMo();
	}
}

void CUIQTE_Manager::Fail_QTE()
{
	m_isStartQTE = false;
}

void CUIQTE_Manager::Set_Current_Node_Progress(EQTETiming eType)
{
	m_eCurrentQTETIming = eType;
}

_uint CUIQTE_Manager::Get_Random_Key()
{
	_uint i = m_pGameInstance->Rand_Int(0, 3);

	switch (i)
	{
	case 0: return DIK_A;
	case 1: return DIK_S;
	case 2: return DIK_D;
	case 3: return DIK_F;
	}

	return DIK_Q;
}

void CUIQTE_Manager::Tick_Show_NextNode(const _float fTimeDelta)
{
	if (m_iNextShowedNodeIndex >= m_iMaxNodeCount)
		return;

	m_fShowTimeAcc += fTimeDelta;

	if (m_fShowTimeAcc > m_vecQTEData[m_iNextShowedNodeIndex].fDelayTime)
	{
		_uint iCurrentLevel = m_pGameInstance->Get_CurrentLevelIndex();
		UI_PREFAB_DATA tPrefabData = {};
		tPrefabData.Data = m_vecQTEData[m_iNextShowedNodeIndex];

		CUI_Manager::GetInstance()->Request_Add_Prefab(iCurrentLevel, EUIPrefabType::QTE, iCurrentLevel, &tPrefabData);
		++m_iNextShowedNodeIndex;
		m_fShowTimeAcc = 0.f;
	}
}

void CUIQTE_Manager::Init_Vaules()
{
	m_fTimeAcc = 0.f;
	m_iCurrentNodeIndex = 0;
	m_fShowTimeAcc = 0.f;
	m_iNextShowedNodeIndex = 0;
	m_eCurrentQTETIming = EQTETiming::END;
}

void CUIQTE_Manager::Free()
{
	Safe_Release(m_pGameInstance);
	Super::Free();
}

NS_END