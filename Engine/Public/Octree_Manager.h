#pragma once
#include "Base.h"

// (메인)Transform / Bounds 업데이트
// -> (메인) Octree::Update 
// -> (워커) Query_Visible
// -> (메인) Render

#define DIVISION 8

NS_BEGIN(Engine)

class CGameObject;
struct tagOctreeNode;

// 등록될 오브젝트 Entry
typedef struct tagOctreeEntry
{
	_bool bDynamic{ false };
	RENDER_CATEGORY eCategory{ RENDER_CATEGORY::NONEBLEND };
	CGameObject* pGo{ nullptr };
	tagOctreeNode* pOwner{ nullptr };
	BoundingBox AABB;
	size_t iOwnerIndex{ 0 };
}OCTREE_ENTRY;

typedef struct tagOctreeNode
{
	BoundingBox bounds;
	BoundingBox looseBounds;
	_int iDepth{ 0 };
	tagOctreeNode* pChilds[DIVISION]{nullptr};
	vector<tagOctreeEntry*> vecItems;

	_bool Is_Leaf() const
	{
		for (_int i = 0; i < DIVISION; ++i)
			if (pChilds[i] != nullptr)
				return false;
		return true;
	}
}OCTREE_NODE;

typedef struct tagOctreeQueryStats
{
	_uint iVisitedNodes{ 0 };
	_uint iTestedEntries{ 0 };
	_uint iVisibleOut{ 0 };
}OCTREE_QUERY_STATS;

class COctree_Manager final : public CBase
{
	using Super = CBase;
private:
	COctree_Manager();
	virtual ~COctree_Manager() = default;
public:
	HRESULT Initialize(const OCTREE_DESC& desc);
	OCTREE_ENTRY* Register(CGameObject* pGo, RENDER_CATEGORY eCategory, const BoundingBox& AABB, _bool bDynamic = false);
	void Unregister(CGameObject* pGo);
	void Update_AABB(CGameObject* pGo, const BoundingBox& newAABB);
private:
	void Query_Node(OCTREE_NODE* pNode, const BoundingFrustum& frustrum, RENDER_CATEGORY eCtegory, OUT vector<CGameObject*>& outObjects, OUT OCTREE_QUERY_STATS* pDebugStat) const;
public:
	void Query_Visible(const BoundingFrustum& frustrum, RENDER_CATEGORY eCategory, OUT vector<CGameObject*>& outObjects, OUT OCTREE_QUERY_STATS* pDebugStat) const;
#pragma region 나중에 안정화 될때는 아래 함수로 바꾸기
private:
	void Query_Node(OCTREE_NODE* pNode, const BoundingFrustum& frustrum, RENDER_CATEGORY eCtegory, OUT vector<CGameObject*>& outObjects) const;
public:
	void Query_Visible(const BoundingFrustum& frustrum, RENDER_CATEGORY eCategory, OUT vector<CGameObject*>& outObjects) const;
#pragma endregion
	void Clear();
private:
	BoundingBox Make_Loose(const BoundingBox &boundingBox, _float fLooseFactor);
	BoundingBox Make_ChildBounds(const BoundingBox& parent, _int iIndex);
	_int Candidate_ChildIndex(const BoundingBox& node, const BoundingBox& object);
	void Add_ToNode(OCTREE_NODE* pNode, OCTREE_ENTRY* pEntry);
	void Remove_FromOwner(OCTREE_ENTRY* pEntry);
	void Split(OCTREE_NODE* pNode);
	void Insert(OCTREE_NODE* pNode, OCTREE_ENTRY* pEntry);
	void Destroy_Node(OCTREE_NODE* pNode);
	OCTREE_NODE* Create_Node(const BoundingBox &bound, _float fLooseFactor, _int iDepth);
private:
	OCTREE_DESC m_Desc{};
	OCTREE_NODE* m_pRoot{ nullptr };
	unordered_map<CGameObject*, OCTREE_ENTRY*> m_umapEntries;
public:
	static COctree_Manager* Create();
	virtual void Free() override;
private:
#ifdef _DEBUG
	size_t m_iRegisteredStaticCount{ 0 };
	
#endif
};

NS_END

/*
* 하나의 노드를 8개로 분할
* 부모 노드는 8개의 자식노드로 분할 자식 노드들은 다시 8개의 자식노드로 분할하는 트리 구조
* 카메라에 부딪히는 공간을 노드로 가정, 해당 노드를 분할하여 카메라에 포함되는 오브젝트만 그려주기 위함
* -> 절두체와 노드가 충돌하여 해동 공간을 분할하고 정말 최대로 분할된 공간에 포함된 Object들만 렌더콜!
* 옥트리를 활용하지 않는다면, 물체들의 거리를 모두 판단해서 세밀함 정도(프로그레시브 메시)를 판단해야함
* 정적 오브젝트가 Level에 배치되고 해당 Layer를 들고와서 공간 트리를 형성, 물체가 노드들 사이에 끼어있는 경우에는
* 끼인면에 대해 분할을 시키거나 간단하게 느슨한 옥트리방식(느슨한 판정!)을 이용하여 설정한다.
*/