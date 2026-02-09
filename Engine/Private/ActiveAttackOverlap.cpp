#include "Engine_pch.h"
#include "ActiveAttackOverlap.h"
#include "GameInstance.h"
#include "EngineConsole.h"

#include "GameObject.h"

CActiveAttackOverlap::CActiveAttackOverlap()
	: Super(),
	m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CActiveAttackOverlap::Initialize()
{
	m_strEventString = L"[On collision enter : Attac Overlap]\n";

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

			hitObject->OnCollision_Enter(m_tHitboxDesc->eFilterLayer, m_pOwner);
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

void CActiveAttackOverlap::Set(CPhysicsAttackOverlap::HITBOX_DESC* pDesc, Matrix ownerMatrix, CGameObject* pOwner)
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
