#include "pch.h"
#include "Client_EventDefine.h"
#include "Boss_Xibi.h"
#include "Sword.h"
#include "MainPlayer.h"
#include "Model.h"
#include "Boss_Xibi_Body.h"
#include "Bone.h"
#include "StatCom_Boss.h"
#include "ComputeShader.h"
#include "MonsterActionState.h"
#include "MonsterControlContext.h"
#include "Xibi_GimmikController.h"
#include "Weapon.h"
#include "GameInstance.h"
#include "UI_Manager.h"
#include "UIIcon_Component.h"
#include "MyStat.h"

CBoss_Xibi::CBoss_Xibi(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: Super(pDevice, pDeviceContext)
{
	m_eMonsterType = EMonster_Type::Xibi;

}


CBoss_Xibi::CBoss_Xibi(const CBoss_Xibi& rhs)
	: Super(rhs)
{
	m_arrStateIndex.fill(-1);
}

HRESULT CBoss_Xibi::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;

	Set_Object_Enum_Tag(OBJECT_ENUM_TAG::MONSTER_BOSS_XIBI);

	return S_OK;
}

HRESULT CBoss_Xibi::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Ability()))
		return E_FAIL;

	Set_Name("악몽의 군단장 시빌라");

	if (FAILED(Ready_Weapon()))
		return E_FAIL;

	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;

	if (FAILED(Get_Component<CGimmikController>()->Bind_Events()))
		return E_FAIL;

	if (FAILED(Ready_StateIndexForDirecting()))
		return E_FAIL;
	
	return S_OK;
}

HRESULT CBoss_Xibi::Ready_GlobalEvent()
{
	m_pGameInstance->Subscribe<TUTORIAL_BOSS_CONTATCT>([this](){
		Set_Render(true);
		});

	/* Xibi_Cinematic Event 구독 */
	m_pGameInstance->Subscribe<XIBI_CHANGE_STATE_BOSS_DIRECTION>([this]() {
		Set_Render(true);
		if (FAILED(Change_State_ForDirecting(CBoss_Xibi::EStateForDirecting::Direction)))
		{
			MSG_BOX(" Boss 연출 Direction 실패 ");
			return E_FAIL;
		}

		return S_OK;
		});

	m_pGameInstance->Subscribe<XIBI_CHANGE_STATE_BOSS_IDLE>([this]() {
		if (FAILED(Change_State_ForDirecting(CBoss_Xibi::EStateForDirecting::Idle)))
		{
			MSG_BOX(" Boss 연출 Idle 변경 실패 ");
			return E_FAIL;
		}
		return S_OK;
		});


	return S_OK;
}

HRESULT CBoss_Xibi::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;

	if (FAILED(Get_Component<CXibi_GimmikController>()->Awake(iCurrentLevelID)))
		return E_FAIL;

	CTransform* pTrnasform = Get_Component<CTransform>();
	pTrnasform->Set_MovePerSec(1.5f);
	pTrnasform->Set_RotatePerSec(3.f);

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

	return S_OK;
}

void CBoss_Xibi::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CBoss_Xibi::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
	Get_Component<CXibi_GimmikController>()->Update(fTimeDelta);
}

void CBoss_Xibi::Update_Late(const _float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);
}

void CBoss_Xibi::Ready_Before_Render(const _float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);
}

HRESULT CBoss_Xibi::Render()
{
	if (FAILED(Super::Render()))
		return E_FAIL;

	return S_OK;
}

_int CBoss_Xibi::Get_WeaponAnimationIndex(const wstring& wstrName)
{
	if (wstrName.empty() == true)
		return -1;

	CSword* pSword = Get_Part<CSword>(Part::SWORD);
	if (pSword == nullptr)
		return -1;

	CModel* pModel = pSword->Get_Component<CModel>();
	if (pModel == nullptr)
		return -1;

	return pModel->Get_AnimationIndex(wstrName);
}

