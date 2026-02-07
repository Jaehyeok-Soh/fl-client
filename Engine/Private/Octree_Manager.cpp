#include "Engine_pch.h"
#include "Octree_Manager.h"
#include "GameObject.h"
#include "Transform.h"
#include "EngineConsole.h"
#include "GameInstance.h"

COctree_Manager::COctree_Manager()
{
}

HRESULT COctree_Manager::Initialize(const OCTREE_DESC& desc)
{
	m_Desc = desc;
	m_pRoot = Create_Node(desc.rootBounds, desc.fLooseFactor, 0);
	if (m_pRoot == nullptr)
		return E_FAIL;

	return S_OK;
}

OCTREE_ENTRY* COctree_Manager::Register(CGameObject* pGo, RENDER_CATEGORY eCategory, const BoundingBox& AABB, _bool bDynamic)
{
	if (pGo == nullptr || m_pRoot == nullptr)
		return nullptr;

	// 이미 있다면 빼고 갱신
	if (m_umapEntries.find(pGo) != m_umapEntries.end())
		Unregister(pGo);

	OCTREE_ENTRY* pEntry = new OCTREE_ENTRY;
	pEntry->AABB = AABB;
	pEntry->pGo = pGo;
	pEntry->bDynamic = bDynamic;
	pEntry->eCategory = eCategory;
	Safe_AddRef(pGo);

#ifdef _DEBUG
	if (m_pRoot->looseBounds.Contains(pEntry->AABB) == ContainmentType::DISJOINT)
	{
		const string& strName = pEntry->pGo->Get_Name();
		Vec3 vPos = pEntry->pGo->Get_Component<CTransform>()->Get_Info(TRANSFORM_INFO_STATE::POS);
		string strPos{ std::to_string(vPos.x) + " " + std::to_string(vPos.y) + " " + std::to_string(vPos.z) };
		CLOG_INFO("=================");
		CLOG_INFO("오브젝트 이름: " + strName);
		CLOG_INFO("오브젝트 위치: " + strPos);
		CLOG_ERROR("COctree_Manager::Register(), Root사이즈가 너무 작습니다.Entry가 Root의 밖에 있습니다.");
		Safe_Release(pGo);
		Safe_Delete(pEntry);
		return nullptr;
	}
#endif

	m_umapEntries.emplace(pGo, pEntry);
	Insert(m_pRoot, pEntry);
	return pEntry;
}

void COctree_Manager::Unregister(CGameObject* pGo)
{
	auto itr = m_umapEntries.find(pGo);
	if (itr == m_umapEntries.end())
		return;

	OCTREE_ENTRY* pFindEntry = itr->second;

	// Owner와 해제
	Remove_FromOwner(pFindEntry);

	Safe_Release(pFindEntry->pGo);
	Safe_Delete(pFindEntry);
	m_umapEntries.erase(itr);
}

void COctree_Manager::Update_AABB(CGameObject* pGo, const BoundingBox& newAABB)
{
	auto itr = m_umapEntries.find(pGo);
	if (itr == m_umapEntries.end())
		return;

	OCTREE_ENTRY* pEntry = itr->second;
	pEntry->AABB = newAABB;

	// Owner 등록 안됬으면 넣기
	if (pEntry->pOwner == nullptr)
	{
		Insert(m_pRoot, pEntry);
		return;
	}

	// 갱신된 AABB가 LooseBound에 완전포함 안되어있다면 Owner로부터 빠지고 재삽입
	if (pEntry->pOwner->looseBounds.Contains(pEntry->AABB) != ContainmentType::CONTAINS)
	{
#ifdef _DEBUG
		CLOG_INFO("COctree_Manager::Update_AABB(), looseBounds로부터 벗어나 Reinsert 발생");
#endif
		Remove_FromOwner(pEntry);
		Insert(m_pRoot, pEntry);
	}
}

