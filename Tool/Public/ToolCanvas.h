#pragma once
#include "UIObject.h"
#include "Tool_Defines.h"
#include "UIData_Repository.h"

NS_BEGIN(Tool)

class CToolLayer;

class CToolCanvas final : public CUIObject
{
	using Super = CUIObject;
public:
	typedef struct tagToolUIDesc : public Super::UIOBJECT_DESC
	{
		_wstring wstrTextureTag;

	}TOOLUI_DESC;

private:
	CToolCanvas(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CToolCanvas(const CToolCanvas& rhs);
	virtual ~CToolCanvas() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg)override;

	virtual HRESULT Awake(const _uint iCurrentLevelID) override;
	virtual void Update_Priority(const _float fTimeDelta) override;
	virtual void Update(const _float fTimeDelta) override;
	virtual void Update_Late(const _float fTimeDelta) override;
	virtual void Ready_Before_Render(const _float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_Components(TOOLUI_DESC* pDesc);
	HRESULT Bind_ShaderResources();

private:
	CANVAS_DATA m_tData = {};
	vector<CToolLayer*> m_vecToolLayers;

public:
	static CToolCanvas* Create(EToolObjectType eType, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

NS_END

