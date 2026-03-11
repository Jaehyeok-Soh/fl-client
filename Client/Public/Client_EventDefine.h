#pragma once

namespace Engine
{
	class CGameObject;
}

struct Test { using Signature = void(unsigned int iCount); };
struct Test2 { using Signature = void(unsigned int iCount); };

#pragma region Global Event

struct TUTORIAL_BOSS_CONTATCT		{ using Signature = void(); };
struct TUTORIAL_BOSS_CONTATCT_END	{ using Signature = void(); };

#pragma endregion


#pragma region Player Event
struct PLAYER_SKILL_TRIGGERED { using Signature = void(unsigned int); }; // 플레이어의 어떤 스킬이 사용된 순간의 Trigger // UI Skill Event 발생용
#pragma endregion

#pragma region UI Evnet

struct COMBO_ATTACK_EVENT_START { using Signature = void(); };
struct COMBO_ATTACK_EVENT_END	{ using Signature = void(); };

struct MONSTER_DEAD_EVENT_START { using Signature = void(CGameObject* ); }; // 몬스터 NamePlate 사라지게 하기

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

#pragma endregion
