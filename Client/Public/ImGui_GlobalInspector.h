#pragma once
#include "ImGui_Inspector.h"

NS_BEGIN(Client)

class CImGui_GlobalInspector final : public CImGui_Inspector
{
	using Super = CImGui_Inspector;
private:
	enum ELayoutType : unsigned int
	{
		Frustrum = 0,
		Shader,
		COUNT
	};
private:
	CImGui_GlobalInspector();
	virtual ~CImGui_GlobalInspector() = default;

	HRESULT Initialize();
public:
	virtual void Render(CGameObject* pGo) override;
public:
	static CImGui_GlobalInspector* Create();
	virtual void Free() override;
};

NS_END