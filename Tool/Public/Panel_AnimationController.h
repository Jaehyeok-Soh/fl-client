#pragma once
#include "ImGui_Panel.h"
#include "Animation_Defines.h"

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
	void SetAnimationObject(CAnimObj* pObject);

	BONEINFO GetBoneInfo(_uint index);
	ANIMINFO GetAnimInfo(_uint index);

	// window
private:
	void AnimationListWindow();
	void BoneListWindow();
	void AnimationControllPanelWindow();
	void ButtonsWindow();

	void DrawController();

private:
	// 오브젝트와 정보 다시 가져오기
	void SetAnimControllInfo();
	void SetAnimationInfo();
	void SetBoneInfo();

	// 오브젝트가 nullptr이 아닌지 검사
	_bool ValidCheck();

private:
	void Update_Animation(const _float fTimeDelta);

	void ChangeAnimation(_uint iIndex);
	
	// 매 프레임 가져오는 애니메이션 정보
	void UpdateAnimationInfo();

private:
	CAnimObj* m_pCurrentObject = { nullptr };
	CModel* m_pModel = { nullptr };
	_uint  m_iCurrentAnimationState = {};
	vector<class CBone*> m_vecBones;
	vector<class CModelAnimation*> m_vecAnimations;

	ANIMCTRLINFO m_tAnimControllInfo{};

	ANIM_EVENT_INFO m_tEventInfo{};

public:
	static			CPanel_AnimationController* Create(const _char* pLabel, CLevel* pOwner, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual void	Free() override;
};

NS_END