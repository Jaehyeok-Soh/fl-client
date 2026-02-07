#pragma once
#include "MonoBehaviour.h"

NS_BEGIN(Engine)

class CUIButton_Component final : public CMonoBehaviour
{
	using Super = CMonoBehaviour;
public:
	typedef struct tagButtonComponentDesc
	{

	}BUTTON_COMPONENT_DESC;

protected:
	CUIButton_Component();
	explicit CUIButton_Component(const CMonoBehaviour& rhs);
	virtual ~CUIButton_Component() = default;

	virtual HRESULT Initialize_Prototype() override;
public:
	virtual HRESULT Awake(_uint iLevelIndex)override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Update(const _float fTimeDelta) override;
public:
	static CUIButton_Component* Create();
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END