#pragma once
#include "UIObject.h"
#include "Tool_Defines.h"
#include "UIData_Repository.h"

NS_BEGIN(Tool)

class CImGui_UIManager;
class CToolLayer;

class CToolCanvas final : public CUIObject
{
	using Super = CUIObject;
public:
	typedef struct tagToolCanvasDesc : public Super::UIOBJECT_DESC
	{
		_string strTag;

	}TOOLCANVAS_DESC;

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
	HRESULT Ready_Components(TOOLCANVAS_DESC* pDesc);
	HRESULT Bind_ShaderResources();

public:
	HRESULT Safe_Add_Layer(CToolLayer* pLayer);

	vector<CToolLayer*>* Safe_Access_LayerObject_Vector_Ptr();
	CToolLayer* Safe_Access_LayerObject_Ptr(int32_t index);
	CToolLayer* Safe_Access_CurLayerObject_Ptr();

	const _string& Get_Tag() const { return m_strTag; }
	void Set_Tag(const _string& Tag) { m_strTag = Tag; }

	_bool Get_isUsingViewport() const { return m_isUsingViewport; }
	void Set_isUsingViewport(_bool is) { m_isUsingViewport = is; }

	_float* Get_Width_Ptr() { return &m_fWidth; }
	_float* Get_Height_Ptr() { return &m_fHeight; }
	_float* Get_PosX_Ptr() { return &m_fX; }
	_float* Get_PosY_Ptr() { return &m_fY; }
	_float* Get_PosZ_Ptr() { return &m_fZ; }

	const CANVAS_DATA& Get_Data()const { return m_tCanvasData; }
	CANVAS_DATA& Get_Data_Ref() { return m_tCanvasData; }

	void Sync_Data();
private:
	PrimitiveBatch<DirectX::VertexPositionColor>* m_pBatch = { nullptr };
	BasicEffect* m_pEffect = { nullptr };
	ID3D11InputLayout* m_pInputLayout = { nullptr };

private:
	CImGui_UIManager* m_pUIManager = { nullptr };

	CANVAS_DATA m_tCanvasData = {};
	vector<CToolLayer*> m_vecToolLayers;

	_string m_strTag;
	_bool m_isUsingViewport;

public:
	static CToolCanvas* Create(EToolObjectType eType, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

NS_END

