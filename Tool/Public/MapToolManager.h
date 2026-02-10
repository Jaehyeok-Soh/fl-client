#pragma once
#include "Base.h"




NS_BEGIN(Engine)

class  CGameInstance;
struct CLIENT_MAKEPATH_DESC_BASE;

NS_END

NS_BEGIN(Tool)

class CMapObject;
class CImGui_ToolManager;
class CLevel_Map;

using MapObjectCloneFactory = std::function<CGameObject*(void* pArg)>;
using PairKey = std::pair<wstring, vector<wstring>>;

typedef struct tagBrushModeOption
{
	/* 법선에 태울건지 안태울건지 옵션도 있으면 좋긴할듯 */
	bool	isUseBrushScale{false};
	bool	isUseBrushRotation{false};

	Vec3	vBrushScale{1.f,1.f,1.f};
	Vec3	vBrushRotation{0.f,0.f,0.f};

	
	bool	isOnNormal{false};

	bool	isUsePlacementSpacing{ false };
	float	fPlacementSpacing = 0.5f;

	bool	isUseRandomYRotation = false;
	Vec2	vRandomMinMaxRotaionRange = {0.f, 360.f};


	bool	isUseRandomScale = false;
	Vec2	vMinMaxScaleX = { 1.f,1.f };
	Vec2	vMinMaxScaleY = { 1.f,1.f };
	Vec2	vMinMaxScaleZ = { 1.f,1.f };

public:

	void Render_ImGui()
	{
		/* 피킹 옵션일때만 적용된다 */
		ImGui::Checkbox(" Is On Normal "  , &this->isOnNormal);
		ImGui::NewLine();


		ImGui::Checkbox(" Use Brush Scale " , &this->isUseBrushScale);
		ImGui::SameLine();
		if (ImGui::Button(" Reset##Scale "))
			vBrushScale = {1.f,1.f,1.f};
		ImGui::BeginDisabled(!this->isUseBrushScale);
		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x * 0.7f);
		ImGui::SliderFloat3("Brush Scale",&this->vBrushScale.x  , 1.f , 100.f , "%.2f");
		ImGui::EndDisabled();

		ImGui::Checkbox(" Use Brush Rotation ", &this->isUseBrushRotation );
		ImGui::SameLine();
		if (ImGui::Button(" Reset##Rot "))
			vBrushRotation = { 1.f,1.f,1.f };
		ImGui::BeginDisabled(!this->isUseBrushRotation);
		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x * 0.7f);
		ImGui::SliderFloat3("Brush Rotation", &this->vBrushRotation.x, 0.f, 360.f, "%.2f");
		ImGui::EndDisabled();

		ImGui::Checkbox(" Use Placement Spacing  ", &this->isUsePlacementSpacing);
		ImGui::BeginDisabled(!this->isUsePlacementSpacing);
		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x * 0.7f);
		ImGui::DragFloat(" Placement Spacing " , &this->fPlacementSpacing, 0.1f , 0.f , 100.f , "%.2f");
		ImGui::EndDisabled();
		ImGui::NewLine();

		ImGui::Checkbox(" Use Random Rotation Y ", &this->isUseRandomYRotation );
		ImGui::BeginDisabled(!this->isUseRandomYRotation);
		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x * 0.7f);
		ImGui::DragFloatRange2(" Random Rot Y Range ", &vRandomMinMaxRotaionRange.x, &vRandomMinMaxRotaionRange.y
			, 1.f, 0.f, 360.f, "Min: %.1f", "Max: %.1f");
		ImGui::EndDisabled();

		ImGui::Checkbox(" Use Random Scale ", &this->isUseRandomScale);
		ImGui::BeginDisabled(!this->isUseRandomScale);
		ImGui::SeparatorText(" Random Scale ");
		ImGui::DragFloatRange2(" Random Scale X Range ", &vMinMaxScaleX.x, &vMinMaxScaleX.y
			, 0.01f, 0.f, 100.f, "Min: %.1f", "Max: %.1f");
		ImGui::DragFloatRange2(" Random Scale Y Range ", &vMinMaxScaleY.x, &vMinMaxScaleY.y
			, 0.01f, 0.f, 100.f, "Min: %.1f", "Max: %.1f");
		ImGui::DragFloatRange2(" Random Scale Z Range ", &vMinMaxScaleZ.x, &vMinMaxScaleZ.y
			, 0.01f, 0.f, 100.f, "Min: %.1f", "Max: %.1f");
		ImGui::Separator();
		ImGui::EndDisabled();

	}
}BRUSH_MODE_OPTION;



class CMapToolManager : public CBase
{
public:
	using Super = CBase;
	DECLARE_SINGLETON(CMapToolManager)
private:
	CMapToolManager();
	virtual ~CMapToolManager() = default;
public:
	HRESULT					Initialize(ID3D11Device* pDevice , ID3D11DeviceContext* pContext);
	CMapObject*				Make_MapObject(void* pArg , _bool isPreview = false);
	HRESULT					Batch_Preview();
private:
	HRESULT					Register_MapObjectCloneFactory();
public:
	void					Update(float DT);
	void					Input_Update(float DT);
	void					Mouse_Update(float DT);
	void					Preview_Update(float DT);
public:
	CLIENT_MAKEPATH_DESC_BASE* Make_Client_MakePathDesc( EClientMakePath eClientMakePath  , CLIENT_MAKEPATH_DESC_BASE* pPrototype = nullptr);

