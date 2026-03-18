#pragma once
#include "Quest_DataModel.h"

namespace Engine
{
	class CGameObject;
}

struct Test { using Signature = void(unsigned int iCount); };
struct Test2 { using Signature = void(unsigned int iCount); };

#pragma region Global Event

/* Xibi */
struct TUTORIAL_BOSS_CONTATCT		{ using Signature = void(); };
struct TUTORIAL_BOSS_CONTATCT_END	{ using Signature = void(); };

/* Xibi */
struct XIBI_CHANGE_STATE_BOSS_DIRECTION { using Signature = void();};
struct XIBI_CHANGE_STATE_BOSS_IDLE		{ using Signature = void();};


#pragma endregion


#pragma region Player Event
struct PLAYER_SKILL_TRIGGERED { using Signature = void(unsigned int); }; // 플레이어의 어떤 스킬이 사용된 순간의 Trigger // UI Skill Event 발생용
#pragma endregion

#pragma region UI Evnet

struct COMBO_ATTACK_EVENT_START { using Signature = void(); };
struct COMBO_ATTACK_EVENT_END	{ using Signature = void(); };

struct MONSTER_DEAD_EVENT_START { using Signature = void(CGameObject*); }; // 몬스터 NamePlate 사라지게 하기


// 연출관련으로 Delegate를 들고있고 

struct CINEMATIC_START { using Signature = void(); };			// UI 위 아래 나오는 연출
struct CINEMATIC_END { using Signature = void(); };				// UI 위 아래 들어가는 연출

struct XIBILA_BOSS_ACTION_ON { using Signature = void(); };		// UI 보스 이름 띄우기
struct XIBILA_BOSS_ACTION_OFF { using Signature = void(); };	// UI 보스 이름 넣기

struct XIBILA_BOSS_UI_ON { using Signature = void(); };			// 시빌라 체력바 켜기
struct XIBILA_BOSS_UI_OFF { using Signature = void(); };		// 끄기

struct BOSS_GROGGY { using Signature = void(); }; // 약점 노출

struct GUN_ON_HIT { using Signature = void(); };	// 총알을 적중했을 때  

struct TUTORIAL_POPUP_TRIGGER{ using Signature = void(EUITutorialPopUpTypeID ID); }; // 튜토리얼 팝업 띄우기
struct TUTORIAL_POPUP_CLEAR{ using Signature = void(EUITutorialPopUpTypeID ID); }; // 튜토리얼 팝업 내리고 클리어 띄우기

// 퀘스트 이벤트
struct QUEST_NOTIFY{ using Signature = void(DTO::QUEST_EVENT_SIGNATURE ID); }; // 퀘스트 업데이트
struct QUEST_CHANGE_SCENARIO_NOTIFY{ using Signature = void(); }; // 퀘스트 시나리오 바뀜
// CQuestManager::GetInstance()->Get_QuestInfo();
struct QUEST_CHANGE_CHAPTER_NOTIFY{ using Signature = void(); }; // 퀘스트 챕터 바뀜
// CQuestManager::GetInstance()->Get_QuestChapterInfo();
struct QUEST_ALL_COMPLETE{ using Signature = void(); }; // 모든 퀘스트 완료

// 대화 이벤트
struct DIALOGUE_BEGIN { using Signature = void(_int iId); }; // 대화 시작
struct DIALOGUE_SELECT { using Signature = void(_int iIndex); };
struct DIALOGUE_NEXT { using Signature = void(); };
struct DIALOGUE_PREV { using Signature = void(); };
struct DIALOGUE_END { using Signature = void(); };
struct DIALOGUE_CANCEL { using Signature = void(); }; // 대화 매니저만 사용


#pragma endregion