void CBoss_Xibi::OnCollision(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther)
{
	Super::OnCollision(iMyColliderLayer, iOtherLayer, pOther);
}

void CBoss_Xibi::OnCollision_Enter(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther, const COL_HIT_INFO& tHitInfo)
{
	Super::OnCollision_Enter(iMyColliderLayer, iOtherLayer, pOther, tHitInfo);
}

void CBoss_Xibi::OnCollision_Exit(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther)
{
	Super::OnCollision_Exit(iMyColliderLayer, iOtherLayer, pOther);
}

void CBoss_Xibi::OnTrigger_Enter(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther, const COL_HIT_INFO& tHitInfo)
{
	Super::OnTrigger_Enter(iMyColliderLayer, iOtherLayer, pOther, tHitInfo);
}

void CBoss_Xibi::OnTrigger_Exit(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther)
{
	Super::OnTrigger_Exit(iMyColliderLayer, iOtherLayer, pOther);
}

_bool CBoss_Xibi::On_Hit(const HIT_DESC& hitDesc)
{
	_bool result = Super::On_Hit(hitDesc);
	if (result == true)
	{
		CMonsterControlContext *pControlContext = Get_Component<CMonsterControlContext>();
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
						pComBoss->Add_Health(-500.f);
						_float fHpRatio = pComBoss->Get_Rate(CMyStat::STAT_TYPE::HP);
						if (fHpRatio <= g_XMEpsilon.f[0])
							Change_State_ForDirecting(EStateForDirecting::Condemned_Die);
						else
							Change_State_ForDirecting(EStateForDirecting::Condemned_Attacked);
					}
				}
			}
		}
		else
		{
			EGroggyState eGroggy{ EGroggyState::None };
			if(Engine_Utils::Has_Flag( hitDesc.iDamageFlag, ENUM_TO_UINT(EPlayerAttackFlag::GUN)))
				eGroggy = Get_Component<CStatCom_Boss>()->Sub_Groggy(0.25f);
			else
				eGroggy = Get_Component<CStatCom_Boss>()->Sub_Groggy(2.f);

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

void CBoss_Xibi::Try_Attack(const HIT_DESC& hitDesc)
{
	Super::Try_Attack(hitDesc);

	Get_Component<CMonsterControlContext>()->Set_AttackLanded();
}

HRESULT CBoss_Xibi::Change_State_ForDirecting(EStateForDirecting eState)
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

HRESULT CBoss_Xibi::Ready_Ability()
{
	CStatCom_Boss::BOSS_STAT_DESC desc = {};
	desc.fCriticalAttack = 30.f;
	desc.fCriticalRate = 0.4f;
	desc.fMaxHp = 3500000.f;
	desc.FStatFlags = CMyStat::StatFlags::None;
	desc.vecExtraComputeOrder = vector<_uint>{ 0, 2 };

	if (FAILED(Add_Component<CStatCom_Boss>(0 /*static*/, L"Prototype_Component_Stat_Boss", &desc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBoss_Xibi::Ready_Weapon()
{
	// Weapons
	{
		CWeapon::WEAPON_DESC weaponDesc		= {};
		weaponDesc.wstrModelPrototypeName	= L"Prototype_Component_Model_XibiWeapon";
		weaponDesc.pMatParent		= &Get_Component<CTransform>()->Get_WorldMatrix();
		weaponDesc.pMatHandSocket	= Get_Part<CBoss_Xibi_Body>(Part::BODY)->Get_SocketMatrix(75);
		weaponDesc.eModel			= CWeapon::Weapon_ModelType::ANIM;
		weaponDesc.eAnimState		= CWeapon::AnimState::PLAY;
		weaponDesc.eState			= CWeapon::State::HAND;		//CWeapon::State::HAND_ONLY_POS_SCALE;
		weaponDesc.bMianWeapon		= true;
		weaponDesc.FDescFlag		= 0;

		weaponDesc.iStartAnimIdx	= 2;


		weaponDesc.matHandOffsetMatrix = Matrix::CreateFromYawPitchRoll(XMConvertToRadians(180.f), XMConvertToRadians(100.f), XMConvertToRadians(0.f));

		if (FAILED(Add_Part(Part::SWORD, 0, L"Prototype_GameObject_Part_Sword", &weaponDesc)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CBoss_Xibi::Ready_Components(void* pArg)
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
		CComputeShader* pSwordAnimECS = static_cast<CComputeShader*>(pSword->Get_Script_Component(TEXT("ComputeShader_AnimE")));
		if (pSwordAnimECS == nullptr)
			return E_FAIL;

		CMonsterActionState::MONSTERACTIONSTATE_DESC desc = {};
		desc.pOwnerModel = pBody->Get_Component<CModel>();
		desc.pOwnerWeaponModel = pSword->Get_Component<CModel>();
		desc.pOwnerAnimECS = pBodyAnimECS;
		desc.pOwnerWeaponAnimECS = pSwordAnimECS;
		desc.wstrMonsterStateTag = pDesc->wstrMonsterStateTag;
		desc.iLevelIndex = pDesc->iLevelIndex;
		if (FAILED(Add_Component<CMonsterActionState>(0, L"Prototype_Component_ActionState_Monster", &desc)))
			return E_FAIL;
	}

	CMonsterControlContext::MONSTER_CONTROLCONTEXT_DESC desc{};
	desc.fMeleeRange = 2.f;
	desc.fAttackRange = 8.f;
	desc.fCloseRange = 1.f;
	desc.fDetectionRange = 15.f;
	desc.fSpeed = 1.f;
	//desc.iSkillCount;
	//desc.vecSkillRange;

	if (FAILED(Add_Component<CMonsterControlContext>(0 /*static*/, L"Prototype_Component_ControlContext_Monster", &desc)))
		return E_FAIL;

	// GimmikController
	{
		CXibi_GimmikController::GIMMIKCTRL_DESC desc{};
		desc.pOwnerModel = pBody->Get_Component<CModel>();
		if (FAILED(Add_Component<CXibi_GimmikController>(0 /*static*/, L"Prototype_Component_Xibi_GimmikController", &desc)))
			return E_FAIL;
	}

	{
		CUIIcon_Component::UI_ICON_COMP_DESC Desc = {};
		Desc.wstrIconTextureTag = L"Texture_T_Battle_HudRescue_Elite";
		if (FAILED(Add_Script_Component(L"UIIconComp", L"Prototype_ScriptComponent_UIIcon", &Desc)))
			return E_FAIL;
	}


	return S_OK;
}

HRESULT CBoss_Xibi::Ready_StateIndexForDirecting()
{
	CMonsterActionState *pActionState = Get_Component<CMonsterActionState>();
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
	if (setStateIndex(EStateForDirecting::Condemned_Die, "Condemned_Die") == false)
		return E_FAIL;
	if (setStateIndex(EStateForDirecting::Condemned_Attacked, "Condemned_Attacked") == false)
		return E_FAIL;
	if (setStateIndex(EStateForDirecting::Direction, "Direction") == false)
		return E_FAIL;
	 
	return S_OK;
}

CBoss_Xibi* CBoss_Xibi::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CBoss_Xibi* pInsatnce = new CBoss_Xibi(pDevice, pDeviceContext);
	if (FAILED(pInsatnce->Initialize_Prototype()))
	{
		MSG_BOX("CBoss_Xibi::Create, Failed");
		Safe_Release(pInsatnce);
	}
	return pInsatnce;
}

CGameObject* CBoss_Xibi::Clone(void* pArg)
{
	CBoss_Xibi* pInstance = new CBoss_Xibi(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CBoss_Xibi::Clone, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBoss_Xibi::Free()
{
	Super::Free();
}