#pragma once
#include "UIObject.h"
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
		uint32_t iClientLevelIndex;

		uint32_t iEditorSizeX;
		uint32_t iEditorSizeY;
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
	void Calc_HitUpdate();
	void Sync_Data();
public:
	CToolUI* Calc_TopUI();
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
	Vec2 Get_LT() { return Vec2{ m_fX - m_fWidth * 0.5f , m_fY - m_fHeight * 0.5f }; }	// Left Top
	Vec2 Get_CT() { return Vec2{ m_fX,  m_fY - m_fHeight * 0.5f }; }					// Center Top
	Vec2 Get_RT() { return Vec2{ m_fX + m_fWidth * 0.5f , m_fY - m_fHeight * 0.5f }; }	// Right Top
	Vec2 Get_LC() { return Vec2{ m_fX - m_fWidth * 0.5f  , m_fY }; }					// Left Center
	Vec2 Get_C()  { return Vec2{ m_fX , m_fY }; }										// Center
	Vec2 Get_RC() { return Vec2{ m_fX + m_fWidth * 0.5f , m_fY }; }						// Right Center
	Vec2 Get_LB() { return Vec2{ m_fX - m_fWidth * 0.5f , m_fY + m_fHeight * 0.5f }; }	// Left Bottom
	Vec2 Get_CB() { return Vec2{ m_fX, m_fY + m_fHeight * 0.5f }; }						// Center Bottom
	Vec2 Get_RB() { return Vec2{ m_fX + m_fWidth * 0.5f , m_fY + m_fHeight * 0.5f }; }	// Right Bottom
	virtual _bool Export_Data(DTO::ECategory eCategory, CDataDocumentBase* pDocument) override;
	void Set_CaptureUI(CToolUI* pUI) { m_pCaptureUI = pUI; }
	const DTO::TUI_CanvasData& Get_Data()const { return m_tCanvasData; }
	DTO::TUI_CanvasData& Get_Data_Ref() { return m_tCanvasData; }

	void Clear();
private:
	PrimitiveBatch<DirectX::VertexPositionColor>* m_pBatch = { nullptr };
	BasicEffect* m_pEffect = { nullptr };
	ID3D11InputLayout* m_pInputLayout = { nullptr };
private:
	CImGui_UIManager* m_pUIManager = { nullptr };
	DTO::TUI_CanvasData m_tCanvasData = {};
	vector<CToolLayer*> m_vecToolLayers;
	uint32_t m_iClientLevelIndex = {};
	_string m_strTag;
	_bool m_isUsingViewport;

	CToolUI* m_pCaptureUI = {nullptr};
	CToolUI* m_pHoveringUI = { nullptr };
	array<CToolUI*, 2> m_ArrReleasedUI = {nullptr};
	_bool m_isPreUIPressing = { FALSE };
	_bool m_isPreHovering = { FALSE };

public:
	static CToolCanvas* Create(EToolObjectType eType, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

NS_END

