#include "pch.h"
#include "SkillPierceProjectile.h"
#include "GameInstance.h"

CSkillPierceProjectile::CSkillPierceProjectile(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: Super(pDevice, pContext)
{
}

CSkillPierceProjectile::CSkillPierceProjectile(const CSkillPierceProjectile& rhs)
	: Super(rhs)
{
}

HRESULT CSkillPierceProjectile::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CSkillPierceProjectile::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CSkillPierceProjectile::Handle_Hit(_uint iMyLayer, _uint iOtherLayer, CGameObject* pOther, const COL_HIT_INFO& tHitInfo)
{
	COLLIDED_DESC desc{};
	desc.iCollisionType = COLLISIONEVENT::ON_COLLISION_ENTER;
	desc.iRequesterLayer = iMyLayer;
	desc.iOtherLayer = iOtherLayer;
	desc.pRequester = this;
	desc.pOther = pOther;
	desc.tHitInfo = tHitInfo;
	m_pGameInstance->Push_CollidedData(desc);
}

CSkillPierceProjectile* CSkillPierceProjectile::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSkillPierceProjectile* pInstance = new CSkillPierceProjectile(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		Safe_Release(pInstance);
		MSG_BOX("CSkillPierceProjectile::Create, Failed");
		return nullptr;
	}
	return pInstance;
}

CGameObject* CSkillPierceProjectile::Clone(void* pArg)
{
	CSkillPierceProjectile* pClone = new CSkillPierceProjectile(*this);
	if (FAILED(pClone->Initialize(pArg)))
	{
		Safe_Release(pClone);
		MSG_BOX("CSkillPierceProjectile::Clone, Failed");
		return nullptr;
	}
	return pClone;
}

void CSkillPierceProjectile::Free()
{
	Super::Free();
}
