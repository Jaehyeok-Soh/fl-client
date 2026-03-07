#include "Engine_pch.h"
#include "SkillObjectSpawner_RandomXZ.h"
#include "GameInstance.h"

CSkillObjectSpawner_RandomXZ::CSkillObjectSpawner_RandomXZ(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: Super(pDevice, pDeviceContext)
{
}

CSkillObjectSpawner_RandomXZ::CSkillObjectSpawner_RandomXZ(const CSkillObjectSpawner_RandomXZ& rhs)
	: Super(rhs)
{
}

HRESULT CSkillObjectSpawner_RandomXZ::Initialize_Prototype(SPAWNER_ORIGIN_DESC* pOriginDesc)
{
	if (FAILED(Super::Initialize_Prototype(pOriginDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CSkillObjectSpawner_RandomXZ::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	SPAWNER_RAND_COPY_DESC* pDesc = static_cast<SPAWNER_RAND_COPY_DESC*>(pArg);
	m_fDurationSec = pDesc->fDurationSec;
	m_iPerTick = pDesc->iPerTick;
	m_fRadiusMin = pDesc->fRadiusMin;
	m_fRadiusMax = pDesc->fRadiusMax;
	m_bUseForwardDir = pDesc->bUseForwardDir;
	m_vFixedDir = pDesc->vFixedDir;


	if (m_fRadiusMax < m_fRadiusMin)
		std::swap(m_fRadiusMin, m_fRadiusMax);

	return S_OK;
}

_uint CSkillObjectSpawner_RandomXZ::Get_TotalCount() const
{
	if (m_fDurationSec <= 0.f)
		return UINT_MAX;

	// interval이 0이면 duration 의미가 약하니 1텀만
	const _float interval = m_pOriginDesc->fInterval;
	if (interval <= 0.f)
		return 1;

	return (_uint)std::ceil(m_fDurationSec / interval);
}

void CSkillObjectSpawner_RandomXZ::Emit_One(_uint i, const Vec3& vForward, const Vec3& vUp)
{
	const _uint iCount = (std::max)(1u, m_iPerTick);
	for (_uint k = 0; k < iCount; ++k)
	{
		const Vec3 vPos = Make_RandomPos_OnPlane(m_desc.vOrigin, vForward, vUp);

		Vec3 dir = m_vFixedDir;
		if (m_bUseForwardDir)
			dir = vForward;

		Spawn_SkillObject(vPos, dir);
	}
}

Vec3 CSkillObjectSpawner_RandomXZ::Make_RandomPos_OnPlane(const Vec3& vOrigin, const Vec3& vForwardIn, const Vec3& vUpIn) const
{
	Vec3 vUp = vUpIn;
	if (vUp == Vec3::Zero)
		vUp = Vec3(0.f, 1.f, 0.f);
	vUp.Normalize();

	// fwd를 up 평면에 투영
	Vec3 vForward = vForwardIn - vUp * vForwardIn.Dot(vUp);
	if (vForward == Vec3::Zero) vForward = Vec3(0.f, 0.f, 1.f);
	vForward.Normalize();

	Vec3 vRight = vUp.Cross(vForward);
	if (vRight == Vec3::Zero) vRight = Vec3(1.f, 0.f, 0.f);
	vRight.Normalize();

	const _float u = m_pGameInstance->Rand_Float(0.f, 1.f);
	const _float v = m_pGameInstance->Rand_Float(0.f, 1.f);

	const _float fRadius = std::lerp(m_fRadiusMin, m_fRadiusMax, std::sqrt(u));
	const _float fArea = v * XM_2PI;

	return vOrigin + vRight * (std::cos(fArea) * fRadius) + vForward * (std::sin(fArea) * fRadius);
}

CSkillObjectSpawner_RandomXZ* CSkillObjectSpawner_RandomXZ::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, SPAWNER_ORIGIN_DESC* pDesc)
{
	CSkillObjectSpawner_RandomXZ* pInstance = new CSkillObjectSpawner_RandomXZ(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype(pDesc)))
	{
		MSG_BOX("CSkillObjectSpawner_RandomXZ::Create, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CSkillObjectSpawner_RandomXZ::Clone(void* pArg)
{
	CSkillObjectSpawner_RandomXZ* pInstance = new CSkillObjectSpawner_RandomXZ(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CSkillObjectSpawner_RandomXZ::Clone, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CSkillObjectSpawner_RandomXZ::Free()
{
	Super::Free();
}
