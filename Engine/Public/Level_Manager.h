#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class CLevel_Manager : public CBase
{
	using Super = CBase;
private:
	CLevel_Manager();
	virtual ~CLevel_Manager() = default;
public:
	HRESULT	Awake();
	HRESULT Change_Level(_uint iNewLevelID, class CLevel* pNewLevel);
	void Update(_float fTimeDelta);
	void Update_Picking();
	HRESULT Render();
	_uint Get_CurrentLevelIndex() const { return m_iCurrentLevelID; }
	class CLevel* Get_CurrentLevel() { return m_pCurrentLevel; }
	const Vec4& Get_ClearColor() const;
	_bool Is_Awaked(const _uint iLevelID) const;
private:
	_bool m_bAwaked = { false };
	class CGameInstance* m_pGameInstance = { nullptr };
	class CLevel* m_pCurrentLevel = { nullptr };
	_uint m_iCurrentLevelID = { 0 };
public:
	static CLevel_Manager* Create();
	virtual void Free() override;
};

NS_END