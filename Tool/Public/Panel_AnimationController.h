#pragma once
#include "ImGui_Panel.h"
#include "Animation_Defines.h"
#include "Anim_Event_Info.h"

NS_BEGIN(Tool)

class CPanel_AnimationController final : public CImGui_Panel
{
private:
	using Super = CImGui_Panel;

private:
	explicit CPanel_AnimationController(const _char* pLabel, CLevel* pOwner, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CPanel_AnimationController() = default;

private:
	HRESULT	Initialize();

public:
	virtual HRESULT Render(CToolObject* pGo) override;
	virtual void Update(const _float fTimeDelta) override;

public:
	void SetAnimationObject();

	BONEINFO GetBoneInfo(_uint index);
	ANIMINFO GetAnimInfo(_uint index);

	// window
private:
	void AnimationListWindow();
	void BoneListWindow();
	void AnimationControllPanelWindow();
	void ButtonsWindow();

	void DrawController();

	// 이벤트 추가 모달
private:
	void Render_AddEventModal();

private:
	class CAnimTool_Manager* m_pAnimToolManager = { nullptr };

	ANIMCTRLINFO* m_tAnimControllInfo;

	DTO::ANIM_EVENT_INFO1* m_tEventInfo;

	// imgui
	EAnimEvent::Enum m_eEventTypeCombo = EAnimEvent::NONE;
	_float		 m_fGlobalTimeScale = 1.f;
	_float		 m_fTimeDebug = 1.f;


private:
	_float m_fTimeScale = { 1.f };

public:
	static			CPanel_AnimationController* Create(const _char* pLabel, CLevel* pOwner, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual void	Free() override;
};

NS_END