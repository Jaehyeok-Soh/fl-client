#include "pch.h"
#include "TriggerBox_CinematicPlayer.h"

#include "GameInstance.h"
#include "QuestManager.h"

CTriggerBox_CinematicPlayer::CTriggerBox_CinematicPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CTriggerBox(pDevice, pContext), m_strCinemaitcCameraSequnceName{""}
{
}

CTriggerBox_CinematicPlayer::CTriggerBox_CinematicPlayer(const CTriggerBox_CinematicPlayer& rhs)
	: CTriggerBox(rhs), m_strCinemaitcCameraSequnceName{ rhs.m_strCinemaitcCameraSequnceName }
{
}

HRESULT CTriggerBox_CinematicPlayer::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CTriggerBox_CinematicPlayer::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	TRIGGERBOX_CINEMATICCAMERA_DESC* pDesc = static_cast<TRIGGERBOX_CINEMATICCAMERA_DESC*>(pArg);


	m_strCinemaitcCameraSequnceName = pDesc->strCinemaitcCameraSequnceName;



	return S_OK;
}

HRESULT CTriggerBox_CinematicPlayer::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;

	return S_OK;
}

void CTriggerBox_CinematicPlayer::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CTriggerBox_CinematicPlayer::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);

}

void CTriggerBox_CinematicPlayer::Update_Late(const _float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);
}

void CTriggerBox_CinematicPlayer::Ready_Before_Render(const _float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);

}
HRESULT CTriggerBox_CinematicPlayer::Render()
{
	if (FAILED(Super::Render()))
		return E_FAIL;

	return S_OK;
}


void CTriggerBox_CinematicPlayer::OnCollision(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther)
{
}

void CTriggerBox_CinematicPlayer::OnCollision_Enter(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther, const COL_HIT_INFO& tHitInfo)
{
	return;
}

void CTriggerBox_CinematicPlayer::OnCollision_Exit(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther)
{
}

void CTriggerBox_CinematicPlayer::OnTrigger_Enter(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther, const COL_HIT_INFO& tHitInfo)
{
	if (!m_isTriggerEventPlay)
	{
		m_isTriggerEventPlay = true;
		if(FAILED(m_pGameInstance->Play_CameraCinematic(Engine_Utils::ToWString(m_strCinemaitcCameraSequnceName))))
			return;
	}


	if (m_eQuestEvent == DTO::QUESTEVENT::AREA_ENTER)
	{
		CallQuestEvent(Get_Object_Enum_Tag(), 1);
	}
}

void CTriggerBox_CinematicPlayer::OnTrigger_Exit(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther)
{
	if (m_eQuestEvent == DTO::QUESTEVENT::AREA_EXIT)
	{
		CallQuestEvent(Get_Object_Enum_Tag(), 1);
	}
}

CTriggerBox_CinematicPlayer* CTriggerBox_CinematicPlayer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTriggerBox_CinematicPlayer* pTriggerBox = new CTriggerBox_CinematicPlayer(pDevice, pContext);
	if (FAILED(pTriggerBox->Initialize_Prototype()))
	{
		MSG_BOX("TriggerBox TutorialUIEvent is Failed To Create");
		return nullptr;
	}

	return pTriggerBox;
}

CGameObject* CTriggerBox_CinematicPlayer::Clone(void* pArg)
{
	CTriggerBox_CinematicPlayer* pTriggerBox = new CTriggerBox_CinematicPlayer(*this);
	if (FAILED(pTriggerBox->Initialize(pArg)))
	{
		MSG_BOX("TriggerBox TutorialUIEvent is Failed To Clone");
		return nullptr;
	}

	return pTriggerBox;
}

void CTriggerBox_CinematicPlayer::Free()
{
	Super::Free();
}