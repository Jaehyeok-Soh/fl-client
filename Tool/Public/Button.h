#pragma once
#include "MonoBehaviour.h"

NS_BEGIN(Tool)
class CToolCanvas;
class CToolUI;
class CButton final : public CMonoBehaviour
{
	using Super = CMonoBehaviour;
public:
	typedef struct tagButtonDesc : public CMonoBehaviour::MONO_DESC
	{
		CToolUI* pOwner = { nullptr };
	}BUTTON_DESC;

private:
	CButton();
	CButton(const CButton& rhs);
	virtual ~CButton() = default;

private:
	HRESULT Initialize_Prototype() override;
	HRESULT Initialize_Prototype(const BUTTON_DESC& Desc);
	HRESULT Initialize(void* pArg) override;

public:
	void Update(const _float fTimeDelta) override;
	void OnClick();
	 
private:
	CToolUI* m_pOwner = { nullptr };

	/* ¸¶¿ì½º¶û »óÈ£ÀÛ¿ë µÆÀ» ¶§ ¹» ÇÒ²«Áö */

public:
	static CButton* Create(const BUTTON_DESC& Desc);
	virtual CComponent* Clone(void *pArg)override;
	virtual void Free()override;

};

NS_END