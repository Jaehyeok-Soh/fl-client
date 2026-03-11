#pragma once
#include "ImGui_Panel.h"
#include "Animation_Defines.h"

#include "Tool_Weapon.h"

/* 파츠에 대한 정보를 보여주고 재셋팅하게 한다. */

NS_BEGIN(Tool)

class CPanel_Parts final : public CImGui_Panel
{
private:
	using Super = CImGui_Panel;

private:
	explicit CPanel_Parts(const _char* pLabel, CLevel* pOwner, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CPanel_Parts() = default;

private:
	HRESULT	Initialize();

public:
	virtual HRESULT Render(CToolObject* pGo) override;
	virtual void Update(const _float fTimeDelta) override;

private:
	class CAnimTool_Manager* m_pAnimToolManager = { nullptr };
	// imgui
	EAnimEvent::Enum m_eEventTypeCombo = EAnimEvent::NONE;

	CTool_Weapon* m_pSelectedWeapon = { nullptr };

private:
	_int		m_iPartNums = { 0 };
	_int		m_iSelectPartIdx = { -1 };
	_bool		m_bCombine = { true };
	_bool		m_bRender = { true };

	_int m_iCurAnimationIdx = {};
	_int m_iSelecAnimationIdx = {};

	CTool_Weapon::WEAPON_INFO m_tInfo = {};

private:
	void Render_SelectPart();

	void Render_PartInfo();
	void Socket_Info();
	void State_Info();
	void SRT_Info();

	void Animation_Info();

private:
	void Set_PartObj();
	void Set_Socket();
	void Set_SRT(CTool_Weapon::SRT eSRT);

public:
	static			CPanel_Parts* Create(const _char* pLabel, CLevel* pOwner, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual void	Free() override;
};

NS_END

