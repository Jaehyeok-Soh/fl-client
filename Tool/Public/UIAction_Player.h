#pragma once
#include "MonoBehaviour.h"

/* 지연 실행 큐 관리 (Delay Queue) */
/* ActionFunc 실행 (me/target 컨텍스트로 호출) */
/* (추가 옵션) 타임라인 재생까지 확장 가능* /
* 
NS_BEGIN(Tool)

class CToolUI;
class CUIAction_Player final : public CMonoBehaviour
{
	using Super = CUIAction_Player;

private:
	CUIAction_Player();
	virtual ~CUIAction_Player() = default;

public:
	HRESULT Initialize_Prototype() override;
	HRESULT Initialize(void* pArg) override;
	void Update(const _float fTimeDelta) override;

private:
	  
public:
	static CUIAction_Player* Create();
	CComponent* Clone(void* pArg);
	virtual void Free() override;
};

NS_END
