#pragma once
#include "ImGui_Panel.h"
#include "Animation_Defines.h"

NS_BEGIN(Tool)

class CPanel_ModelInfo final : public CImGui_Panel
{
private:
	using Super = CImGui_Panel;

private:
	explicit CPanel_ModelInfo(const _char* pLabel, CLevel* pOwner, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CPanel_ModelInfo() = default;

private:
	HRESULT	Initialize();

public:
	virtual HRESULT Render(CToolObject* pGo) override;
	virtual void Update(const _float fTimeDelta) override;

private:
	class CAnimTool_Manager* m_pAnimToolManager = { nullptr };
	// imgui
	EAnimEvent::Enum m_eEventTypeCombo = EAnimEvent::NONE;

private:
	_int m_iRootBondIdx = { -1 };
	_float m_fRootMotionOffset = { 1.f };

	_uint m_iCurAnimIdx = {};
	_wstring m_wstrCurAnimName = {};

	_bool m_bFirst = { true };

	// render
private:
	void Render_RootMotionInfo();

	void Render_AnimationInfo();
	void Anim_Info();
	void RootOffset_Info();

private:
	void Set_RootBone();
	void Set_RootOffset();

public:
	static			CPanel_ModelInfo* Create(const _char* pLabel, CLevel* pOwner, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual void	Free() override;
};

NS_END