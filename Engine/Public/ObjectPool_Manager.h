#pragma once
#include "Base.h"

NS_BEGIN(Engine)
class CGameInstance;
class CObjectPool;
class CGameObject;

class CObjectPool_Manager final : public CBase
{
	using Super = CBase;
private:
	CObjectPool_Manager();
	virtual ~CObjectPool_Manager() = default;

	HRESULT Initialize(_uint iLevelCount);
public:
	HRESULT Regist_Pool(_uint iTargetLevelIndex, const wstring& wstrPoolTag, const wstring& wstrLayerTag, _uint iSeedLevelID, const wstring& wstrSeedPrototypeTag, void* pArg, _uint iPoolCapacityCount);
	CObjectPool* Get_Pool(_uint iLevelIndex, const wstring& wstrPoolTag);
	CGameObject* Spawn(_uint iLevelIndex, const wstring& wstrPoolTag, OUT wstring& wstrLayerTag_OUT, void* pArg);
	HRESULT Despawn(_uint iLevelIndex, const wstring& wstrPoolTag, CGameObject* pGo);
	_int Get_ActiveCount(_uint iLevelIndex, const wstring& wstrPoolTag);
	CGameObject* Get_ActiveObjectAt(_uint iLevelIndex, const wstring& wstrPoolTag, _uint iIndex);
	void All_Despawn_StaticLevel();
	void Clear(_uint iLevelIndex);
private:
	void All_Clear();
	CObjectPool* Find_Pool(_uint iLevelIndex, const wstring& wstrPoolTag);
	_bool Is_OutOfRange(_uint iIndex) const { return m_iLevelCount <= iIndex; }
private:
	_uint m_iLevelCount = { 0 };
	vector<unordered_map<wstring, CObjectPool*>> m_Pools;
public:
	static CObjectPool_Manager* Create(_uint iLevelCount);
	virtual void Free() override;
#ifdef _DEBUG
	void Collect_PoolTags(_uint iLevelIndex, vector<wstring>& outTags) const;
	_int Get_Capacity(_uint iLevelIndex, const wstring &wstrPoolTag);
#endif
};

NS_END
