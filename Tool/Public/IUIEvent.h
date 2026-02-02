#pragma once
#include "Base.h"

NS_BEGIN(Engine)
class CGameInstance;
NS_END

NS_BEGIN(Tool)
class CToolUI;
class IUIEvent abstract : public CBase
{
public:
	typedef struct tagUIEventDesc
	{
		CToolUI* pOwner;
	}UIEVENT_DESC;

protected:
	IUIEvent(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	IUIEvent(const IUIEvent& rhs);
	virtual ~IUIEvent() = default;

protected:
	virtual HRESULT Initialize_Prototype()PURE;
	virtual HRESULT Initialize(void* pArg);

protected:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pDeviceContext = { nullptr };

	CGameInstance* m_pGameInstance = { nullptr };

	CToolUI* m_pOwner = { nullptr };

public:
	virtual IUIEvent* Clone(void* pArg)PURE;
	virtual void Free()override;
};

NS_END