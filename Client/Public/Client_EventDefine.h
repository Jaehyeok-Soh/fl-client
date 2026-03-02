#pragma once

namespace Engine
{
	class CGameObject;
}

struct Test { using Signature = void(unsigned int iCount); };
struct Test2 { using Signature = void(unsigned int iCount); };

#pragma region UI Evnet
struct BOSS_STAGING_EVENT_START	{ using Signature = void(); };
struct BOSS_STAGING_EVENT_END	{ using Signature = void(); };

struct COMBO_ATTACK_EVENT_START { using Signature = void(); };
struct COMBO_ATTACK_EVENT_END	{ using Signature = void(); };

struct MONSTER_DEAD_EVENT_START { using Signature = void(CGameObject* ); };
#pragma endregion
