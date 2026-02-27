#include "pch.h"
#include "Boss_Xibi.h"
#include "Sword.h"
#include "Model.h"
#include "Boss_Xibi_Body.h"
#include "Bone.h"
#include "ComputeShader.h"
#include "MonsterActionState.h"
#include "MonsterControlContext.h"
#include "Weapon.h"
#include "GameInstance.h"

CBoss_Xibi::CBoss_Xibi(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: Super(pDevice, pDeviceContext)
{
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

	if (FAILED(Ready_Weapon()))
		return E_FAIL;

	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBoss_Xibi::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
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
}

void CBoss_Xibi::OnCollision_Enter(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther, const COL_HIT_INFO& tHitInfo)
{
}

void CBoss_Xibi::OnCollision_Exit(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther)
{
}

void CBoss_Xibi::OnTrigger_Enter(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther)
{
}

void CBoss_Xibi::OnTrigger_Exit(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther)
{
}

HRESULT CBoss_Xibi::Ready_Weapon()
{
	// Weapons
	{
		CWeapon::WEAPON_DESC weaponDesc = {};
		weaponDesc.wstrModelPrototypeName = L"Prototype_Component_Model_XibiWeapon";
		weaponDesc.pMatParent = &Get_Component<CTransform>()->Get_WorldMatrix();
		//weaponDesc.pMatHandSocket = &Get_Part<CBoss_Xibi_Body>(Part::BODY)->Get_Bone(CMonster_Body_Base::EBone::RightHand)->Get_CombinedTransformMatrix();
		weaponDesc.pMatHandSocket = Get_Part<CBoss_Xibi_Body>(Part::BODY)->Get_SocketMatrix(375);
		weaponDesc.eModel = CWeapon::Weapon_ModelType::ANIM;
		weaponDesc.eAnimState = CWeapon::AnimState::PLAY;
		weaponDesc.eState = CWeapon::State::HAND;
		weaponDesc.bMianWeapon = true;
		weaponDesc.FDescFlag = 0;

		weaponDesc.iStartAnimIdx = 2;

		if (FAILED(Add_Part(Part::SWORD, ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_GameObject_Part_Sword", &weaponDesc)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CBoss_Xibi::Ready_Components(void* pArg)
{
	MONSTER_DESC* pDesc = static_cast<MONSTER_DESC*>(pArg);

	// TODO : BattleFiled

	{
		CMonster_Body_Base* pBody = Get_Part<CMonster_Body_Base>(ENUM_TO_UINT(Part::BODY));
		if (pBody == nullptr)
			return E_FAIL;
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
	if(FAILED(pInstance->Initialize(pArg)))
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