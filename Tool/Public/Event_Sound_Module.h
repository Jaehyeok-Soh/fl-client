#pragma once
#include "Base.h"
#include "ToolAnimSoundPlayer.h"

NS_BEGIN(Engine)
class CGameInstance;
NS_END

NS_BEGIN(Tool)

class CAnimObj;

class CEvent_Sound_Module final : public CBase
{
	using Super = CBase;
private:
	CEvent_Sound_Module();
	virtual ~CEvent_Sound_Module() = default;

	HRESULT Initialize();
public:
	void Set_Owner(CAnimObj* pOwner);
	HRESULT Rebuild(const vector<DTO::SOUNDEVENT>& events);
	void Clear();
	CToolAnimSoundPlayer* Get_SoundPlayer() { return m_pSoundPlayer; }
private:
	HRESULT Ensure_SoundPlayer();
private:
	CGameInstance* m_pGameInstance{ nullptr };
	CAnimObj* m_pOwner{ nullptr };
	CToolAnimSoundPlayer* m_pSoundPlayer{ nullptr };
public:
	static CEvent_Sound_Module* Create();
	virtual void Free() override;
};

NS_END