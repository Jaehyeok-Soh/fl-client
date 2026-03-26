#pragma once
#include "ImGui_Inspector.h"

NS_BEGIN(Client)

class CImGui_CameraToolInspector final : public CImGui_Inspector
{
	using Super = CImGui_Inspector;
public:
	enum ELayoutType
	{
		CameraShot = 0,
		COUNT
	};
private:
	CImGui_CameraToolInspector();
	virtual ~CImGui_CameraToolInspector() = default;

	HRESULT Initialize();
public:
	virtual void Render(CGameObject* pGo) override;
public:
	static CImGui_CameraToolInspector* Create();
	virtual void Free() override;
};

NS_END