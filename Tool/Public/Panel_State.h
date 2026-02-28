#pragma once
#include "ImGui_Panel.h"
#include "Animation_Defines.h"
#include "DTO_MonsterState.h"

NS_BEGIN(Engine)
class  CGameInstance;
class CBuilderSystem;
NS_END

NS_BEGIN(Tool)

class CPanel_State final : public CImGui_Panel
{
	struct LOAD_OPTIONS
	{
		_char strStatePath[256] = "";

		// 모달을 열 때 초기화하는 함수
		void Reset() {
			memset(strStatePath, 0, 256);
		}
	};
	// TransitionModal 충돌 방지
	enum class ETransOwner { Local, Global, None };
private:
	using Super = CImGui_Panel;

private:
	explicit CPanel_State(const _char* pLabel, CLevel* pOwner, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CPanel_State() = default;

private:
	HRESULT	Initialize();

	HRESULT Ready_Builder();

public:
	virtual HRESULT Render(CToolObject* pGo) override;
	virtual void Update(const _float fTimeDelta) override;

	// window
private:
	void StateEditor();

private:
	void DrawTopLevelInfo();
	void DrawStateList();
	void DrawStateDetails();

	void DrawTimeCounter(const char* label, DTO::MONSTERTIME_COUNTER& counter);
	void DrawStateTransition(DTO::STATE_TRANSITION& transition, int index, ETransOwner eOwner);
	void DrawGlobalStateTransition();

	bool InputTextString(const char* label, std::string& str);

	void SyncStateNamesToSet();
	void SyncGlobalStateTransition();
	bool DrawStateComboBox(const char* label, std::string& selectedState);
	bool DrawAnimationComboBox(const char* label, std::string& selectedAnim);
	void DrawRegistryPicker(const _char* comboID, const _char* const* items, _int iCount, string& strIOvalue, string& strIOfilter);
	_bool PassFilter(const _char* item, const string& strFilter);
	HRESULT Save(fs::path path);
	HRESULT Load(fs::path path);

	// modal
private:
	LOAD_OPTIONS m_tLoadOptions; // 모달 상태 변수
	void OpenFileDialog(char* buffer, const char* filter);
	void SaveFileDialog(char* buffer, const char* filter);
	void OpenLoadModal();
	void OpenSaveModal();
	void RenderLoadModal();
	void RenderSaveModal();
	void RenderStateParamEditor(DTO::STATE_PARAM& param);
	void RenderFeatureEntryModal(DTO::MONSTER_STATEBASE_DESC& state);
	void RenderConditionEntryModal(DTO::STATE_TRANSITION& transition);
	void RenderConditionFeatureModal(DTO::MONSTER_STATEBASE_DESC& state);
public:
	void SetAnimationObject();

private:
	class CAnimTool_Manager* m_pAnimToolManager = { nullptr };
	CGameInstance* m_pGameInstance = { nullptr };
	CBuilderSystem* m_pBuilderSystem = { nullptr };

	ANIMCTRLINFO* m_tAnimControllInfo;

	// imgui
	_bool m_bModalOpen = { false };

	// FeatureEntry modal
	_bool m_bFeatModalOpen = false;
	_int  m_iFeatEditIndex = -1;
	DTO::FEATURE_ENTRY m_tFeatDraft{};

	// ConditionEntry modal
	ETransOwner m_eCondOwner = ETransOwner::Local;
	_bool m_bCondModalOpen = false;
	_int  m_iCondEditIndex = -1;
	_int  m_iCondTransIndex = -1; // 어떤 transition의 조건인지
	DTO::CONDITION_ENTRY m_tCondDraft{};

	// FeatureConditionEntry modal
	_int m_iCondFeatEditIndex = -1;
	DTO::CONDITIONFEATURE_ENTRY m_tCondFeatDraft{};

	// For. Popup
	_bool m_bReqOpenCondPopup = false;
	_bool m_bReqOpenFeatPopup = false;
	_bool m_bReqOpenCondFeatPopup = false;

	// For. Registry 검증
	_bool m_bStrictNameCheck = false;
	string m_errCondModal{ "" };
	string m_errFeatModal{ "" };
	string m_errCondFeatModal{ "" };
	string m_filterCond{ "" };
	string m_filterFeat{ "" };
	string m_filterCondFeat_Cond{ "" };
	string m_filterCondFeat_Feat{ "" };
private:
	DTO::MONSTERSTATE_DESC m_MonsterData;
	int m_iSelectedIndex = -1;

	vector<DTO::STATE_TRANSITION> m_vecGlobalStateTransition;

	char m_szGlobalSetBuf[256] = "";

public:
	static			CPanel_State* Create(const _char* pLabel, CLevel* pOwner, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual void	Free() override;
};

NS_END