#pragma once
#include "MonoBehaviour.h"

NS_BEGIN(Tool)

class CButton final : public CMonoBehaviour
{
	using Super = CMonoBehaviour;
public:
	typedef struct tagButtonDesc : public CMonoBehaviour::MONO_DESC
	{

	}BUTTON_DESC;

private:
	CButton();
	CButton(const CButton& rhs);
	virtual ~CButton() = default;

private:
	HRESULT Initialize_Prototype() override;
	HRESULT Initialize(void* pArg) override;
public:
	void Update(const _float fTimeDelta) override;

private:
	void OnClick();

public:
	static CButton* Create(const BUTTON_DESC& Desc);
	virtual CComponent* Clone(void *pArg)override;
	virtual void Free()override;

};

NS_END