#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class CCollider;

class CCollision_Manager final : public CBase
{
	using Super = CBase;
	struct PairState
	{
		CCollider* pLeft = { nullptr };
		CCollider* pRight = { nullptr };
		_bool bIsColliding = { false };
		_bool bChecked = { false };
	};
	using PairMap = std::unordered_map<_uint64, PairState>;
private:
	CCollision_Manager(_uint iLayerCount);
	virtual ~CCollision_Manager() = default;

	HRESULT Initialize();
public:
	void Update(const _float fTimeDelta);
	void Check_Group(_uint iLeft, _uint iRight);
	HRESULT Register_Collider(CCollider* pCollider);
	HRESULT Unregister_Collider(CCollider* pCollider);
	void Clear();
private:
	_uint64 Make_PairKey(const CCollider* pA, const CCollider* pB);
	void Check_LayerPair(_uint iLeftLayer, _uint iRightLayer, const _float fTimeDelta);
	void CollideTest_Pair(CCollider* pLeft, CCollider* pRight);
private:
	_uint m_iLayerCount = { 0 };
	vector<vector<CCollider*>> m_CollidersByLayer;
	vector<_uint> m_vecCheckBit;
	PairMap m_umapPairs;
public:
	static CCollision_Manager* Create(_uint iLayerCount);
	virtual void Free() override;
};

NS_END