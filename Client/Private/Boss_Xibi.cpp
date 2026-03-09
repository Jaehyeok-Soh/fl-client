#include "pch.h"
#include "Client_EventDefine.h"
#include "Boss_Xibi.h"
#include "Sword.h"
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
#include "MyStat.h"

CBoss_Xibi::CBoss_Xibi(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: Super(pDevice, pDeviceContext)
{
	m_eMonsterType = EMonster_Type::Xibi;

}


CBoss_Xibi::CBoss_Xibi(const CBoss_Xibi& rhs)
	: Super(rhs)
{
}

HRESULT CBoss_Xibi::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBoss_Xibi::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Ability()))
		return E_FAIL;

	Set_Name("시빌라");

	if (FAILED(Ready_Weapon()))
		return E_FAIL;

	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;

	if (FAILED(Get_Component<CGimmikController>()->Bind_Events()))
		return E_FAIL;

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

	/*이펙트를 생성하기 위해서*/
	if (Engine_Utils::Has_Flag(hitDesc.iDamageFlag, ENUM_TO_UINT(EPlayerAttackFlag::MOON)))
	{
		/*	hitDesc.attackDesc.iAttackerLayer = PHYSICSFILTERGROUP::ATTACK_PROJECTTILE;*/
		EFFECT_SPAWN_DESC Desc = {};
		//Matrix OffsetMatrix = Matrix::CreateTranslation(Vec3(0.f, 0.5f, 0.5f));
		Matrix WorldMatrix = Get_Component<CTransform>()->Get_WorldMatrix();

		Vec3 vScale, vPos;
		Quat vQuat;
		WorldMatrix.Decompose(vScale, vQuat, vPos);

		Desc.matWorld = Matrix::CreateFromQuaternion(vQuat) * Matrix::CreateTranslation(hitDesc.vHitPoint);
		Desc.iSimulationType = (int)EFFECT_SPAWN_DESC::E_VFX_SIMULTYPE::VFX_WORLD;
		m_pGameInstance->Request_Effect("VFX_Sword_Hit", Desc);
	}

	return result;
}

void CBoss_Xibi::Try_Attack(const HIT_DESC& hitDesc)
{
	Super::Try_Attack(hitDesc);
}

HRESULT CBoss_Xibi::Ready_Ability()
{
	CMyStat::STAT_DESC desc = {};
	desc.fMaxHp = 3000.f;
	desc.fDefense = 1000.f;
	desc.FStatFlags = CMyStat::StatFlags::HpUpdate | CMyStat::StatFlags::DefenseUpdtae;

	if (FAILED(Add_Component<CMyStat>(0/* STATIC */, L"Prototype_Component_Stat", &desc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBoss_Xibi::Ready_Weapon()
{
	// Weapons
	{
		CWeapon::WEAPON_DESC weaponDesc = {};
		weaponDesc.wstrModelPrototypeName = L"Prototype_Component_Model_XibiWeapon";
		weaponDesc.pMatParent = &Get_Component<CTransform>()->Get_WorldMatrix();
		weaponDesc.pMatHandSocket = Get_Part<CBoss_Xibi_Body>(Part::BODY)->Get_SocketMatrix(375);
		weaponDesc.eModel = CWeapon::Weapon_ModelType::ANIM;
		weaponDesc.eAnimState = CWeapon::AnimState::PLAY;
		weaponDesc.eState = CWeapon::State::HAND_ONLY_POS_SCALE;
		weaponDesc.bMianWeapon = true;
		weaponDesc.FDescFlag = 0;

		weaponDesc.iStartAnimIdx = 2;

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
	desc.fAttackRange = 4.f;
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

	// BossStat
	{
		CStatCom_Boss::BOSS_STAT_DESC desc{};
		desc.fCriticalAttack = 30.f;
		desc.fCriticalRate = 0.4f;
		desc.fMaxHp = 200000.f;
		desc.FStatFlags = CMyStat::StatFlags::None;
		desc.vecExtraComputeOrder = vector<_uint>{ 0, 2 };
		if (FAILED(Add_Component<CStatCom_Boss>(0 /*static*/, L"Prototype_Component_Stat_Boss", &desc)))
			return E_FAIL;
	}

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