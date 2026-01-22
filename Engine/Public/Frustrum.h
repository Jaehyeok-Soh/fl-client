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
	_bool Culling(class CCollider* pCollider);
	void Update();
	void Clear();
private:
	class CGameInstance* m_pGameInstance = { nullptr };
	BoundingFrustum* m_pOriginBounding = { nullptr };
	BoundingFrustum* m_pWorldBounding = { nullptr };
public:
	static CFrustrum* Create();
	virtual void Free() override;
};

NS_END