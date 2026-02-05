#pragma once
#include "Base.h"


// GameObject -> SetDead() PoolObject 분기
// GameObject -> ActiveIndex 부여
// GameObject -> Spawn, Despawn -> 완

NS_BEGIN(Engine)

class CGameObject;

class CObjectPool final : public CBase
{
	using Super = CBase;
private:
	CObjectPool();
	virtual ~CObjectPool() = default;

	HRESULT Initialize(const wstring& wstrLayerTag, void* pArg, CGameObject* pSeed, _uint iPoolCapacityCount);
public:
	CGameObject* Spawn(void* pArg);
	HRESULT Despawn(CGameObject* pGo);
	void All_Despawn();
public:
	_uint Get_ActiveCount() const { return m_iActiveCount; }
	CGameObject* Get_ActiveObjectAt(_uint iIndex);
	const wstring& Get_LayerTag() const { return m_wstrLayerTag; }
private:
	HRESULT Ready_Objects(void* pArg, CGameObject* pSeed);
private:
	wstring m_wstrLayerTag = { L"" };
	_uint m_iActiveCount = { 0 };
	size_t m_iTotalCount = { 0 };
	vector<CGameObject*> m_vecObjects;
public:
	static CObjectPool* Create(const wstring& wstrLayerTag, void *pArg, CGameObject* pSeed, _uint iPoolCapacityCount);
	virtual void Free() override;
};

NS_END