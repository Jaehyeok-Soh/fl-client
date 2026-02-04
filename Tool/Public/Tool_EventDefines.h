#pragma once

NS_BEGIN(Engine)
class CGameObject;
NS_END

struct ChangeSelectedObject { using Signature = void(CGameObject* pGo); };
