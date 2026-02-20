#pragma once
#include "ImGui_Panel.h"
#include "Animation_Defines.h"
#include "Anim_Event_Info.h"

NS_BEGIN(Tool)

class CPanel_AnimDescription final : public CImGui_Panel
{
private:
	using Super = CImGui_Panel;

private:
	explicit CPanel_AnimDescription(const _char* pLabel, CLevel* pOwner, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CPanel_AnimDescription() = default;

private:
	HRESULT	Initialize();

public:
	virtual HRESULT Render(CToolObject* pGo) override;
	virtual void Update(const _float fTimeDelta) override;

public:
	void SetAnimationObject();

	BONEINFO GetBoneInfo(_uint index);
	ANIMINFO GetAnimInfo(_uint index);

	void ModifyOne(_uint eventIdx, DTO::ATTACKEVENT event);

	// window
private:
	void Description_TabWindow();
	void Desc_AttackOverlapWindow();
	void Desc_EffectWindow();
	//void Desc_BoneWindow();
	//void Desc_AnimWindow();

private:
	class CAnimTool_Manager* m_pAnimToolManager = { nullptr };

	ANIMCTRLINFO* m_tAnimControllInfo;

	DTO::ANIM_EVENT_INFO1* m_tEventInfo;

	// imgui
	_bool m_bModalOpen = { false };
private:


public:
	static			CPanel_AnimDescription* Create(const _char* pLabel, CLevel* pOwner, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual void	Free() override;
};

NS_END