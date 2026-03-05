#pragma once
#include "ImGui_Inspector.h"

NS_BEGIN(Client)

class CImGui_PoolInspector final : public CImGui_Inspector
{
	using Super = CImGui_Inspector;
private:
	enum ELayoutType : unsigned int
	{
		Object,
		Thred,
		COUNT
	};
private:
	CImGui_PoolInspector();
	virtual ~CImGui_PoolInspector() = default;

	HRESULT Initialize();
public:
	virtual void Render(CGameObject* pGo) override;
public:
	static CImGui_PoolInspector* Create();
	virtual void Free() override;
};

NS_END