#ifdef _DEBUG
void COctree_Manager::Query_Visible(const BoundingFrustum& frustrum, RENDER_CATEGORY eCategory, OUT vector<CGameObject*>& outObjects, OUT OCTREE_QUERY_STATS* pDebugStat) const
{
	outObjects.clear();
	Query_Node(m_pRoot, frustrum, eCategory, outObjects, pDebugStat);
}
void COctree_Manager::Query_Node(OCTREE_NODE* pNode, const BoundingFrustum& frustrum, RENDER_CATEGORY eCtegory, OUT vector<CGameObject*>& outObjects, OUT OCTREE_QUERY_STATS* pDebugStat) const
{
	if (pNode == nullptr)
		return;

	// Entry Frustrum 테스트 시도 횟수
	++(*pDebugStat).iVisitedNodes;

	if (frustrum.Contains(pNode->looseBounds) == ContainmentType::DISJOINT)
		return;

	for (auto* pEntry : pNode->vecItems)
	{
		if (eCtegory != pEntry->eCategory)
			continue;

		// Entry Frustrum 테스트 시도 횟수
		++(*pDebugStat).iTestedEntries;

		if (frustrum.Contains(pEntry->AABB) != ContainmentType::DISJOINT)
			outObjects.push_back(pEntry->pGo);
	}

	for (_int i = 0; i < DIVISION; ++i)
	{
		if (pNode->pChilds[i] != nullptr)
			Query_Node(pNode->pChilds[i], frustrum, eCtegory, outObjects, pDebugStat);
	}
}
#else
void COctree_Manager::Query_Visible(const BoundingFrustum& frustrum, RENDER_CATEGORY eCategory, OUT vector<CGameObject*> &outObjects) const
{
	outObjects.clear();
	Query_Node(m_pRoot, frustrum, eCategory, outObjects);
}

void COctree_Manager::Query_Node(OCTREE_NODE* pNode, const BoundingFrustum& frustrum, RENDER_CATEGORY eCtegory, OUT vector<CGameObject*>& outObjects) const
{
	if (pNode == nullptr)
		return;

	if (frustrum.Contains(pNode->looseBounds) == ContainmentType::DISJOINT)
		return;

	for (auto* pEntry : pNode->vecItems)
	{
		if (eCtegory != pEntry->eCategory)
			continue;

		if (frustrum.Contains(pEntry->AABB) != ContainmentType::DISJOINT)
			outObjects.push_back(pEntry->pGo);
	}

	for (_int i = 0; i < DIVISION; ++i)
	{
		if (pNode->pChilds[i] != nullptr)
			Query_Node(pNode->pChilds[i], frustrum, eCtegory, outObjects);
	}
}
#endif
void COctree_Manager::Clear()
{
	for (auto& Pair : m_umapEntries)
	{
		if (OCTREE_ENTRY* pEntry = Pair.second)
		{
			Remove_FromOwner(pEntry);
			Safe_Release(pEntry->pGo);
			Safe_Delete(pEntry);
		}
	}
	m_umapEntries.clear();
	Destroy_Node(m_pRoot);
	m_pRoot = nullptr;
}

BoundingBox COctree_Manager::Make_Loose(const BoundingBox& boundingBox, _float fLooseFactor)
{
	BoundingBox newBox = boundingBox;
	newBox.Extents.x *= fLooseFactor;
	newBox.Extents.y *= fLooseFactor;
	newBox.Extents.z *= fLooseFactor;
	return newBox;
}

BoundingBox COctree_Manager::Make_ChildBounds(const BoundingBox& parent, _int iIndex)
{
	Vec3 parentCenter = parent.Center;
	Vec3 parentExtents = parent.Extents;

	Vec3 childExtents = parent.Extents * 0.5f;

	// 4등분
	_float fScaleX = (iIndex & 1) ? +1.f : -1.f;
	_float fScaleY = (iIndex & 4) ? +1.f : -1.f;
	_float fScaleZ = (iIndex & 2) ? +1.f : -1.f;

	Vec3 childCenter =
	{
		parentCenter.x + fScaleX * childExtents.x,
		parentCenter.y + fScaleY * childExtents.y,
		parentCenter.z + fScaleZ * childExtents.z
	};

	return BoundingBox(childCenter, childExtents);
}

