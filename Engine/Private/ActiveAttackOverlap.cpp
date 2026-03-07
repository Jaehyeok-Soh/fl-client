#include "Engine_pch.h"
#include "ActiveAttackOverlap.h"
#include "GameObject.h"
#include "EngineConsole.h"
#include "GameInstance.h"

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
		m_hitBuffer,
		m_tHitboxDesc->filterData,
		(PxQueryFilterCallback*)m_tHitboxDesc->filterCallback))
	{
		for (PxU32 i = 0; i < m_hitBuffer.nbTouches; i++)
		{
			CGameObject* hitObject = static_cast<CGameObject*>(m_hitBuffer.touches[i].actor->userData);
			
			if (CheckAlreadyHit(hitObject))
			{
				//m_pGameInstance->Overlap_EventCallback(m_pOwner, &m_hitBuffer, PxPairFlag::eNOTIFY_TOUCH_PERSISTS);
				continue;
			}

#ifdef _DEBUG
			//Debug_Log(hitObject);
#endif // _DEBUG

			m_pGameInstance->Overlap_EventCallback(m_pOwner, m_pxTransform.p, &m_hitBuffer.touches[i], PxPairFlag::eNOTIFY_TOUCH_FOUND, m_tHitboxDesc);

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
	HitObjectsClear();
	m_vecHitResults.clear();
}

void CActiveAttackOverlap::Set(DTO::HITBOX_DESC* pDesc, Matrix ownerMatrix, CGameObject* pOwner)
{
	m_pOwner = pOwner;

	m_tHitboxDesc = pDesc;
	
	m_matTransform = m_tHitboxDesc->matOffset * ownerMatrix;
	m_pxTransform = m_pGameInstance->XMMatrixToPxTransform(m_matTransform);

	m_vecHitResults.resize(m_tHitboxDesc->iMaxHit);
	m_hitBuffer.touches = m_vecHitResults.data();
	m_hitBuffer.maxNbTouches = m_tHitboxDesc->iMaxHit;
}

void CActiveAttackOverlap::Tick(_float fTimeDelta)
{
	if (m_tHitboxDesc->fTickTime < 0)
		return;

	m_fSumTickTime += fTimeDelta;
	if (m_fSumTickTime >= m_tHitboxDesc->fTickTime)
	{
		m_fSumTickTime = 0.f;

		HitObjectsClear();
	}
}

_bool CActiveAttackOverlap::CheckAlreadyHit(CGameObject* hitObject)
{
	auto obj = m_hitObjects.find(hitObject);
	if (obj != m_hitObjects.end())
		return true;

	return false;
}

void CActiveAttackOverlap::HitObjectsClear()
{
	//for (auto& obj : m_hitObjects)
	//	m_pGameInstance->Overlap_EventCallback(m_pOwner, &m_hitBuffer, PxPairFlag::eNOTIFY_TOUCH_LOST);
	m_hitObjects.clear();
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
