#pragma once
#include "UIObject.h"
#include "UIData_Repository.h"

NS_BEGIN(Engine)
class IUIActionForMe;
NS_END

NS_BEGIN(Tool)
class CToolUI final : public CUIObject
{
	using Super = CUIObject;
	using ActionFunc = std::function<void(IUIActionForMe*)>;

public:
	typedef struct tagToolUIDesc : public Super::UIOBJECT_DESC
	{
		_string strName;

		_string strCanvasName;
		uint32_t iCanvasIndex;
		_string strLayerName;
		uint32_t iLayerIndex;

		_string strInitTextureTag;
		uint32_t iInitTextureIndex;
		uint32_t iRectTransformType;
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
	_bool Calc_HitEvent();

public:
	HRESULT Bind_Action(DTO::EUIEvent EventType, DTO::EUIAction ActType, const json& params);
	HRESULT Remove_Action(DTO::EUIEvent EventType, DTO::EUIAction ActType);
	IUIActionForMe* Get_ActionForMe() const { return m_pActionForMe; }
	HRESULT Excute_Action(DTO::EUIEvent EventType);
	HRESULT ReBind_Action();

private:
	HRESULT Ready_Components(TOOLUI_DESC* pDesc);
	HRESULT Bind_ShaderResources();

	void SetUp_RectTransform_Position();
	void SetUp_Visible();

	void Acting_About_State();

	void Sync_Data();
#pragma region GETTER/SETTER
public:
	const _string& Get_Name()const { return m_strName; }
	void Set_Name(const _string& str) { m_strName = str; }
	ERectTransform Get_RectTransformType() const { return m_eRectTransformType; }
	void Set_RectTransformType(ERectTransform value) { m_eRectTransformType = value; }
	const _wstring& Get_TextureTag() const { return m_wstrTextureTag; }
	void Set_TextureTag(const _wstring& value) { m_wstrTextureTag = value; }
	uint32_t Get_TextureIndex() const { return m_iTextureIndex; }
	void Set_TextureIndex(uint32_t value) { m_iTextureIndex = value; }
	_float* Get_WIdth_Ptr() { return &m_fWidth; }
	_float* Get_Height_Ptr() { return &m_fHeight; }
	_float* Get_PosX_Ptr() { return &m_fX; }
	_float* Get_PosY_Ptr() { return &m_fY; }
	_float* Get_PosZ_Ptr() { return &m_fZ; }
	const Vec3& Get_RenderPos() const { return m_vRenderPos; }
	const RECT& Get_RenderRect() const { return m_tRenderRect; }
	void Set_HitTest() { m_isHitTest = TRUE; };
	const DTO::TUI_GenericUIData& Get_Data()const { return m_tUIData; }
	DTO::TUI_GenericUIData& Get_Data_Ref() { return m_tUIData; }

	vector<DTO::TUI_EventBindData>* Safe_Access_EventData(DTO::EUIEvent EventType);
	array< vector<DTO::TUI_EventBindData>, ENUM_TO_UINT(DTO::EUIEvent::END)>* Safe_Access_AllEventData();

#pragma endregion

private:	
	PrimitiveBatch<DirectX::VertexPositionColor>* m_pBatch = { nullptr };
	BasicEffect* m_pEffect = { nullptr };
	ID3D11InputLayout* m_pInputLayout = { nullptr };
private:
	DTO::TUI_GenericUIData m_tUIData = {};

	_string m_strName = {};
	_string m_strCanvasName = {};
	uint32_t m_iCanvasIndex = {};
	_string m_strLayerName = {};
	uint32_t m_iLayerIndex = {};

	ERectTransform m_eRectTransformType = { ERectTransform::C };
	_wstring m_wstrTextureTag = {};
	uint32_t m_iTextureIndex = {};

	Vec3 m_vRenderPos = {};
	RECT m_tRenderRect = {};
	_bool m_isHitTest = { FALSE };

	IUIActionForMe* m_pActionForMe = { nullptr };
	
	/* 액션들을 이벤트 갯수만큼 정적으로 할당 사실상 vector<ActionFunc>[] 이거임 */
	array< vector<ActionFunc> , ENUM_TO_UINT(DTO::EUIEvent::END)> m_vecBindingActions;
	array< vector<DTO::TUI_EventBindData>, ENUM_TO_UINT(DTO::EUIEvent::END)> m_vecBindingActionData;

public:
	static CToolUI* Create(EToolObjectType eType, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

NS_END

