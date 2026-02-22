#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class CGameObject;
class CObjectPool_Manager;

class CObject_Manager final : public CBase
{
	using Super = CBase;
private:
	CObject_Manager();
	virtual ~CObject_Manager() = default;

	HRESULT Initialize(_uint iLevelCount, CObjectPool_Manager* pPoolManager);
public:
	HRESULT Awake(const _uint iCurrentLevelID);
	void Update_Priority(const _float fUnscaledTimeDelta, const _float fScaledTimeDelta);
	void Update(const _float fUnscaledTimeDelta, const _float fScaledTimeDelta);
	void Update_Late(const _float fUnscaledTimeDelta, const _float fScaledTimeDelta);
	void Ready_Before_Render(const _float fUnscaledTimeDelta, const _float fScaledTimeDelta);
	
	void Delete_GameObject(_uint iCloneLevelIndex, const wstring& wstrLayerTag, CGameObject* pGo);

	CGameObject* Add_GameObject(_uint iCloneLevelIndex, const wstring& wstrLayerTag, CGameObject* pGo);
	CGameObject* Add_GameObject(_uint iPrototypeLevelIndex, const wstring &wstrPrototypeTag,
		_uint iCloneLevelIndex, const wstring &wstrLayerTag, void* pArg);

	CGameObject* Get_GameObject(_uint iLevelIndex, const wstring& wstrLayerTag, _uint iIndex);
	CGameObject* Get_GameObject_Front(_uint iLayerIndex, const wstring& wstrLayerTag);
	CGameObject* Get_GameObject_Back(_uint iLayerIndex, const wstring& wstrLayerTag);
	list<CGameObject*>* Get_GameObject_List(_uint iLayerIndex, const wstring& wstrLayerTag);

	void Clear_Layer(_uint iLevelIndex, const wstring& wstrLayerTag);
	void Clear(_uint iLevelIndex);
private:
	class CLayer* Find_Layer(_uint iLevelIndex, const wstring& wstrLayerTag);
private:
	_uint m_iLevelCount = { 0 };
	class CGameInstance* m_pGameInstance = { nullptr };
	CObjectPool_Manager* m_pPoolManager = { nullptr };
	vector<map<const wstring, class CLayer*>> m_pLayers;
public:
	static CObject_Manager* Create(_uint iLevelCount, CObjectPool_Manager* pPoolManager);
	virtual void Free() override;
private:
	friend class CObjectPool_Manager;
};

NS_END