_int COctree_Manager::Candidate_ChildIndex(const BoundingBox& node, const BoundingBox& object)
{
	// node의 center 기준 어디 쪽에 속하는가 ?
	_int iX = (object.Center.x >= node.Center.x) ? 1 : 0;
	_int iY = (object.Center.y >= node.Center.y) ? 1 : 0;
	_int iZ = (object.Center.z >= node.Center.z) ? 1 : 0;

	// 0 ~ 7
	return (iX) | (iZ << 1) | (iY << 2);
}

void COctree_Manager::Add_ToNode(OCTREE_NODE* pNode, OCTREE_ENTRY* pEntry)
{
	pEntry->pOwner = pNode;
	pEntry->iOwnerIndex = pNode->vecItems.size();
	pNode->vecItems.push_back(pEntry);
}

// vecItems에서 엔트리 똑 떼기
void COctree_Manager::Remove_FromOwner(OCTREE_ENTRY* pEntry)
{
	if (pEntry == nullptr || pEntry->pOwner == nullptr)
		return;

	OCTREE_NODE* pNode = pEntry->pOwner;
	const size_t iIndex = pEntry->iOwnerIndex;
	const size_t iLastIndex = pNode->vecItems.size() - 1;

#ifdef _DEBUG
	if (pEntry->iOwnerIndex >= pNode->vecItems.size())
		CLOG_ERROR("COctree_Manager::Remove_FromOwner(), iOwnerIndex가 비정상적입니다. 메모리 오염 발생 가능성 높음");
#endif

	// 삭제 된것과 마지막 Index 스왑 정렬
	if (iIndex != iLastIndex)
	{
		pNode->vecItems[iIndex] = pNode->vecItems[iLastIndex];
		pNode->vecItems[iLastIndex]->iOwnerIndex = iIndex;
	}
	pNode->vecItems.pop_back();

	pEntry->pOwner = nullptr;
	pEntry->iOwnerIndex = 0;
}

void COctree_Manager::Split(OCTREE_NODE* pNode)
{
#ifdef _DEBUG
	size_t iBefore = pNode->vecItems.size();
	size_t iMoved = 0;
#endif

	// 트리 상에 Leaf에 쌓인 Item들 중 Child에 완전 포함 되는것만 내려보냄
	vector<OCTREE_ENTRY*> vecReamin;
	vecReamin.reserve(pNode->vecItems.size());

	for (OCTREE_ENTRY* pEntry : pNode->vecItems)
	{
		const _int iIndex = Candidate_ChildIndex(pNode->bounds, pEntry->AABB);

		if (pNode->pChilds[iIndex] == nullptr)
		{
			BoundingBox childrenBound = Make_ChildBounds(pNode->bounds, iIndex);
			pNode->pChilds[iIndex] = Create_Node(childrenBound, m_Desc.fLooseFactor, pNode->iDepth + 1);
		}

		// 완전히 포함된다면, Insert하며 Owner세팅
		if (pNode->pChilds[iIndex]->looseBounds.Contains(pEntry->AABB) == ContainmentType::CONTAINS)
		{
			pEntry->pOwner = nullptr;
			Insert(pNode->pChilds[iIndex], pEntry);
		}
		// 그렇지 못한 Entry들은 Remain에 다시 넣기
		else
		{
#ifdef _DEBUG
			++iMoved;
#endif
			vecReamin.push_back(pEntry);
		}
	}

#ifdef _DEBUG
	string strLog{ "노드의 깊이: " + std::to_string(pNode->iDepth)
		+ "이전 아이템 사이즈: " + std::to_string(iBefore)
		+ "Remain에 넣은 횟수: " + std::to_string(iMoved)
		+ "이후 아이템 사이즈: " + std::to_string(pNode->vecItems.size()) };
	CLOG_INFO("COctree_Manager::Split(), " + strLog);
#endif

	pNode->vecItems = std::move(vecReamin);
	for (size_t i = 0; i < pNode->vecItems.size(); ++i)
	{
		pNode->vecItems[i]->pOwner = pNode;
		pNode->vecItems[i]->iOwnerIndex = i;
	}
}

