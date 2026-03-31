#include "pch.h"
#include "Client_EventDefine.h"
#include "Boss_Lianhuo.h"
#include "Sword.h"
#include "MainPlayer.h"
#include "Model.h"
#include "Boss_Lianhuo_Body.h"
#include "Bone.h"
#include "StatCom_Boss.h"
#include "PhysicsCCT.h"
#include "ComputeShader.h"
#include "MonsterActionState.h"
#include "MonsterControlContext.h"
#include "Weapon.h"
#include "UI_Manager.h"
#include "UIIcon_Component.h"
#include "CameraEventBinder.h"
#include "MyStat.h"
#include "Lianhuo_GimmikController.h"

// CustomState
#include "State_BackdashCatch.h"
#include "State_EndCatch.h"
#include "State_StartCatch.h"
#include "State_GimmikAttack.h"
#include "State_GimmikCamera.h"
#include "State_GimmikRunLoop.h"
#include "State_GimmikRunStart.h"
#include "State_SpawnAttack.h"

#include "GameInstance.h"

CBoss_Lianhuo::CBoss_Lianhuo(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: Super(pDevice, pDeviceContext)
{
	m_eMonsterType = EMonster_Type::Lianhuo;
}


CBoss_Lianhuo::CBoss_Lianhuo(const CBoss_Lianhuo& rhs)
	: Super(rhs)
{
	m_arrStateIndex.fill(-1);
}

HRESULT CBoss_Lianhuo::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;

	Set_Object_Enum_Tag(OBJECT_ENUM_TAG::MONSTER_BOSS_LIANHUO);

	return S_OK;
}

HRESULT CBoss_Lianhuo::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Ability()))
		return E_FAIL;

	Set_Name("지옥불의 교도소장 리안후오");

	if (FAILED(Ready_Weapon()))
		return E_FAIL;

	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;

	if (FAILED(Ready_CameraEvent()))
		return E_FAIL;

	if (FAILED(Ready_CustomStates()))
		return E_FAIL;

	if (FAILED(Ready_StateIndexForDirecting()))
		return E_FAIL;

	Set_GhostTrailDesc();
	return S_OK;
}

HRESULT CBoss_Lianhuo::Ready_GlobalEvent()
{
	return S_OK;
}

HRESULT CBoss_Lianhuo::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;

	if (FAILED(Get_Component<CLianhuo_GimmikController>()->Awake(iCurrentLevelID)))
		return E_FAIL;

	CTransform* pTrnasform = Get_Component<CTransform>();
	pTrnasform->Set_MovePerSec(4.f);
	pTrnasform->Set_RotatePerSec(5.f);

	{
		UI_PREFAB_DATA ePrefabData = {};
		UI_BOSS_NAMEPLATE_PREFAB_DATA Desc = {};
		Desc.pTarget = this;
		ePrefabData.Data = Desc;
		CUI_Manager::GetInstance()->Request_Add_Prefab(iCurrentLevelID, EUIPrefabType::BOSS_NAMEPLATE, iCurrentLevelID, &ePrefabData);
	}

	if (FAILED(Change_State_ForDirecting(EStateForDirecting::Idle)))
		return E_FAIL;

	if (FAILED(Ready_GlobalEvent()))
		return E_FAIL;

	vPos = Get_Component<CTransform>()->Get_Info(TRANSFORM_INFO_STATE::POS);
	return S_OK;
}

void CBoss_Lianhuo::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CBoss_Lianhuo::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
	Get_Component<CLianhuo_GimmikController>()->Update(fTimeDelta);
	CMonsterActionState* pActionState = Get_Component<CMonsterActionState>();
	if (m_pGameInstance->KeyButton_Down(DIK_NUMPAD2))
	{
		_int iIndex = pActionState->Get_StateIndex("StartCatch");
		pActionState->Change_State(iIndex);
	}
	if (m_pGameInstance->KeyButton_Down(DIK_NUMPAD4))
	{		
		_int iIndex = pActionState->Get_StateIndex("Idle");
		pActionState->Change_State(iIndex);
	}
	if (m_pGameInstance->KeyButton_Down(DIK_NUMPAD5))
	{
		_int iIndex = pActionState->Get_StateIndex("BackdashCatch");
		pActionState->Change_State(iIndex);
	}
	if (m_pGameInstance->KeyButton_Down(DIK_NUMPAD6))
	{
		Get_Component<CTransform>()->Set_Info(TRANSFORM_INFO_STATE::POS, vPos);
		Get_Component<CPhysicsCCT>()->SetFootPosition(vPos);
	}
	if (m_pGameInstance->KeyButton_Down(DIK_NUMPAD8))
	{
		_int iIndex = pActionState->Get_StateIndex("GimmikCamera");
		pActionState->Change_State(iIndex);
	}
}

