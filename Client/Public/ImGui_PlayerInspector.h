#pragma once
#include "ImGui_Inspector.h"

NS_BEGIN(Client)

class CImGui_PlayerInspector final : public CImGui_Inspector
{
	using Super = CImGui_Inspector;
private:
	enum ELayoutType : unsigned int
	{
		Frame = 0,
		Transform,
		State,
		MoveState,
		COUNT
	};
private:
	CImGui_PlayerInspector();
	virtual ~CImGui_PlayerInspector() = default;

	HRESULT Initialize();
public:
	virtual void Render(CGameObject* pGo) override;
public:
	static CImGui_PlayerInspector* Create();
	virtual void Free() override;
};

NS_END