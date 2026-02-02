#pragma once
#include "IUIEvent.h"

NS_BEGIN(Tool)

class CUIEvent_Hovering final : public IUIEvent
{
	using Super = IUIEvent;
public:
	enum class EEventType {

	};

	typedef struct tagUIEventHoveringDesc : public UIEVENT_DESC
	{
		EEventType eType;
	}UIEVENT_HOVERING_DESC;

private:
	CUIEvent_Hovering(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUIEvent_Hovering(const CUIEvent_Hovering& rhs);
	virtual ~CUIEvent_Hovering() = default;

public:
	HRESULT Initialize_Prototype() override;
	HRESULT Initialize(void* pArg) override;

public:
	static CUIEvent_Hovering* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	IUIEvent* Clone(void* pArg) override;
	virtual void Free()override;
};

NS_END