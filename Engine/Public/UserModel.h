#pragma once

#include <windows.h>
#include "Engine_Define.h"

struct UserModel
{
	INT32 ClientIndex = { 0 };
	char UserID[MAX_USER_ID_LEN + 1] = { 0, };
};
