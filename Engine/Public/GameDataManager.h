#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class CGameDataManager final : public CBase
{
	using Super = CBase;
private:
	CGameDataManager();
	virtual ~CGameDataManager() = default;

	HRESULT Initialize();
public:

private:

private:

private:

private:
	class CGameInstance *m_pGameInstance = { nullptr };
public:
	static CGameDataManager* Create();
	virtual void Free() override;
};

NS_END