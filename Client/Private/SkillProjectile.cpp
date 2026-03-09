#include "pch.h"
#include "Transform.h"
#include "SkillProjectile.h"
#include "ColliderModule.h"
#include "EffectBase.h"
#include "GameInstance.h"

CSkillProjectile::CSkillProjectile(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: Super(pDevice, pContext)
{
}

CSkillProjectile::CSkillProjectile(const CSkillProjectile& rhs)
	: Super(rhs)
{
}

HRESULT CSkillProjectile::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CSkillProjectile::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CSkillProjectile::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;

	return S_OK;
}

void CSkillProjectile::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
}

void CSkillProjectile::OnTrigger_Enter(_uint iMyLayer, _uint iOtherLayer, Engine::CGameObject* pOther, const COL_HIT_INFO& tHitInfo)
{
	if (m_iCurrentState != ENUM_TO_UINT(EState::FLY))
		return;

	if (pOther == nullptr || pOther->IsDead())
		return;

	if (IsDying() || IsDead())
		return;

	// 기본은 맵에 부딪히면 Dying
	// 파생에서 결정
	Handle_Hit(iMyLayer, iOtherLayer, pOther, tHitInfo);
}

_bool CSkillProjectile::On_Hit(const HIT_DESC& hitDesc)
{
	if (IsDying() || IsDead())
		return false;

	Set_Dying();
	LoopEnd_EffectModules(ENUM_TO_UINT(EState::FLY));
	return true;
}

void CSkillProjectile::Update_HybridState(const _float fTimeDelta)
{
	EState eCurrentState = static_cast<EState>(m_iCurrentState);
	switch (eCurrentState)
	{
	case EState::FLY:
		if (IsDying() == true)
		{
			if (Is_StateEffectFinished(ENUM_TO_UINT(EState::FLY)))
				Change_HybridState(ENUM_TO_UINT(EState::IMPACT));

			return;
		}
		Process_Move(fTimeDelta);
		Process_Life(fTimeDelta);
		break;
	case EState::IMPACT:
	{
		if (Is_StateEffectFinished(ENUM_TO_UINT(EState::IMPACT)))
		{
			Set_Dead();
			return;
		}		
	} break;
	default:
		break;
	}
}

void CSkillProjectile::On_StateEnter(_uint iState)
{
	EState eCurrentState = static_cast<EState>(iState);
	switch (eCurrentState)
	{
	case EState::FLY:
	{
		Get_Component<CTransform>()->Look_At_Dir(m_tRuntimeDesc.vCurDir);
		if (m_tDesc.fSpeed > g_XMEpsilon.f[0])
			Get_Component<CTransform>()->Set_MovePerSec(m_tDesc.fSpeed);
	} break;
	case EState::IMPACT:
	{

	} break;
	}
}

void CSkillProjectile::On_StateExit(_uint iState)
{
	EState eCurrentState = static_cast<EState>(iState);
	switch (eCurrentState)
	{
	case EState::FLY:
		break;
	case EState::IMPACT:
		break;
	}
}

void CSkillProjectile::On_LifeOver()
{
	LoopEnd_EffectModules(ENUM_TO_UINT(EState::FLY));
}

void CSkillProjectile::Handle_Hit(_uint iMyLayer, _uint iOtherLayer, Engine::CGameObject* pOther, const COL_HIT_INFO& tHitInfo)
{
	if (iOtherLayer == PHYSICSFILTERGROUP::Enum::MAP)
	{
		Set_Dying();
		LoopEnd_EffectModules(ENUM_TO_UINT(EState::FLY));
		return;
	}

	COLLIDED_DESC desc{};
	desc.iCollisionType = COLLISIONEVENT::ON_COLLISION_ENTER;
	desc.iRequesterLayer = iMyLayer;
	desc.iOtherLayer = iOtherLayer;
	desc.pRequester = this;
	desc.pOther = pOther;
	desc.tHitInfo = tHitInfo;
	m_pGameInstance->Push_CollidedData(desc);
}
void CSkillProjectile::Free()
{
	Super::Free();
}
