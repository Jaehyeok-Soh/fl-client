#pragma once
#include "Base.h"
#include "DataStruct_EffectEvent.h"
#include "DataStruct_Effect.h"
#include "EffectBase.h"

NS_BEGIN(Engine)

class CGameObject;
class CEffectHandler;

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
	void Request_Effect(CEffectHandler* handler, const std::string& UniqueEffectName, const std::string& strTag, EFFECT_SPAWN_DESC& Desc);
	void Request_Effect(const std::string& strTag, EFFECT_SPAWN_DESC& Desc);

	// TODO : EffectManager에서 Key(EffectOBject ID), Value(EffectHandler)을 가지는 map 컨테이너 관리.
	// EffectObject가 본인이 반환이 될 때 EffectHandler에게 알려줄 수 있도록 한다.
	void Notify_EffectDespawn(_uint iEffectID);
	void Push_EffectData(_uint HashTag, void* Desc);

	void* Find_EffectData(_uint HashTag);
	
private:
	class CGameInstance*											m_pGameInstance = { nullptr };
	std::unordered_map<_uint, CEffectHandler*>						m_mEffectData = {};
	std::map<_uint, CEffectBase::EFFECT_CONTAINERDESC>				m_EffectDescData = {};	// 해싱한 Data

public:
	static CEffect_Manager* Create();
	virtual void Free() override;
};

NS_END