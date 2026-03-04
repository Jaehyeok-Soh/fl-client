#pragma once
#include "Base.h"

NS_BEGIN(Engine)


class CLevel;
class CGameObject;

struct AddEventDesc
{
	_int iCloneLevelIndex = { -1 };
	wstring wstrLayerTag = { L"" };
	CGameObject* pClone = { nullptr };
	std::function<void(CGameObject*)> callback;
};

struct RemoveEventDesc
{
	_int iClonedLevelIndex = { -1 };
	CGameObject* pGo = { nullptr };
};

struct ChangeLevelEventDesc
{
	_int iNewLevelID = { -1 };
	CLevel* pNewLevel = { nullptr };
};


class CEvent_Manager final : public CBase
{
	using Super = CBase;
	enum class EventType : unsigned int
	{
		OBJECT_ADD = 0,
		OBJECT_REMOVE,
		LEVEL_CHANGE,
		END
	};
private:
	CEvent_Manager();
	virtual ~CEvent_Manager() = default;

	HRESULT Initialize();
public:
	void Push_AddEvent(const AddEventDesc &desc);
	void Push_RemoveEvent(const RemoveEventDesc &desc);
	void Push_ChangeLevelEvet(const ChangeLevelEventDesc &desc);
	void Flush_All();
	void Clear_All();
private:
	void Clear(EventType eType);
	void Flush(EventType eType);
	void Flush_Pending(EventType eType);
	HRESULT Add_GameObject(AddEventDesc &spawnDesc);
	HRESULT Remove_GameObject(RemoveEventDesc &despawnDesc);
	HRESULT Change_Level(ChangeLevelEventDesc& changeLevelDesc);
	void Clear_AddEvent(AddEventDesc& spawnDesc);
	void Clear_RemoveEvent(RemoveEventDesc &removeDesc);
	void Clear_ChangeLevelEvent(ChangeLevelEventDesc& changeLevelDesc);
private:
	_bool m_bFlushing = { false };
	class CGameInstance* m_pGameInstance = { nullptr };
	std::queue<AddEventDesc> m_queAddGameObject;
	std::queue<AddEventDesc> m_queAddGameObject_Pending;
	std::queue<RemoveEventDesc> m_queRemoveGameObject;
	std::queue<RemoveEventDesc> m_queRemoveGameObject_Pending;
	std::queue<ChangeLevelEventDesc> m_queChangeLevel;
	std::queue<ChangeLevelEventDesc> m_queChangeLevel_Pending;
public:
	static CEvent_Manager* Create();
	virtual void Free() override;
};

NS_END