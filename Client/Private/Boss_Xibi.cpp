#include "pch.h"
#include "Boss_Xibi.h"
#include "Sword.h"
#include "Model.h"
#include "Boss_Xibi_Body.h"
#include "Bone.h"
#include "ComputeShader.h"
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

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBoss_Xibi::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;
	CComputeShader* pCom = static_cast<CComputeShader*>(Get_Script_Component(L"ComputeShader_AnimE"));
	CWeapon* pWaepon = Get_Part<CWeapon>(Part::SWORD);

	pWaepon->Set_WeaponState(CWeapon::State::HAND);
	pWaepon->Get_Component<CModel>()->Change_Animation(pCom, 1, true);
	pWaepon->Set_Weapon_PlayState(0);
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
		weaponDesc.pMatHandSocket = &Get_Part<CBoss_Xibi_Body>(Part::BODY)->Get_Bone(CMonster_Body_Base::EBone::RightHand)->Get_CombinedTransformMatrix();
		weaponDesc.eModel = CWeapon::Weapon_ModelType::ANIM;
		weaponDesc.bMianWeapon = true;
		weaponDesc.FDescFlag = 0;
		weaponDesc.eAnimState = CWeapon::AnimState::PLAY;
		if (FAILED(Add_Part(Part::SWORD, ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_GameObject_Part_Sword", &weaponDesc)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CBoss_Xibi::Ready_Components()
{
	// TODO : BattleFiled


	//typedef struct tagMonsterControlContextDesc
	//{
	//	_float fMeleeRange = {};
	//	_float fAttackRange = {};
	//	_float fCloseRange = {};
	//	_float fDetectionRange = {};
	//	_float fSpeed = {};
	//	_int iSkillCount = { -1 };
	//	vector<_int> vecSkillRange;
	//}MONSTER_CONTROLCONTEXT_DESC;

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