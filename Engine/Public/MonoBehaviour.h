#pragma once
#include "Component.h"

NS_BEGIN(Engine)

class ENGINE_DLL CMonoBehaviour abstract : public CComponent
{
	using Super = CComponent;
public:
	constexpr static EComponentType _ID = EComponentType::SCRIPT;
	typedef struct tagMonoBehaviourDesc
	{

	}MONO_DESC;
protected:
	CMonoBehaviour();
	explicit CMonoBehaviour(const CMonoBehaviour& rhs);
	virtual ~CMonoBehaviour() = default;

	virtual HRESULT Initialize_Prototype() PURE;
public:
	virtual HRESULT Awake(_uint iLevelIndex) { return S_OK; }
	virtual HRESULT Initialize(void* pArg) PURE;
	virtual void Update(const _float fTimeDelta) PURE;
protected:

public:
	virtual CComponent* Clone(void* pArg) PURE;
	virtual void Free() override;
};

NS_END