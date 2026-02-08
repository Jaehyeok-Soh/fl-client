#pragma once
#include "MonoBehaviour.h"

NS_BEGIN(Tool)
class CToolUI;

class CUIProgress_Component final : public CMonoBehaviour
{
	using Super = CMonoBehaviour;
public:
	typedef struct tagProgressComponentDesc
	{
	}PROGRESS_COMPONENT_DESC;

	enum eFillDir
	{
		LEFT, RIGHT, UP, DOWN
	};

	typedef struct tagProgressInfoDesc
	{
		_float m_fMaxValue = {};
		_float m_fCurValue = {};
		eFillDir m_eFillDir = {};
		_float m_fDuration = {};
		_float m_fDelay = {};
		_float m_fRatio = {};
	}PROGRESS_DESC;

protected:
	CUIProgress_Component();
	explicit CUIProgress_Component(const CMonoBehaviour& rhs);
	virtual ~CUIProgress_Component() = default;

	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize_Prototype(CToolUI* pOwner);
public:
	virtual HRESULT Awake(_uint iLevelIndex)override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Update(const _float fTimeDelta) override;

	void Set_Value(const PROGRESS_DESC& Desc) { m_tValue = Desc; }
	PROGRESS_DESC& Get_Value_Ref() { return m_tValue; }
	const PROGRESS_DESC& Get_Value()const { return m_tValue; }

private:
	CToolUI* m_pOwner = { nullptr };
	PROGRESS_DESC m_tValue = {};

public:
	static CUIProgress_Component* Create(CToolUI* pOwner);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END