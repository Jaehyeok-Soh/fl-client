#pragma once
#include "GenericUI.h"
#include "DataStruct_UI.h"

NS_BEGIN(Client)
class CStatComponent;
class CUIProgress_Bar abstract : public CGenericUI
{
	using Super = CGenericUI;
public:
	typedef struct tagUIProgressBarDesc : public GENERIC_UI_DESC
	{
		DTO::EUISubClassType eOwner;
	}PROGRESS_BAR_DESC;
protected:
	CUIProgress_Bar(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CUIProgress_Bar(const CUIProgress_Bar& rhs);
	virtual ~CUIProgress_Bar() = default;
public:
	HRESULT Initialize_Prototype() override;
	HRESULT Initialize(void* pArg) override;
public:
	virtual HRESULT Awake(const _uint iCurrentLevelID) override;
	virtual void Update_Priority(const _float fTimeDelta) override;
	virtual void Update(const _float fTimeDelta) override;
	virtual void Update_Late(const _float fTimeDelta) override;
	virtual void Ready_Before_Render(const _float fTimeDelta) override;
	virtual HRESULT Render() override;
protected:
	HRESULT Ready_Components(PROGRESS_BAR_DESC* pDesc);
	HRESULT Bind_ShaderResources();
protected:
	DTO::EUISubClassType m_eSubClassType = {};
public:
	virtual void Free()override;
};

NS_END