#include "pch.h"
#include "TriggerBox_EnvJukebox.h"
#include "Transform.h"
#include "GameInstance.h"
#include "Level_Loading.h"

//=================
// Builder
//=================
#include "Builder_Map.h"

CTriggerBox_EnvJukebox::CTriggerBox_EnvJukebox(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CTriggerBox(pDevice, pContext)
{
	m_eTriggerBoxType = CTriggerBox::Type::ENV_JUKEBOX;
}

CTriggerBox_EnvJukebox::CTriggerBox_EnvJukebox(const CTriggerBox_EnvJukebox& rhs)
	: CTriggerBox(rhs)
{
}

HRESULT CTriggerBox_EnvJukebox::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;

	Set_Object_Enum_Tag(OBJECT_ENUM_TAG::TRIGGER_BOX_MILESTONE_DEFAULT);

	return S_OK;
}

HRESULT CTriggerBox_EnvJukebox::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	TRIGGERBOX_ENVJUKEBOX_DESC* pDesc = static_cast<TRIGGERBOX_ENVJUKEBOX_DESC*>(pArg);

	if (FAILED(Ready_Component(pDesc)))
		return E_FAIL;

	m_eType = pDesc->eType;
	m_strSoundTag = pDesc->strSoundTag;
	m_iSoundHash = pDesc->iSoundHash;
	m_fRadius = pDesc->fRadius;

	//m_iPlayerSecondFootSoundHash[EFOOTSOUNDTYPE::GRASS] = TO_HASH("sfx_footstep_grass_r");
	//m_iPlayerSecondFootSoundHash[EFOOTSOUNDTYPE::SAND] = TO_HASH("sfx_footstep_sand_r");
	//m_iPlayerSecondFootSoundHash[EFOOTSOUNDTYPE::WATER] = TO_HASH("sfx_footstep_water_r");
	//m_iPlayerSecondFootSoundHash[EFOOTSOUNDTYPE::WOOD] = TO_HASH("sfx_footstep_wood_r");

	if (m_strSoundTag == "sfx_footstep_grass_r")
		m_eEnvFootSoundType = CSound_Handler::EFOOTSOUNDTYPE::GRASS;
	else if (m_strSoundTag == "sfx_footstep_sand_r")
		m_eEnvFootSoundType = CSound_Handler::EFOOTSOUNDTYPE::SAND;
	else if (m_strSoundTag == "sfx_footstep_water_r")
		m_eEnvFootSoundType = CSound_Handler::EFOOTSOUNDTYPE::WATER;
	else if (m_strSoundTag == "sfx_footstep_wood_r")
		m_eEnvFootSoundType = CSound_Handler::EFOOTSOUNDTYPE::WOOD;
	else
		m_eEnvFootSoundType = CSound_Handler::EFOOTSOUNDTYPE::NONE;

	return S_OK;
}

HRESULT CTriggerBox_EnvJukebox::Ready_Component(TRIGGERBOX_ENVJUKEBOX_DESC* pDesc)
{
	return S_OK;
}

HRESULT CTriggerBox_EnvJukebox::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;

	m_pPlayer = m_pGameInstance->Get_GameObject_Front(0, L"Player_Layer");

	m_vecMyPos = Get_Component<CTransform>()->Get_Info(TRANSFORM_INFO_STATE::POS);

	return S_OK;
}

void CTriggerBox_EnvJukebox::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CTriggerBox_EnvJukebox::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);

	switch (m_eType)
	{
	case Engine::EEnvJukeboxType::FOOTSTEP:
		break;
	case Engine::EEnvJukeboxType::TRIGGER:
		break;
	case Engine::EEnvJukeboxType::RANGE:
		Calc_PlayerDist();
		break;
	default:
		break;
	}
}

void CTriggerBox_EnvJukebox::Update_Late(const _float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);
}

void CTriggerBox_EnvJukebox::Ready_Before_Render(const _float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);
}

HRESULT CTriggerBox_EnvJukebox::Render()
{
	return S_OK;
}

void CTriggerBox_EnvJukebox::OnCollision(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther)
{
	Super::OnCollision(iMyColliderLayer, iOtherLayer, pOther);
}

void CTriggerBox_EnvJukebox::OnCollision_Enter(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther, const COL_HIT_INFO& tHitInfo)
{
	Super::OnCollision_Enter(iMyColliderLayer, iOtherLayer, pOther, tHitInfo);
}

