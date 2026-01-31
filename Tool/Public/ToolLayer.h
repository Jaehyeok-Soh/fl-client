#pragma once
#include "UIObject.h"
#include "Tool_Defines.h"
#include "UIData_Repository.h"

NS_BEGIN(Tool)

class CImGui_UIManager;
class CToolUI;

class CToolLayer final : public CUIObject
{
	using Super = CUIObject;
public:
	typedef struct tagToolLayerDesc : public Super::UIOBJECT_DESC
	{
		_string strTag = {};
	}TOOLLAYER_DESC;

private:
	CToolLayer(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CToolLayer(const CToolLayer& rhs);
	virtual ~CToolLayer() = default;

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
	HRESULT Ready_Components(TOOLLAYER_DESC* pDesc);
	HRESULT Bind_ShaderResources();

	void Sync_Data();

#pragma region GETTER/SETTER
public:
	HRESULT Safe_Add_UI(CToolUI* pUI);
	vector<CToolUI*>* Safe_Access_UIObject_Vector_Ptr();
	CToolUI* Safe_Access_UIObject_Ptr(int32_t index);
	CToolUI* Safe_Access_CurUIObject_Ptr();
	const _string& Get_Name() const { return m_strName; }
	void Set_Name(const _string& Tag) { m_strName = Tag; }
	_bool Get_isVisible() const { return m_isVisible; }
	void Set_isVisible(_bool is) { m_isVisible = is; }
	const DTO::TUI_LayerData& Get_Data()const { return m_tLayerData; }
	DTO::TUI_LayerData& Get_Data_Ref() { return m_tLayerData; }
#pragma endregion
private:
	CImGui_UIManager* m_pUIManager = { nullptr };
	DTO::TUI_LayerData m_tLayerData = {};
	vector<CToolUI*> m_vecToolUIs;

	_string m_strName = {};
public:
	static CToolLayer* Create(EToolObjectType eType, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

NS_END

