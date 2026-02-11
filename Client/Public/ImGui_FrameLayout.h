#pragma once
#include "ImGui_Layout.h"

NS_BEGIN(Client)

class CImGui_FrameLayout final : public CImGui_Layout
{
	using Super = CImGui_Layout;
private:
	CImGui_FrameLayout();
	virtual ~CImGui_FrameLayout() = default;
public:
	virtual _bool Can_Render(CGameObject* pGo) override { return true; }
	virtual void Render(CGameObject* pGo) override;
private:
	_float m_fAvgFrame{ 0.f };
	_float m_fMinFrame{ 0.f };

	_float m_fAvgNoLimitFrame{ 0.f };
	_float m_fMinNoLimitFrame{ 0.f };
	_float m_fBestNoLimitFrame{ 0.f };

	_float m_fUIAccSec{ 0.f };
	_float m_fDispDT60{ 0.f };
	_float m_fDispFPS60 = 0.f;
	_float m_fDispAvgFPS60 = 0.f;
	_float m_fDispMinFPS60 = 0.f;

	_float m_fDispDTNoLimit = 0.f;
	_float m_fDispFPSNoLimit = 0.f;
	_float m_fDispAvgFPSNoLimit = 0.f;
	_float m_fDispMinFPSNoLimit = 0.f;
	_float m_fDispMaxFPSNoLimit = 0.f;
public:
	static CImGui_FrameLayout* Create();
	virtual void Free() override;
};

NS_END