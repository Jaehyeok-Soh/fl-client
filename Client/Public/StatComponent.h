#pragma once
#include "MonoBehaviour.h"

NS_BEGIN(Client)

class CStatComponent : public CMonoBehaviour
{
	using Super = CMonoBehaviour;
public:
	typedef struct tagStatComponentDesc
	{
		_int iHealth = { 0 };
	}STATCOMP_DESC;
protected:
	CStatComponent();
	explicit CStatComponent(const CStatComponent& rhs);
	virtual ~CStatComponent() = default;

	virtual HRESULT Initialize_Prototype() override;

public:
	virtual HRESULT Awake(_uint iLevelIndex) override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Update(const _float fTimeDelta) override;

public:
	_float Get_HealthRatio() const { return (_float)m_iHealth / (_float)m_iMaxHealth; }
	void Add_Health(_int iHealth) { m_iHealth += iHealth; }
	_bool Is_HealthZero() const { return m_iHealth <= 0; }

protected:
	CGameInstance*	m_pGameInstance		= { nullptr };
	_int			m_iMaxHealth		= { 0 };
	_int			m_iHealth			= { 0 };

public:
	static CStatComponent* Create();
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END