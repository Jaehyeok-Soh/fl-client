#pragma once

namespace Engine
{
	class CGameObject;
}

struct Test { using Signature = void(unsigned int iCount); };
struct Test2 { using Signature = void(unsigned int iCount); };

#pragma region Player Event
struct PLAYER_SKILL_TRIGGERED { using Signature = void(unsigned int); };
#pragma endregion

#pragma region UI Evnet
struct BOSS_STAGING_EVENT_START	{ using Signature = void(); };
struct BOSS_STAGING_EVENT_END	{ using Signature = void(); };

struct COMBO_ATTACK_EVENT_START { using Signature = void(); };
struct COMBO_ATTACK_EVENT_END	{ using Signature = void(); };

struct MONSTER_DEAD_EVENT_START { using Signature = void(CGameObject* ); };

struct ACTION1 { using Signature = void(); }; // UI 위 아래 나오는 연출
struct ACTION2 { using Signature = void(); }; // UI 위 아래 들어가는 연출
struct ACTION3 { using Signature = void(); }; // UI 보스 이름 띄우기
struct ACTION4 { using Signature = void(); }; // UI 보스 이름 넣기

#pragma endregion
