#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class CFrustrum : public CBase
{
	using Super = CBase;
private:
	CFrustrum();
	virtual ~CFrustrum() = default;
public:
	HRESULT Initialize();
	void Update();
	void Clear();
	BoundingFrustum* Get_BoundingFrustrum_Local() { return m_pOriginBounding; }
	BoundingFrustum* Get_BoundingFrustrum_World() { return m_pWorldBounding; }
private:
	class CGameInstance* m_pGameInstance = { nullptr };
	BoundingFrustum* m_pOriginBounding = { nullptr };
	BoundingFrustum* m_pWorldBounding = { nullptr };
public:
	static CFrustrum* Create();
	virtual void Free() override;
};

NS_END