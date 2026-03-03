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


#pragma region UI Evnet
struct BOSS_STAGING_EVENT_START	{ using Signature = void(); };
struct BOSS_STAGING_EVENT_END	{ using Signature = void(); };

struct COMBO_ATTACK_EVENT_START { using Signature = void(); };
struct COMBO_ATTACK_EVENT_END	{ using Signature = void(); };

struct MONSTER_DEAD_EVENT_START { using Signature = void(CGameObject* ); };
#pragma endregion
