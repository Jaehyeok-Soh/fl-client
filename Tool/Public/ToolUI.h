#pragma once
#include "UIObject.h"
#include "Tool_Defines.h"
#include "UIData_Repository.h"

NS_BEGIN(Tool)

class CToolUI final : public CUIObject
{
	using Super = CUIObject;
public:
	typedef struct tagToolUIDesc : public Super::UIOBJECT_DESC
	{
		_wstring wstrTextureTag;
		uint32_t iTextureIndex;
	}TOOLUI_DESC;

private:
	CToolUI(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CToolUI(const CToolUI& rhs);
	virtual ~CToolUI() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg)override;

	virtual HRESULT Awake(const _uint iCurrentLevelID) override;
	virtual void Update_Priority(const _float fTimeDelta) override;
	virtual void Update(const _float fTimeDelta) override;
	virtual void Update_Late(const _float fTimeDelta) override;
	virtual void Ready_Before_Render(const _float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Set_TextureIndex(uint32_t index) { m_iTextureIndex = index; }

private:
	HRESULT Ready_Components(TOOLUI_DESC* pDesc);
	HRESULT Bind_ShaderResources();

private:
	TOOLUI_DESC* m_tMyDesc = { nullptr };
	uint32_t m_iTextureIndex = {};

public:
	static CToolUI* Create(EToolObjectType eType, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

NS_END