void COctree_Manager::Insert(OCTREE_NODE* pNode, OCTREE_ENTRY* pEntry)
{
	if (pNode == nullptr || pEntry == nullptr)
		return;

	if (pNode->looseBounds.Contains(pEntry->AABB) == ContainmentType::DISJOINT)
	{
#ifdef _DEBUG
		if(pNode != m_pRoot)
		{
			const string& strName = pEntry->pGo->Get_Name();
			Vec3 vPos = pEntry->pGo->Get_Component<CTransform>()->Get_Info(TRANSFORM_INFO_STATE::POS);
			string strPos{ std::to_string(vPos.x) + " " + std::to_string(vPos.y) + " " + std::to_string(vPos.z) };
			CLOG_INFO("=================");
			CLOG_INFO("오브젝트 이름: " + strName);
			CLOG_INFO("오브젝트 위치: " + strPos);
			CLOG_INFO("노드 깊이: " + pNode->iDepth);
			CLOG_INFO("노드 아이템 개수: " + pNode->vecItems.size());
			CLOG_ERROR("COctree_Manager::Insert(), non-root node DISJOINT, 노드에 쌓입니다. bound, looseFactor를 조정하세요");
		}
#endif
		Add_ToNode(pNode, pEntry);
		return;
	}
	
	// 분할이 가능한 깊이인가 ?
	const _bool bCanSplit = pNode->iDepth < m_Desc.iMaxDepth;

	// 트리의 가장 끝에 있다면
	if (pNode->Is_Leaf() == true)
	{
		// TODO - 프롤로그 씬에서 iMaxItemsPerLeaf에 의해 Render가 빵꾸나면 이부분을 건드려야함
		// 더이상 분할이 불가능하다면 해당 노드에 추가
		if (bCanSplit == false || pNode->vecItems.size() < m_Desc.iMaxItemsPerLeaf)
		{
			Add_ToNode(pNode, pEntry);
			return;
		}

		// 분할을 하는데 최소 크기에 도달했다면
		_bool bTooSmall = (
			(pNode->bounds.Extents.x <= m_Desc.fMinNodeSizeXZ) ||
			(pNode->bounds.Extents.z <= m_Desc.fMinNodeSizeXZ)
			);

		if (bTooSmall)
		{
			Add_ToNode(pNode, pEntry);
			return;
		}
		else
			Split(pNode);
	}

	// 자식 Index 후보 검출
	const _int iIndex = Candidate_ChildIndex(pNode->bounds, pEntry->AABB);

	// 안전하게 할당된 자식이 없다면 자식 생성 후 노드 생성
	if (pNode->pChilds[iIndex] == nullptr)
	{
		BoundingBox childrenBox = Make_ChildBounds(pNode->bounds, iIndex);
		pNode->pChilds[iIndex] = Create_Node(childrenBox, m_Desc.fLooseFactor, pNode->iDepth + 1);
	}

	// 완전히 포함된다면 해당 Entry 등록
	if (pNode->pChilds[iIndex]->looseBounds.Contains(pEntry->AABB) == ContainmentType::CONTAINS)
	{
		Insert(pNode->pChilds[iIndex], pEntry);
		return;
	}

	Add_ToNode(pNode, pEntry);
}

void COctree_Manager::Destroy_Node(OCTREE_NODE* pNode)
{
	if (pNode == nullptr)
		return;

	for (_int i = 0; i < DIVISION; ++i)
	{
		Destroy_Node(pNode->pChilds[i]);
		pNode->pChilds[i] = nullptr;
	}
	Safe_Delete(pNode);
}

OCTREE_NODE* COctree_Manager::Create_Node(const BoundingBox& bound, _float fLooseFactor, _int iDepth)
{
	OCTREE_NODE* pNewNode = new OCTREE_NODE{};
	pNewNode->iDepth = iDepth;
	pNewNode->bounds = bound;
	pNewNode->looseBounds = Make_Loose(bound, fLooseFactor);
	return pNewNode;
}

COctree_Manager* COctree_Manager::Create()
{
	return new COctree_Manager();
}

void COctree_Manager::Free()
{
	Clear();
	Super::Free();
}
