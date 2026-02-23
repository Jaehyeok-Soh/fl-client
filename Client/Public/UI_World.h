#pragma once
#include "GenericUI.h"
#include "DataStruct_UI.h"

NS_BEGIN(Client)
class CCanvas;
class CStatComponent;
class CUI_World abstract : public CGenericUI
{
	using Super = CGenericUI;
public:
	typedef struct tagUIWorldDesc : public GENERIC_UI_DESC
	{

	}WORLD_UI_DESC;

protected:
	CUI_World(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CUI_World(const CUI_World& rhs);
	virtual ~CUI_World() = default;

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
	HRESULT Ready_Components(WORLD_UI_DESC* pDesc);
	HRESULT Bind_ShaderResources();

public:
	CGameObject* Clone(void* pArg)PURE;
	virtual void Free()override;
};

NS_END