void CTriggerBox_EnvJukebox::OnCollision_Exit(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther)
{
	Super::OnCollision_Exit(iMyColliderLayer, iOtherLayer, pOther);
}

void CTriggerBox_EnvJukebox::OnTrigger_Enter(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther, const COL_HIT_INFO& tHitInfo)
{
	if (iOtherLayer & PHYSICSFILTERGROUP::PLAYER)
	{
		Super::OnTrigger_Enter(iMyColliderLayer, iOtherLayer, pOther, tHitInfo);

		switch (m_eType)
		{
		case Engine::EEnvJukeboxType::FOOTSTEP:
		{
			if (m_pPlayer == nullptr || m_pPlayer->IsDead())
				return;

			m_pPlayer->Get_Component<CSound_Handler>()->Set_EnvFootSound(m_eEnvFootSoundType);
		}
			break;
		case Engine::EEnvJukeboxType::TRIGGER:
			break;
		case Engine::EEnvJukeboxType::RANGE:
		default:
			break;
		}
	}
	else
		return;
}

void CTriggerBox_EnvJukebox::OnTrigger_Exit(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther)
{
  	if (iOtherLayer & PHYSICSFILTERGROUP::PLAYER)
	{
		Super::OnTrigger_Exit(iMyColliderLayer, iOtherLayer, pOther);

		switch (m_eType)
		{
		case Engine::EEnvJukeboxType::FOOTSTEP:
		{
			if (m_pPlayer == nullptr || m_pPlayer->IsDead())
				return;

			m_pPlayer->Get_Component<CSound_Handler>()->Set_EnvFootSound(CSound_Handler::EFOOTSOUNDTYPE::NONE);
		}
		break;
		case Engine::EEnvJukeboxType::TRIGGER:
			break;
		case Engine::EEnvJukeboxType::RANGE:
		default:
			break;
		}
	}
	else
		return;
}

void CTriggerBox_EnvJukebox::QuestEnter()
{
	Super::QuestEnter();
}

void CTriggerBox_EnvJukebox::QuestExit()
{
	Super::QuestExit();
}

void CTriggerBox_EnvJukebox::Calc_PlayerDist()
{
	Vec3 vecPlayerPos = m_pPlayer->Get_Component<CTransform>()->Get_Info(TRANSFORM_INFO_STATE::POS);

	Vec3 vecDist = vecPlayerPos - m_vecMyPos;

	_float fLength = vecDist.Length();

	if (fLength <= m_fRadius)
		Ctrl_RangedVolume(fLength);
	else if (fLength > m_fRadius && m_bPlayed)
		m_pGameInstance->Stop_Controlled(ENUM_TO_UINT(EControlledChannel::AMBIENT));
}

void CTriggerBox_EnvJukebox::Ctrl_RangedVolume(_float fDist)
{
	_float fRatio = (m_fRadius - fDist) / m_fRadius;

	if (m_bPlayed == false)
	{
		m_bPlayed = true;

		m_pGameInstance->Play_Controlled(0 /* static */, m_iSoundHash, ENUM_TO_UINT(EControlledChannel::AMBIENT), fRatio, true);
	}
	else
		m_pGameInstance->Set_ControlledVolume(ENUM_TO_UINT(EControlledChannel::AMBIENT), fRatio);
}

CTriggerBox_EnvJukebox* CTriggerBox_EnvJukebox::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTriggerBox_EnvJukebox* pTriggerBox = new CTriggerBox_EnvJukebox(pDevice, pContext);

	if (FAILED(pTriggerBox->Initialize_Prototype()))
	{
		Safe_Release(pTriggerBox);
		MSG_BOX(" Trigger Box Jukebox Is Failed To Craete ");
		return nullptr;
	}

	return pTriggerBox;
}

CGameObject* CTriggerBox_EnvJukebox::Clone(void* pArg)
{
	CTriggerBox_EnvJukebox* pTriggerBox = new CTriggerBox_EnvJukebox(*this);

	if (FAILED(pTriggerBox->Initialize(pArg)))
	{
		Safe_Release(pTriggerBox);
		MSG_BOX(" Trigger Box Jukebox Is Failed To Clone ");
		return nullptr;
	}

	return pTriggerBox;
}

void CTriggerBox_EnvJukebox::Free()
{
	Super::Free();
}
