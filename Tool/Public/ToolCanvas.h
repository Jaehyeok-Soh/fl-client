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
	vector<CToolLayer*> m_vecToolLayers;

	/* 아마 있어야될 변수 */
	/* 얘의 이동량에 따라 Tool Layers들에 전달? 아니면 바로 전달 */

public:
	static CToolCanvas* Create(EToolObjectType eType, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

NS_END

