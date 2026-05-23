#pragma once

#include <windows.h>

struct PacketInfo
{
	UINT32 ClientIndex = { 0 };
	UINT16 PacketId = { 0 };
	UINT16 DataSize = { 0 };
	char* pDataPtr = { 0 };
};