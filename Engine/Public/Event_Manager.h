#pragma once
#include "Base.h"

NS_BEGIN(Engine)


class CLevel;
class CGameObject;

struct SpawnEventDesc
{
	_int iCloneLevelIndex = { -1 };
	wstring wstrLayerTag = { L"" };
	CGameObject* pClone = { nullptr };
	std::function<void(CGameObject*)> callback;
};

struct DespawnEventDesc
{
	_int iClonedLevelIndex = { -1 };
	wstring wstrLayerTag = { L"" };
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
		OBJECT_CREATE = 0,
		OBJECT_REMOVE,
		LEVEL_CHANGE,
		END
	};
private:
	CEvent_Manager();
	virtual ~CEvent_Manager() = default;

	HRESULT Initialize();
public:
	void Push_SpawnEvent(const SpawnEventDesc &desc);
	void Push_DespawnEvent(const DespawnEventDesc &desc);
	void Push_ChangeLevelEvet(const ChangeLevelEventDesc &desc);
	void Flush_All();
	void Clear_All();
private:
	void Clear(EventType eType);
	void Flush(EventType eType);
	void Flush_Pending(EventType eType);
	HRESULT Spawn_GameObject(SpawnEventDesc &spawnDesc);
	HRESULT Despawn_GameObject(DespawnEventDesc &despawnDesc);
	HRESULT Change_Level(ChangeLevelEventDesc& changeLevelDesc);
	void Clear_SpawnEvent(SpawnEventDesc& spawnDesc);
	void Clear_ChangeLevelEvent(ChangeLevelEventDesc& changeLevelDesc);
private:
	_bool m_bFlushing = { false };
	class CGameInstance* m_pGameInstance = { nullptr };
	std::queue<SpawnEventDesc> m_queSpawn;
	std::queue<SpawnEventDesc> m_queSpawn_Pending;
	std::queue<DespawnEventDesc> m_queDespawn;
	std::queue<DespawnEventDesc> m_queDespawn_Pending;
	std::queue<ChangeLevelEventDesc> m_queChangeLevel;
	std::queue<ChangeLevelEventDesc> m_queChangeLevel_Pending;
public:
	static CEvent_Manager* Create();
	virtual void Free() override;
};

NS_END