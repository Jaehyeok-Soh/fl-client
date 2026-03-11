#include "pch.h"
#include "State_Condemn.h"

#include "Player.h"
#include "Weapon.h"
#include "PhysicsCCT.h"

#include "GameInstance.h"

CState_Condemn::CState_Condemn(CActionState* pOwnerComponent)
	: Super(pOwnerComponent, "Condemn")
{
}

HRESULT CState_Condemn::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CState_Condemn::Awake(const _uint iLevelIndex)
{
	if (FAILED(Super::Awake(iLevelIndex)))
		return E_FAIL;

	return S_OK;
}

HRESULT CState_Condemn::Start(void* pArg, _bool bForce)
{
	if (FAILED(Super::Start(pArg, bForce)))
		return E_FAIL;

	// boss 앞에 위치. 바라 보도록
	CGameObject* pBoss = m_pGameInstance->Get_GameObject_Front(m_pGameInstance->Get_CurrentLevelIndex(), g_wszBossLayer);
	if (pBoss == nullptr)
		return S_OK;
	CTransform* pBossTransform = pBoss->Get_Component<CTransform>();
	CTransform* pPlayerTransform = Get_OwnerObject()->Get_Component<CTransform>();
	CPhysicsCCT* pCCT = Get_OwnerObject()->Get_Component<CPhysicsCCT>();
	if(pBossTransform == nullptr ||
		pPlayerTransform == nullptr ||
		pCCT == nullptr)
		return S_OK;

	Vec3 vBossPos	= pBossTransform->Get_Info(TRANSFORM_INFO_STATE::POS);
	Vec3 vBossLook	= pBossTransform->Get_Info(TRANSFORM_INFO_STATE::LOOK);
	vBossLook.Normalize();

	// boss 위치 + look 방향으로 밀어낸 위치
	Vec3 vNewPos = vBossPos + vBossLook * 5.f;
	pCCT->SetFootPosition(vNewPos);

	// boss를 쳐다 봄
	pPlayerTransform->Look_At_XZ(vBossPos);


	Change_Weapon(CPlayer::Part::SWORD, ENUM_TO_UINT(CWeapon::State::HAND));

	return S_OK;
}

void CState_Condemn::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
}

HRESULT CState_Condemn::End()
{
	if (FAILED(Super::End()))
		return E_FAIL;

	Change_Weapon(CPlayer::Part::SWORD, ENUM_TO_UINT(CWeapon::State::HOLD));

	return S_OK;
}

CState_Condemn* CState_Condemn::Create(CActionState* pOwnerComponent, void* pArg)
{
	CState_Condemn* pInstance = new CState_Condemn(pOwnerComponent);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CState_Condemn::Create, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CState_Condemn::Free()
{
	Super::Free();
}