void CBoss_Lianhuo::Update_Late(const _float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);
}

void CBoss_Lianhuo::Ready_Before_Render(const _float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);
}

HRESULT CBoss_Lianhuo::Render()
{
	if (FAILED(Super::Render()))
		return E_FAIL;

	return S_OK;
}

void CBoss_Lianhuo::OnCollision(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther)
{
	Super::OnCollision(iMyColliderLayer, iOtherLayer, pOther);
}

void CBoss_Lianhuo::OnCollision_Enter(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther, const COL_HIT_INFO& tHitInfo)
{
	Super::OnCollision_Enter(iMyColliderLayer, iOtherLayer, pOther, tHitInfo);
}

void CBoss_Lianhuo::OnCollision_Exit(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther)
{
	Super::OnCollision_Exit(iMyColliderLayer, iOtherLayer, pOther);
}

void CBoss_Lianhuo::OnTrigger_Enter(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther, const COL_HIT_INFO& tHitInfo)
{
	Super::OnTrigger_Enter(iMyColliderLayer, iOtherLayer, pOther, tHitInfo);
}

void CBoss_Lianhuo::OnTrigger_Exit(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther)
{
	Super::OnTrigger_Exit(iMyColliderLayer, iOtherLayer, pOther);
}

_bool CBoss_Lianhuo::On_Hit(const HIT_DESC& hitDesc)
{
	_bool result = Super::On_Hit(hitDesc);
	if (result == true)
	{
		CMonsterControlContext* pControlContext = Get_Component<CMonsterControlContext>();
		CStatCom_Boss* pComBoss = Get_Component<CStatCom_Boss>();
		// 그로기 일때
		if (pControlContext->IsGroggy() == true)
		{
			// 플레이어 공격이라면
			if (hitDesc.attackDesc.iAttackerLayer == EPhysicsFilterGroup::ATTACK)
			{
				// 확정 static_cast<>
				if (CMainPlayer* pMainPlayer = dynamic_cast<CMainPlayer*>(hitDesc.attackDesc.pAttacker))
				{
					CActionState* pActionState = pMainPlayer->Get_Component<CActionState>();
					// Condemn
					if (pActionState->Get_CurrentStateIndex() == ENUM_TO_UINT(CPlayer::State::CONDEMN))
					{
						pComBoss->Add_Health(-1247.f);
						_float fHpRatio = pComBoss->Get_Rate(CMyStat::STAT_TYPE::HP);
						if (fHpRatio <= g_XMEpsilon.f[0])
						{
							Change_State_ForDirecting(EStateForDirecting::Condemned_Die);
							m_pGameInstance->Broadcast<BOSS_UI_OFF>();
						}
						else
							Change_State_ForDirecting(EStateForDirecting::Condemned_Attacked);
					}
				}
			}
		}
		else
		{
			EGroggyState eGroggy{ EGroggyState::None };
			if (Engine_Utils::Has_Flag(hitDesc.iDamageFlag, ENUM_TO_UINT(EPlayerAttackFlag::GUN)))
				eGroggy = Get_Component<CStatCom_Boss>()->Sub_Groggy(0.125f);
			else if (Engine_Utils::Has_Flag(hitDesc.iDamageFlag, ENUM_TO_UINT(EPlayerAttackFlag::DUAL)))
				eGroggy = Get_Component<CStatCom_Boss>()->Sub_Groggy(0.5f);
			else
				eGroggy = Get_Component<CStatCom_Boss>()->Sub_Groggy(1.f);

			// 그로기 세팅하고 !!리턴!!
			if (eGroggy != EGroggyState::None)
			{
				if (_bool bRequstedSucess = pControlContext->Set_Groggy(eGroggy))
					m_pGameInstance->Broadcast<BOSS_GROGGY>();

			}
		}
	}
	return result;
}

