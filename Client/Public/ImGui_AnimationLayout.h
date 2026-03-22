#pragma once
#include "ImGui_Layout.h"

/*
우선 플레이어 용으로만 만듦
*/

NS_BEGIN(Engine)
class CModel;
NS_END

NS_BEGIN(Client)

class CImGui_AnimationLayout final : public CImGui_Layout
{
	using Super = CImGui_Layout;
private:
	CImGui_AnimationLayout();
	virtual ~CImGui_AnimationLayout() = default;
public:
	virtual _bool Can_Render(CGameObject* pGo) override;
	virtual void Render(CGameObject* pGo) override;

private:
	void Update_ModelInfo();

	void Render_AnimationList();
	void Render_AnimationInfo();
	void Render_ChangeAnimInfo();
		
private:
	CModel* m_pPlayerModel = { nullptr };


	_uint m_iAnimNums = {};

	_wstring m_wstrSelectAnimName = {};

	_int	m_iSelectAnimation = {};
	_int	m_iChageAnimation = {};

	_float m_fSelectSpeed = {0.f};
	_float m_fChnageSpeed = { 0.f };

	_float m_fSelectOffset = { 0.f };
	_float m_fChnageOffset = { 0.f };

public:
	static CImGui_AnimationLayout* Create();
	virtual void Free() override;
};

NS_END;
