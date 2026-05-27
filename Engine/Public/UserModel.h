#pragma once

#include <windows.h>
#include "Engine_Define.h"

NS_BEGIN(Engine)

struct UserModel
{
	INT32 ClientIndex = { 0 };
	char UserID[MAX_USER_ID_LEN + 1] = { 0, };
};

NS_END