void CBoss_Lianhuo::On_Dying()
{
	CMonsterControlContext* pControlContext = Get_Component<CMonsterControlContext>();
	if (_bool bRequestedSucess = pControlContext->Set_Groggy(EGroggyState::Final))
		m_pGameInstance->Broadcast<BOSS_GROGGY>();
}

void CBoss_Lianhuo::Try_Attack(const HIT_DESC& hitDesc)
{
	Super::Try_Attack(hitDesc);

	Get_Component<CMonsterControlContext>()->Set_AttackLanded();
}

const Matrix* CBoss_Lianhuo::Get_PlayerLocBonePosition()
{
	CBoss_Lianhuo_Body* pBody = Get_Part<CBoss_Lianhuo_Body>(Part::BODY);
	CModel* pModel = pBody->Get_Component<CModel>();
	CBone* pBone = pModel->Get_Bone(178);
	return &pBone->Get_CombinedTransformMatrix();
}

HRESULT CBoss_Lianhuo::Change_State_ForDirecting(EStateForDirecting eState)
{
	if (eState < 0 || eState >= COUNT)
		return E_FAIL;

	CActionState* pActionState = Get_Component<CActionState>();
	if (pActionState == nullptr)
		return E_FAIL;

	if (FAILED(pActionState->Change_State(m_arrStateIndex[eState], true)))
		return E_FAIL;

	return S_OK;
}

void CBoss_Lianhuo::Play_GhostTrail()
{
	CBoss_Lianhuo_Body* pBody = Get_Part<CBoss_Lianhuo_Body>(Part::BODY);
	if (pBody == nullptr)
		return;

	pBody->Get_Component<CModel>()->Enable_GhostTrail();
}

void CBoss_Lianhuo::Stop_GhostTrail()
{
	CBoss_Lianhuo_Body* pBody = Get_Part<CBoss_Lianhuo_Body>(Part::BODY);
	if (pBody == nullptr)
		return;

	pBody->Get_Component<CModel>()->Disable_GhostTrail();
}

void CBoss_Lianhuo::Clear_GhostTrail()
{
	CBoss_Lianhuo_Body* pBody = Get_Part<CBoss_Lianhuo_Body>(Part::BODY);
	if (pBody == nullptr)
		return;

	pBody->Get_Component<CModel>()->Clear_GhostTrail();
}