	HRESULT						Change_Instance_To_OtherDrawType(CMapObject* pChangeMapObject, EMapObject_DrawType eChangeType);
	HRESULT						Change_Default_To_OtherDrawType(CMapObject* pChangeMapObject, EMapObject_DrawType eChangeType);
	void						Delete_Preview();
	void						DrawImGui_Preview();
public:
	HRESULT						Check_And_Bind();
	void						Get_SRT_BrushData(Vec3& vOutScale , Quat& vOutQuat , Vec3& vOutPosition);
public:
	void						Set_PreviewMapObject(CMapObject* pMapObject) { m_pPreviewMapobject = pMapObject; }
	void						Set_BrushScale(const Vec3& vScale) { m_tBrushModeOption.vBrushScale = vScale; }
	void						Set_BrushRotation(const Vec3 vDegree) { m_tBrushModeOption.vBrushRotation = vDegree; }
	void						Set_BrushRotation(const Quat& vQuat);
	void						Set_LevelMap(CLevel_Map* pLevelMap) { m_pLevelMap = pLevelMap; }
	void						Set_MouseRange(float fMouseRange) { m_fMouseRange = fMouseRange; }
	void						Set_MouseWheelPos(float fMouseWheelPos) { m_fMouseWheelSpeed = fMouseWheelPos; }

	void						Set_MapToolObjectBatchMode(EMapToolObjectBatchMode eType)				{ m_eMapTooObjectBatchMode = eType; }
	void						Set_MakeMapObjectClientMakePath(EClientMakePath eClientMakePathType)	{ m_eMakeMapObjectClientMakePath = eClientMakePathType; }
	void						Set_MakeMapObjectClientLevelType(EClientLevelType	eClientLevelType)	{ m_eMakeMapObjectClientLevelType = eClientLevelType; }
	void						Set_MakeMapObjectDrawType(EMapObject_DrawType eMapObjectDrawType)		{ m_eMakeMapObjectDrawType = eMapObjectDrawType; }
public:
	CMapObject*					Get_PrevieObject()					{ return m_pPreviewMapobject; }

	const Vec3&					Get_BrushScale()					const { return m_tBrushModeOption.vBrushRotation; }
	const Vec3&					Get_BrushRotation()					const { return m_tBrushModeOption.vBrushRotation; }

	BRUSH_MODE_OPTION*			Get_BrushModeOption()				{ return &m_tBrushModeOption; }
	CMapObject*					Get_Preview()						const { return m_pPreviewMapobject; }
	const float&				Get_MouseRange()					const { return m_fMouseRange; }
	const float&				Get_MouseWheelSpeed()				const { return m_fMouseRange; }
	const  Vec3&				Get_MousePickingPos()						const;

	EMapToolObjectBatchMode		Get_MapToolObjectBatchMode()		const { return m_eMapTooObjectBatchMode; }
	EClientLevelType			Get_MakeMapObejctClientLevelType()	const { return m_eMakeMapObjectClientLevelType; }
	EClientMakePath				Get_MakeMapObjectClientMakePath()	const { return m_eMakeMapObjectClientMakePath; }
	EMapObject_DrawType			Get_MakeMapObjectDrawType()			const { return m_eMakeMapObjectDrawType; }
private:

	ID3D11Device*			m_pDevice{};
	ID3D11DeviceContext*	m_pContext{};
	
	/* 생성되기 직전 PreviewObject */
	CMapObject*				m_pPreviewMapobject{};
	/* GameInstance */
	CGameInstance*			m_pGameInstance{ nullptr };

	/* Batch Mode */
	EMapToolObjectBatchMode	m_eMapTooObjectBatchMode{ EMapToolObjectBatchMode::Single};

	/* 내가 생성시킬 때 기본 Defautl 값으로 들어갈 타입들 모임 */
	EMapObject_Type			m_eMakeMapObjectType{EMapObject_Type::STATICMODEL};
	EMapObject_DrawType		m_eMakeMapObjectDrawType{ EMapObject_DrawType::Default};
	EClientMakePath			m_eMakeMapObjectClientMakePath{EClientMakePath::StaticObject};
	EClientLevelType		m_eMakeMapObjectClientLevelType{ EClientLevelType::LOGO };

	/* 생성된 Map Level */
	CLevel_Map*				m_pLevelMap{nullptr};

	/* 마우스 피킹 관련 */
	float					m_fMouseRange{};
	float					m_fMouseWheelSpeed{};

	/* Brush Mode Option */
	BRUSH_MODE_OPTION		m_tBrushModeOption{};


	/* 월드의 광선 위치 */
	Vec3					m_vRayWorldPos{};


	MapObjectCloneFactory	m_funcMapObjectCloneFactory{nullptr};

	CImGui_ToolManager*												  m_pImGui_ToolManager{nullptr};
	array< MapObjectCloneFactory, ENUM_TO_UINT(EMapObject_Type::END)> m_arrayMapObjectCloneFactory{};


	Vec3					m_vLastPlacedPos{ 0.f, 0.f, 0.f };

private:
	virtual void Free() override;
public:
	friend class CPanel_MapTool;
};

NS_END