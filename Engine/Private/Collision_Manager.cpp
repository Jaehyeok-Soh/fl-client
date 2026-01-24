#include "Engine_pch.h"
#include "Collision_Manager.h"
#include "Collider.h"


CCollision_Manager::CCollision_Manager(_uint iLayerCount)
	: m_iLayerCount(iLayerCount)
{
	m_vecCheckBit.resize(m_iLayerCount);
	m_CollidersByLayer.resize(m_iLayerCount);
}

HRESULT CCollision_Manager::Initialize()
{
    return S_OK;
}

void CCollision_Manager::Update(const _float fTimeDelta)
{
	if (m_CollidersByLayer.empty())
		return;

	for (auto& pair : m_umapPairs)
		pair.second.bChecked = false;

	for (_uint iRow = 0; iRow < m_iLayerCount; ++iRow)
	{
		for (_uint iCol = iRow; iCol < m_iLayerCount; ++iCol)
		{
			if (m_vecCheckBit[iRow] & (1 << iCol))
				Check_LayerPair(iRow, iCol, fTimeDelta);
		}
	}

	// 검사 안된 pair 중, 이전 프레임 충돌 중이던 애들 Eixt 처리 후 제거 (SetAcitve 끈애들 꺼)
	for (auto itr = m_umapPairs.begin(); 
		itr != m_umapPairs.end();)
	{
		PairState& pairState = itr->second;
		// 충돌 중이였고?
		if (!pairState.bChecked && pairState.bIsColliding)
		{
			if (pairState.pLeft && pairState.pRight)
			{
				pairState.pLeft->OnCollision_Exit(pairState.pRight);
				pairState.pRight->OnCollision_Exit(pairState.pLeft);
			}
			pairState.bIsColliding = false;
		}

		if (!pairState.bIsColliding)
			itr = m_umapPairs.erase(itr);
		else
			++itr;
	}
}

void CCollision_Manager::Check_Group(_uint iLeft, _uint iRight)
{
	_uint iRow = iLeft;
	_uint iCol = iRight;

	if (iCol < iRow)
		std::swap(iRow, iCol);

	if (m_vecCheckBit[iRow] & (1 << iCol))
	{
		m_vecCheckBit[iRow] &= ~(1 << iCol);
	}		
	else
	{
		m_vecCheckBit[iRow] |= (1 << iCol);
	}
}

HRESULT CCollision_Manager::Register_Collider(CCollider* pCollider)
{
	_uint iLayer = pCollider->Get_Layer();
	if (iLayer >= m_iLayerCount)
		return E_FAIL;

	m_CollidersByLayer[iLayer].push_back(pCollider);
	return S_OK;
}

HRESULT CCollision_Manager::Unregister_Collider(CCollider* pCollider)
{
	if (pCollider == nullptr)
		return E_FAIL;

	_uint iLayer = pCollider->Get_Layer();
	if (iLayer >= m_iLayerCount)
		return E_FAIL;

	auto& vec = m_CollidersByLayer[iLayer];
	{
		auto itr = std::remove(vec.begin(), vec.end(), pCollider);
		vec.erase(itr, vec.end());
	}


	// Pair 쪽 정리하면서 Exit 호출
	{
		for (auto itr = m_umapPairs.begin();
			itr != m_umapPairs.end();)
		{
			PairState& pairState = itr->second;
			if (pairState.pLeft == pCollider || pairState.pRight == pCollider)
			{
				// 충돌 중이였고?
				if (pairState.bIsColliding && pairState.pLeft && pairState.pRight)
				{
					pairState.pLeft->OnCollision_Exit(pairState.pRight);
					pairState.pRight->OnCollision_Exit(pairState.pLeft);
				}

				itr = m_umapPairs.erase(itr);
			}
			else
				++itr;
		}
	}	

	return S_OK;
}

_uint64 CCollision_Manager::Make_PairKey(const CCollider* pA, const CCollider* pB)
{
	COLLIDER_ID id = {};
	_uint a = pA->Get_ID();
	_uint b = pB->Get_ID();

	id.iLeft_ID = (std::min)(a, b);
	id.iRight_ID = (std::max)(a, b);

	return id.ID;
}

void CCollision_Manager::Check_LayerPair(_uint iLeftLayer, _uint iRightLayer, const _float fTimeDelta)
{
	auto& vecLeft = m_CollidersByLayer[iLeftLayer];
	auto& vecRight = m_CollidersByLayer[iRightLayer];

	if (vecLeft.empty() || vecRight.empty())
		return;

	for (CCollider* pLeftCollider : vecLeft)
	{
		if (!pLeftCollider || pLeftCollider->Is_Active() == false)
			continue;

		for (CCollider* pRightCollider : vecRight)
		{
			if (!pRightCollider || pRightCollider->Is_Active() == false)
				continue;

			if (pLeftCollider == pRightCollider)
				continue;

			if (pLeftCollider->Get_Owner() == pRightCollider->Get_Owner())
				continue;

			// 같은 레이어에 중복 충돌 방지
			if (iLeftLayer == iRightLayer && (pLeftCollider->Get_ID() >= pRightCollider->Get_ID()))
				continue;

			CollideTest_Pair(pLeftCollider, pRightCollider);
		}
	}
}

void CCollision_Manager::CollideTest_Pair(CCollider* pLeft, CCollider* pRight)
{
	_uint64 iKey = Make_PairKey(pLeft, pRight);

	auto [itr, inserted] = m_umapPairs.try_emplace(iKey, PairState{});
	PairState& pairState = itr->second;

	pairState.pLeft = pLeft;
	pairState.pRight = pRight;

	// 한 프레임 중복 처리 방지
	if (pairState.bChecked)
		return;

	pairState.bChecked = true;

	_bool bNowColliding = pLeft->Intersect(pRight);

	if (bNowColliding)
	{
		if (!pairState.bIsColliding)
		{
			pLeft->OnCollision_Enter(pRight);
			pRight->OnCollision_Enter(pLeft);
		}
		else
		{
			pLeft->OnCollision(pRight);
			pRight->OnCollision(pLeft);
		}

		pairState.bIsColliding = true;
	}
	else
	{
		if (pairState.bIsColliding)
		{
			pLeft->OnCollision_Exit(pRight);
			pRight->OnCollision_Exit(pLeft);
		}
		
		pairState.bIsColliding = false;
	}
}

void CCollision_Manager::Clear()
{
	for (auto& Colliders : m_CollidersByLayer)
		Colliders.clear();

	::memset(m_vecCheckBit.data(), 0, sizeof(_uint) * m_iLayerCount);
	m_umapPairs.clear();
}

CCollision_Manager* CCollision_Manager::Create(_uint iLayerCount)
{
    CCollision_Manager* pInstance = new CCollision_Manager(iLayerCount);
    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("CCollision_Manager::Create, Failed");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CCollision_Manager::Free()
{
    Super::Free();
}
