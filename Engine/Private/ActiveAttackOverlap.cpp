#include "Engine_pch.h"
#include "ActiveAttackOverlap.h"
#include "GameInstance.h"
#include "EngineConsole.h"

#include "GameObject.h"

#include "EngineConsole.h"

CActiveAttackOverlap::CActiveAttackOverlap()
	: Super(),
	m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CActiveAttackOverlap::Initialize()
{
	m_strEventString = L"[On collision enter : Attack Overlap]\n";

	return S_OK;
}

void CActiveAttackOverlap::Update(_float fTimeDelta)
{
	if (m_tHitboxDesc == nullptr)
		return;

		m_fSumTime += fTimeDelta;
	if (m_fSumTime >= m_tHitboxDesc->fDuration)
		m_eState = Enum::FIN;

	Tick(fTimeDelta);

	if (m_pGameInstance->Execute_Overlap(
		m_tHitboxDesc->geometry.any(),
		m_pxTransform,
		hitBuffer,
		m_tHitboxDesc->filterData,
		(PxQueryFilterCallback*)m_tHitboxDesc->filterCallback))
	{
		for (PxU32 i = 0; i < hitBuffer.nbTouches; i++)
		{
			CGameObject* hitObject = static_cast<CGameObject*>(hitBuffer.touches[i].actor->userData);
			
			if (CheckAlreadyHit(hitObject))
				continue;

#ifdef _DEBUG
			Debug_Log(hitObject);
#endif // _DEBUG

			COL_HIT_INFO hitInfo{};
			Build_HitInfo_FromOverlap(m_tHitboxDesc->geometry.any(), m_pxTransform, hitBuffer.touches[i], hitInfo);

			const PxFilterData &victimFilterData = hitBuffer.touches[i].shape->getSimulationFilterData();
			_uint iAttackerLayer = m_tHitboxDesc->eFilterLayer;
			_uint iVictimLayer = victimFilterData.word0;

			m_pOwner->OnCollision_Enter(iAttackerLayer, iVictimLayer, hitObject, hitInfo);
			m_hitObjects.insert(hitObject);
		}
	}
}

void CActiveAttackOverlap::Reset()
{
	m_pOwner = { nullptr };
	m_eState = Enum::WAIT;
	m_fSumTime = 0.f;
	m_matTransform = Matrix::Identity;
	m_pxTransform = {};
	m_tHitboxDesc = { nullptr };
	m_hitObjects.clear();
	hitResults.clear();
}

void CActiveAttackOverlap::Set(DTO::HITBOX_DESC* pDesc, Matrix ownerMatrix, CGameObject* pOwner)
{
	m_pOwner = pOwner;

	m_tHitboxDesc = pDesc;
	
	m_matTransform = m_tHitboxDesc->matOffset * ownerMatrix;
	m_pxTransform = m_pGameInstance->XMMatrixToPxTransform(m_matTransform);

	hitResults.resize(m_tHitboxDesc->iMaxHit);
	hitBuffer.touches = hitResults.data();
	hitBuffer.maxNbTouches = m_tHitboxDesc->iMaxHit;
}

void CActiveAttackOverlap::Tick(_float fTimeDelta)
{
	if (m_tHitboxDesc->fTickTime < 0)
		return;

	m_fSumTickTime += fTimeDelta;
	if (m_fSumTickTime >= m_tHitboxDesc->fTickTime)
	{
		m_fSumTickTime = 0.f;
		m_hitObjects.clear();
	}
}

_bool CActiveAttackOverlap::CheckAlreadyHit(CGameObject* hitObject)
{
	auto obj = m_hitObjects.find(hitObject);
	if (obj != m_hitObjects.end())
		return true;

	return false;
}

_bool CActiveAttackOverlap::Build_HitInfo_FromOverlap(const PxGeometry& hitboxGeometry, const PxTransform& hitBoxPose, const PxOverlapHit& overlap, OUT COL_HIT_INFO& outInfo)
{
	outInfo = {};

	if (overlap.actor == nullptr || overlap.shape == nullptr)
		return false;

	// 상대 Geometry정보와 Pose정보
	PxGeometryHolder otherHolder{ overlap.shape->getGeometry() };
	const PxGeometry& otherGeomtry = otherHolder.any();
	const PxTransform otherPose = PxShapeExt::getGlobalPose(*overlap.shape, *overlap.actor);

	PxVec3 vDir{ 0.f };
	PxF32 fDpeth{ 0.f };
	const _bool bOK = PxGeometryQuery::computePenetration(vDir, fDpeth, hitboxGeometry, hitBoxPose, otherGeomtry, otherPose);

	// 실패시 근사값 도출
	if (bOK == false)
	{
		PxVec3 vFallback = otherPose.p - hitBoxPose.p;
		const _float fLengthsq = vFallback.magnitudeSquared();
		if (fLengthsq < g_XMEpsilon.f[0])
			vFallback = PxVec3(0.f, 1.f, 0.f);
		else
			vFallback /= PxSqr(fLengthsq);

		outInfo.bHasHitPoint = true;
		outInfo.fDepth = 0.f;
		::memcpy(&outInfo.vRawNormal.x, &vFallback.x, sizeof(Vec3));
		::memcpy(&outInfo.vPosition.x, &otherPose.p.x, sizeof(Vec3));
		return true;
	}

	vDir *= -1.f;
	PxVec3 vHitNormal = vDir.getNormalized();
	PxVec3 vHitPoint = hitBoxPose.p + vDir * (fDpeth * 0.5f);
	outInfo.bHasHitPoint = true;
	outInfo.fDepth = fDpeth;
	::memcpy(&outInfo.vRawNormal.x, &vHitNormal.x, sizeof(Vec3));
	::memcpy(&outInfo.vPosition.x, &vHitPoint.x, sizeof(Vec3));
	return true;
}

#ifdef _DEBUG
void CActiveAttackOverlap::Render()
{
	m_pGameInstance->Physics_Render(m_tHitboxDesc->geometry.any(), m_pxTransform, DirectX::Colors::DarkTurquoise);
}

void CActiveAttackOverlap::Debug_Log(CGameObject* hitObject)
{
	wstring logHeader = m_strEventString;
	wstring leftInfo = {};
	wstring rightInfo = {};

	if (m_pOwner)
		leftInfo = m_pOwner->Get_WName() + L", ID : " + std::to_wstring(m_pOwner->Get_ID()) + L"\n";
	else
		leftInfo = L"NULL\n";

	if (hitObject)
		rightInfo = hitObject->Get_WName() + L", ID : " + std::to_wstring(hitObject->Get_ID()) + L"\n";
	else
		rightInfo = L"NULL\n";

	CLOG_INFO(logHeader + leftInfo + rightInfo);
}
#endif // _DEBUG

CActiveAttackOverlap* CActiveAttackOverlap::Create()
{
	CActiveAttackOverlap* pInstance = new CActiveAttackOverlap();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CActiveAttackOverlap");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CActiveAttackOverlap::Free()
{
	m_hitObjects.clear();

	Super::Free();

	Safe_Release(m_pGameInstance);
}
