#pragma once

#include "PacketHeader.h"

NS_BEGIN(Engine)

#pragma pack(push, 1)
struct CHARACTER_SYNC_PACKET : public PACKET_HEADER
{
    INT32 ClientIndex = 0;
    UINT32 TimeStamp = 0;
    UINT32 Sequence = 0;

    float PosX = 0.f;
    float PosY = 0.f;
    float PosZ = 0.f;
    float RotY = 0.f;

    INT32 StateFlag = 0;
    INT32 AnimIndex = 0;
};
#pragma pack(pop)

NS_END