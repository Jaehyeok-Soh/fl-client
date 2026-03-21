#pragma once
#include "ObjectDataBase.h"
#include "Anim_Event_Base.h"

NS_BEGIN(DTO)

typedef struct tagSoundEvent : public ANIM_EVENT_BASE1
{
    _uint   iCommand = { 0 };
    string  strSoundTag = {};
    _float  fVolume = { 1.f };
    _float  fPitch = { 1.f };
    _float  fDelay = { 0.f };
    _bool   bSteal = { false };
    
    // Controlled Àü¿ë
    _int    iControlledId = { -1 };
    _bool   bLoop = { false };
}SOUNDEVENT;

typedef struct tagSoundEventInfoDesc
{
    string              strOwnerTag = { "Sample" };
    vector<SOUNDEVENT>  vecSoundEvents;
}SOUND_EVENT_INFO_DESC;

void to_json(json& j, const SOUNDEVENT& data);
void from_json(const json& j, SOUNDEVENT& data);
void to_json(json& j, const SOUND_EVENT_INFO_DESC& data);
void from_json(const json& j, SOUND_EVENT_INFO_DESC& data);

NS_END