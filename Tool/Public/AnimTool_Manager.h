#pragma once
#include "Base.h"
#include "Animation_Defines.h"

#include "Level_Animation.h"

#include "Anim_Event_Info.h"
#include "Anim_Event_Base.h"

NS_BEGIN(Engine)
class  CGameInstance;
NS_END

NS_BEGIN(Tool)

class CAnimTool_Manager final : public CBase
{
public:
	using Super = CBase;
	DECLARE_SINGLETON(CAnimTool_Manager)

private:
	CAnimTool_Manager();
	virtual ~CAnimTool_Manager() = default;

public:
	HRESULT Initialize_AnimTool(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	
	void Update(const _float& fTimeDelta);

	void Render();

/// <summary>
/// 애니메이션 툴 공용 GUI
/// </summary>
public:
	/// <summary>
	/// 확인, 취소 모달
	/// 같은 imbui::begin, imgui::end 안에서
	/// ImGui::OpenPopup("{strModalId}"); 이 호출되야함
	/// </summary>
	/// <param name="strModalId">모달 id</param>
	/// <param name="message">메세지 id</param>
	/// <returns>-1 : 대기, 0 : 취소, 1 : 확인</returns>
	_int Render_ConfirmModal(string strModalId, string message);
	void Open_ConfirmModal(string strModalId);

/// <summary>
/// 외부에 애니메이션 정보 건내주기
/// </summary>
public:
	ANIMCTRLINFO& Get_AnimControllInfo() { return m_tAnimControllInfo; }
	DTO::ANIM_EVENT_INFO1& Get_AnimEventInfo() { return m_tEventInfo; }

/// <summary>
/// 애니메이션 모델과 애니메이션
/// </summary>
public:
	// 오브젝트와 정보 다시 가져오기
	void SetAnimationObject(CAnimObj* pObject, fs::path animModelPath);
	void SetAnimControllInfo();
	void SetAnimationInfo();
	void SetBoneInfo();

	// 오브젝트가 nullptr이 아닌지 검사
	_bool ValidCheck();

	// 애니메이션 트랙포지션 업데이트
	void Update_Animation(const _float& fTimeDelta);

	// 애니메이션 바꾸기
	void ChangeAnimation(_uint iIndex);

	// root motion bone 정보 모델에게 넘겨주기
	void Set_RootBone(_int iBoneIdx);
	void Set_RootOffset(_uint iAnimIdx, _float fOffset);

/// <summary>
/// 모듈(컴포넌트) 컨트롤
/// </summary>
public:
	// 히트박스 리셋
	void Awake_AttackOverlap();
	void Modify_AttackOverlap(_uint eventIdx, DTO::ATTACKEVENT event); // 히트박스 수정
	void Modify_AttackOverlap(vector<DTO::ATTACKEVENT> events); // 전체 히트박스 수정

	//void Modify_EffectEvent(_uint eventIdx, DTO::ANIM_EVENT_INFO_DESC event);
	void Modify_EffectEvent(vector<DTO::EFFECTEVENT> events);
	HRESULT EffectEvent_GizmoObjectSetting();
private:
	// 매 프레임 가져오는 애니메이션 정보
	void UpdateAnimationInfo();

/// <summary>
/// 모듈 관련
/// </summary>
private:
	// 모듈 업데이트
	void Update_Module(const _float& fTimeDelta);
	// 모듈 렌더
	void Render_Module();
	// 모듈들(컴포넌트)에 Set Owner
	void SetModuleOwner();

/// <summary>
/// save load
/// </summary>
public:
	HRESULT Load_AttackOverlap(fs::path path);
	HRESULT Load_EffectEvent(fs::path path);
	void Set_AttackOverlap(CPhysicsAttackOverlap* pAttackOverlap);
	void Set_EffectEvent(CEffectHandler* pEffectEvent);
	HRESULT Save_AttackOverlap(fs::path path, string strAnimTag, _int iPool);
	HRESULT Save_EffectEvent(fs::path path, string strAnimTag, _int iPool);

private:
	HRESULT Ready_Builder();
	HRESULT Ready_BuildFiles();
	HRESULT Build_File(_uint iLevelID, DTO::ECategory eCategory, string strFileKey);

/// <summary>
/// 이벤트
/// </summary>
private:
	HRESULT Ready_Event();
	HRESULT Release_Event();

private:
	ID3D11Device* m_pDevice{};
	ID3D11DeviceContext* m_pDeviceContext{};

	CGameInstance* m_pGameInstance{ nullptr };

	CBuilderSystem* m_pBuilderSystem = { nullptr };

	std::array<DelegateHandle, CLevel_Animation::Event::END> m_EventHandles;

/// <summary>
/// 애니메이션 툴 매니저 모듈
/// </summary>
private:
	class CEvent_Overlap_Module* m_pOverlapModule = { nullptr };
	class CEvent_Effect_Module* m_pEffectModule = { nullptr };

private:
	std::vector<string> m_vecEffectTags;	// 이펙트 전용 Tag 리스트들.
public:
	const vector<string>& Get_LoadedEffectTags() { return m_vecEffectTags; }
/// <summary>
/// 애니메이션 컨트롤 정보
/// 이벤트 정보
/// </summary>
private:
	ANIMCTRLINFO m_tAnimControllInfo{};
	DTO::ANIM_EVENT_INFO1 m_tEventInfo{};

public:
	virtual void Free() override;
};

NS_END