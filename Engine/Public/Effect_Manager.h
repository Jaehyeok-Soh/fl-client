#pragma once
#include "Base.h"
#include "DataStruct_EffectEvent.h"
#include "DataStruct_Effect.h"

NS_BEGIN(Engine)

class CEffect_Manager :
    public CBase
{
	using Super = CBase;
private:
	CEffect_Manager();
	virtual ~CEffect_Manager() = default;

	HRESULT Initialize();
public:
	void Update(_float fTimeDelta);
	// 컴토넌트에서 호출하게 될 함수
	void Spawn_Effect(const std::string& strTag, const Matrix& matWorld, _float fDuration, _bool bIsLocal, void* pTargetBone = nullptr);
private:
	class CGameInstance* m_pGameInstance = { nullptr };
public:
	static CEffect_Manager* Create();
	virtual void Free() override;
};

NS_END