HRESULT CBoss_Lianhuo::Ready_Ability()
{
	CStatCom_Boss::BOSS_STAT_DESC desc = {};
	desc.fCriticalAttack = 30.f;
	desc.fCriticalRate = 0.4f;
	desc.fMaxHp = 320000.f;
	desc.FStatFlags = CMyStat::StatFlags::None;
	desc.vecExtraComputeOrder = vector<_uint>{ 0, 2 };

	if (FAILED(Add_Component<CStatCom_Boss>(0 /*static*/, L"Prototype_Component_Stat_Boss", &desc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBoss_Lianhuo::Ready_Weapon()
{
	// Weapons
	{
		CWeapon::WEAPON_DESC weaponDesc = {};
		weaponDesc.wstrModelPrototypeName = L"Prototype_Component_Model_LianhuoWeapon";
		weaponDesc.pMatParent = &Get_Component<CTransform>()->Get_WorldMatrix();
		weaponDesc.pMatHandSocket = Get_Part<CBoss_Lianhuo_Body>(Part::BODY)->Get_SocketMatrix(238);
		weaponDesc.eModel = CWeapon::Weapon_ModelType::STATIC;
		weaponDesc.eState = CWeapon::State::HAND;
		weaponDesc.bMianWeapon = true;
		weaponDesc.FDescFlag = 0;
		if (FAILED(Add_Part(Part::SWORD, 0, L"Prototype_GameObject_Part_Sword", &weaponDesc)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CBoss_Lianhuo::Ready_Components(void* pArg)
{
	MONSTER_DESC* pDesc = static_cast<MONSTER_DESC*>(pArg);
	CMonster_Body_Base* pBody = Get_Part<CMonster_Body_Base>(ENUM_TO_UINT(Part::BODY));
	if (pBody == nullptr)
		return E_FAIL;
	// TODO : BattleFiled

	{
		CSword* pSword = Get_Part<CSword>(ENUM_TO_UINT(Part::SWORD));
		if (pSword == nullptr)
			return E_FAIL;
		CComputeShader* pBodyAnimECS = static_cast<CComputeShader*>(pBody->Get_Script_Component(TEXT("ComputeShader_AnimE")));
		if (pBodyAnimECS == nullptr)
			return E_FAIL;

		CMonsterActionState::MONSTERACTIONSTATE_DESC desc = {};
		desc.pOwnerModel = pBody->Get_Component<CModel>();
		desc.pOwnerWeaponModel = pSword->Get_Component<CModel>();
		desc.pOwnerAnimECS = pBodyAnimECS;
		desc.wstrMonsterStateTag = pDesc->wstrMonsterStateTag;
		desc.iLevelIndex = pDesc->iLevelIndex;
		if (FAILED(Add_Component<CMonsterActionState>(0, L"Prototype_Component_ActionState_Monster", &desc)))
			return E_FAIL;
	}

	{
		CMonsterControlContext::MONSTER_CONTROLCONTEXT_DESC desc{};
		desc.fMeleeRange = 8.f;
		desc.fAttackRange = 16.f;
		desc.fCloseRange = 3.f;
		desc.fDetectionRange = 20.f;
		desc.fSpeed = 1.f;
		//desc.iSkillCount;
		//desc.vecSkillRange;

		if (FAILED(Add_Component<CMonsterControlContext>(0 /*static*/, L"Prototype_Component_ControlContext_Monster", &desc)))
			return E_FAIL;
	}

	{
		CUIIcon_Component::UI_ICON_COMP_DESC Desc = {};
		Desc.wstrIconTextureTag = L"Texture_T_Battle_HudRescue_Elite";
		if (FAILED(Add_Script_Component(L"UIIconComp", L"Prototype_ScriptComponent_UIIcon", &Desc)))
			return E_FAIL;
	}

	{
		CLianhuo_GimmikController::tagGimmikControllerDesc desc = {};
		desc.pOwnerModel = Get_Part<CBoss_Lianhuo_Body>(Part::Enum::BODY)->Get_Component<CModel>();
		if (FAILED(Add_Component<CLianhuo_GimmikController>(0, L"Prototype_Component_Lianhuo_GimmikController", &desc)))
			return E_FAIL;
	}
	return S_OK;
}

HRESULT CBoss_Lianhuo::Ready_StateIndexForDirecting()
{
	CMonsterActionState* pActionState = Get_Component<CMonsterActionState>();
	if (pActionState == nullptr)
		return E_FAIL;

	auto setStateIndex = [&](_uint iStateIndex, const string& strStateName)->_bool
		{
			_uint iIndex = pActionState->Get_StateIndex(strStateName);
			if (iIndex < 0)
				return false;
			m_arrStateIndex[iStateIndex] = iIndex;
			return true;
		};

	if (setStateIndex(EStateForDirecting::Idle, "Idle") == false)
		return E_FAIL;
	if (setStateIndex(EStateForDirecting::Condemned_Die, "Condemnde_Die") == false)
		return E_FAIL;
	if (setStateIndex(EStateForDirecting::Condemned_Attacked, "Condemnde_Attacked") == false)
		return E_FAIL;
	//if (setStateIndex(EStateForDirecting::Direction, "Direction") == false)
	//	return E_FAIL;

	return S_OK;
}

HRESULT CBoss_Lianhuo::Ready_CameraEvent()
{
	_uint iLevelID = m_pGameInstance->Get_CurrentLevelIndex();
	CBoss_Lianhuo_Body* pBody = Get_Part<CBoss_Lianhuo_Body>(ENUM_TO_UINT(Part::BODY));
	if (pBody == nullptr)
		return E_FAIL;
	CModel* pAnimModel = pBody->Get_Component<CModel>();
	if (pAnimModel == nullptr)
		return E_FAIL;
	// 내부에서 Add_Component 해줌
	CCameraEventBinder* pResult = CCameraEventBinder::Create(iLevelID, this, pAnimModel, L"../../Resources/Data/CameraAnimationData/Lianhuo.json");
	if (pResult == nullptr)
		return E_FAIL;
	Safe_Release(pResult);
	return S_OK;
}

HRESULT CBoss_Lianhuo::Ready_CustomStates()
{
	auto AddState = [&]<typename T>(const string & strStateTag, CMonsterActionState * pActionState)->HRESULT
	{
		_int iStateIndex{ -1 };
		DTO::MONSTER_STATEBASE_DESC* pDesc = pActionState->Get_StateDesc(strStateTag, iStateIndex);
		if (pDesc == nullptr || iStateIndex == -1)
			return E_FAIL;
		if (FAILED(pActionState->Add_State(iStateIndex, T::Create(pActionState, iStateIndex, pDesc))))
			return E_FAIL;
		return S_OK;
	};

	CMonsterActionState* pActionState = Get_Component<CMonsterActionState>();
	if (pActionState == nullptr)
		return E_FAIL;

#define ADD_CUSTOM_STATE(TYPE, TAG) AddState.operator()<TYPE>(TAG, pActionState)

	// For. State_BackdashCatch
	if (FAILED(ADD_CUSTOM_STATE(CState_BackdashCatch, "BackdashCatch")))
		return E_FAIL;

	// For. State_EndCatch
	if (FAILED(ADD_CUSTOM_STATE(CState_EndCatch, "EndCatch")))
		return E_FAIL;

	// For. State_StartCatch
	if (FAILED(ADD_CUSTOM_STATE(CState_StartCatch, "StartCatch")))
		return E_FAIL;

	// For. State_GimmikAttack
	if (FAILED(ADD_CUSTOM_STATE(CState_GimmikAttack, "GimmikAttack")))
		return E_FAIL;

	// For. State_GimmikCamera
	if (FAILED(ADD_CUSTOM_STATE(CState_GimmikCamera, "GimmikCamera")))
		return E_FAIL;

	// For. State_GimmikRunLoop
	if (FAILED(ADD_CUSTOM_STATE(CState_GimmikRunLoop, "GimmikRunLoop")))
		return E_FAIL;

	// For. State_GimmikRunStart
	if (FAILED(ADD_CUSTOM_STATE(CState_GimmikRunStart, "GimmikRunStart")))
		return E_FAIL;

	// For. State_SpawnAttack
	if (FAILED(ADD_CUSTOM_STATE(CState_SpawnAttack, "SpawnAttack")))
		return E_FAIL;

	return S_OK;
}

void CBoss_Lianhuo::Set_GhostTrailDesc()
{
	CBoss_Lianhuo_Body* pBody = Get_Part<CBoss_Lianhuo_Body>(Part::BODY);
	if (pBody == nullptr)
		return;

	CModel::GHOST_TRAIL_DESC desc{};
	desc.fInterval = 0.15f;
	desc.iMaxCount = 12;
	desc.vColor = Vec4(1.00f, 0.32f, 0.18f, 0.42f);
	pBody->Get_Component<CModel>()->Set_GhostTrailDesc(desc);
}

CBoss_Lianhuo* CBoss_Lianhuo::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CBoss_Lianhuo* pInsatnce = new CBoss_Lianhuo(pDevice, pDeviceContext);
	if (FAILED(pInsatnce->Initialize_Prototype()))
	{
		MSG_BOX("CBoss_Lianhuo::Create, Failed");
		Safe_Release(pInsatnce);
	}
	return pInsatnce;
}

CGameObject* CBoss_Lianhuo::Clone(void* pArg)
{
	CBoss_Lianhuo* pInstance = new CBoss_Lianhuo(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CBoss_Lianhuo::Clone, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBoss_Lianhuo::Free()
{
	Super::Free();
}