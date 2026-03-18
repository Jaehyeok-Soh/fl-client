#pragma once
#include "Base.h"
#include "DataStruct_Map.h"
#include "GameData_Struct.h"

NS_BEGIN(Engine)

class	CGameInstance;
struct	CLIENT_MAKEPATH_DESC_BASE;
class	CDataDocumentBase;

class CTexture;
class CShader;
class CTextureBase;

struct Camera_Cinematic_Sequence;
struct Camera_Keyframe_Data;


NS_END

NS_BEGIN(Tool)


class CMapObject;
class CImGui_ToolManager;
class CLevel_Map;
class CLevelData;
class CPanel_MapObjectList;

using MapObjectCloneFactory = std::function<CGameObject*(void* pArg)>;

/*  EClinetMakePath 값 체크*/

struct UEDataBindKey
{
	wstring					wstrModelPath{};
	vector<wstring>			vecMaterialNames{};
	Tool::EClientMakePath	eClientMakeType{ Tool::EClientMakePath::END };
	inline _bool	operator==(const UEDataBindKey& other) const
	{
		return std::tie(eClientMakeType, wstrModelPath, vecMaterialNames) < std::tie(other.eClientMakeType, other.wstrModelPath, other.vecMaterialNames);
	}
	inline bool operator<(const UEDataBindKey& other) const
	{
		return std::tie(eClientMakeType , wstrModelPath , vecMaterialNames) < std::tie(other.eClientMakeType, other.wstrModelPath, other.vecMaterialNames);
	}
};

using PairKey = std::pair<vector<uintptr_t>, Tool::EClientMakePath>;



typedef struct tagBrushModeOption
{
	/* 법선에 태울건지 안태울건지 옵션도 있으면 좋긴할듯 */
	bool	isUseBrushScale{false};
	bool	isUseBrushRotation{false};

	Vec3	vBrushScale{1.f,1.f,1.f};
	Vec3	vBrushRotation{0.f,0.f,0.f};

	bool	isOnNormal{false};

	bool	isUseGroupCount{false};
	_int	iMaxGroupMaxCount{};

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

		ImGui::Checkbox(" Use Placement Spacing  ", &this->isUsePlacementSpacing);
		ImGui::BeginDisabled(!this->isUsePlacementSpacing);
		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x * 0.7f);
		ImGui::DragFloat(" Placement Spacing ", &this->fPlacementSpacing, 0.1f, 0.f, 100.f, "%.2f");
		ImGui::EndDisabled();
		ImGui::NewLine();


		ImGui::Checkbox(" Use GroupCount  ", &this->isUseGroupCount);
		ImGui::BeginDisabled(!this->isUseGroupCount);
		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x * 0.7f);
		ImGui::DragInt(" Group MaxCount ", &this->iMaxGroupMaxCount ,1 , 0 , 200 );
		ImGui::EndDisabled();
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
	EClientMakePath				Get_ClientMakePath_ByFilePath(const wstring& wstrFilePullPath);
public:
	HRESULT						Initialize(ID3D11Device* pDevice , ID3D11DeviceContext* pContext);
	CMapObject*					Make_MapObject(void* pArg , _bool isPreview = false);
	HRESULT						Batch_Preview();
	HRESULT						Register_MapObjectCloneFactory();
	HRESULT						Update_EnvEffectList();
public:
	CModel*						Get_MonsterPreviewModel(DTO::EMakeMonsterType eMakeMonsterType );
	CModel*						Get_PlayerPreviewModel();
	CModel*						Get_BatchObjectModel(DTO::EMakeObjectType eType);
public:
	HRESULT						Ready_LevelData();
	HRESULT						Apply_LevelData(const DTO::TLevelData* pData);
	HRESULT						Release_SceneData();
public:
	HRESULT						Bind_SplatingTextureInfo();
	HRESULT						Bind_MapTexture();

	HRESULT						Bind_Mix_RGBA_Info();
	HRESULT						Bind_Mix_RGBA_Texture();
	HRESULT						Bind_Mix_RGBA_Data_And_Count();
public:
	HRESULT						Make_DefaultTexture();
	HRESULT						Slice_DH_Texture();
	HRESULT						Slice_NBR_Texture();
public:
	HRESULT						Register_MapTexture();

	HRESULT						Release_SplatingTextureData();
	HRESULT						Delete_TextureSplatingInfoData(const wstring& wstrDeleteName);
	HRESULT						Load_TextureSplatingInfoData();
	HRESULT						Load_TextureSplatingInfoData(const wstring& wstrLoadName);
	HRESULT						Save_TextureSplatingInfoData();
	HRESULT						Save_TextureSplatingInfoData(const wstring& wstrSaveName);
	HRESULT						UnRegister_MapTexture();
public:
	HRESULT						Set_GPU_DiscardColor();
public:
	void						Update(float DT);
	void						Input_Update(float DT);
	void						Mouse_Update(float DT);
	void						Preview_Update(float DT);
public:
	CLIENT_MAKEPATH_DESC_BASE*	Make_Client_MakePathDesc( EClientMakePath eClientMakePath  , CLIENT_MAKEPATH_DESC_BASE* pPrototype = nullptr);
	_bool						IsExist_ClientMakePathDesc(EClientMakePath eClientMakePath);

	HRESULT						Change_Instance_To_OtherDrawType(CMapObject* pChangeMapObject, EMapObject_DrawType eChangeType);
	HRESULT						Change_Default_To_OtherDrawType(CMapObject* pChangeMapObject, EMapObject_DrawType eChangeType);
	void						Delete_Preview();
	void						DrawImGui_Preview();
public:
	HRESULT						Check_And_Bind_FromUE();
	void						Get_SRT_BrushData(Vec3& vOutScale , Quat& vOutQuat , Vec3& vOutPosition);
public:
	void						Set_MakeMapObjectSectionNumber(_int iSectionNum)						{ m_iMakeSectionNumber = iSectionNum; }
	void						Set_MapToolEmplaceType(EMapTool_EmplaceType eType)						{ m_eMapToolEmplaceType = eType; }
	void						Set_PreviewMapObject(CMapObject* pMapObject)							{ m_pPreviewMapobject = pMapObject; }
	void						Set_BrushScale(const Vec3& vScale)										{ m_tBrushModeOption.vBrushScale = vScale; }
	void						Set_BrushRotation(const Vec3 vDegree)									{ m_tBrushModeOption.vBrushRotation = vDegree; }
	void						Set_BrushRotation(const Quat& vQuat);
	void						Set_LevelMap(CLevel_Map* pLevelMap)										{ m_pLevelMap = pLevelMap; }
	void						Set_MouseRange(float fMouseRange)										{ m_fMouseRange = fMouseRange; }
	void						Set_MouseWheelPos(float fMouseWheelPos)									{ m_fMouseWheelSpeed = fMouseWheelPos; }

	void						Set_MapToolObjectBatchMode(EMapToolObjectBatchMode eType)				{ m_eMapTooObjectBatchMode = eType; }
	void						Set_MakeMapObjectClientMakePath(EClientMakePath eClientMakePathType)	{ m_eMakeMapObjectClientMakePath = eClientMakePathType; }
	void						Set_MakeMapObjectClientLevelType(EClientLevelType	eClientLevelType)	{ m_eMakeMapObjectClientLevelType = eClientLevelType; }
	void						Set_MakeMapObjectDrawType(EMapObject_DrawType eMapObjectDrawType)		{ m_eMakeMapObjectDrawType = eMapObjectDrawType; }

public:
	CMapObject*					Get_PrevieObject()					{ return m_pPreviewMapobject; }

	const Vec3&					Get_BrushScale()					const { return m_tBrushModeOption.vBrushRotation; }
	const Vec3&					Get_BrushRotation()					const { return m_tBrushModeOption.vBrushRotation; }

	_int						Get_MakeObjectSectionNubmer()		const { return m_iMakeSectionNumber; }

	BRUSH_MODE_OPTION*			Get_BrushModeOption()				{ return &m_tBrushModeOption; }
	CMapObject*					Get_Preview()						const { return m_pPreviewMapobject; }
	const float&				Get_MouseRange()					const { return m_fMouseRange; }
	const float&				Get_MouseWheelSpeed()				const { return m_fMouseRange; }
	const  Vec3&				Get_MousePickingPos()				const;

	EMapTool_EmplaceType		Get_MapToolEmplaceType()			const { return m_eMapToolEmplaceType; }
	EMapToolObjectBatchMode		Get_MapToolObjectBatchMode()		const { return m_eMapTooObjectBatchMode; }
	EClientLevelType			Get_MakeMapObejctClientLevelType()	const { return m_eMakeMapObjectClientLevelType; }
	EClientMakePath				Get_MakeMapObjectClientMakePath()	const { return m_eMakeMapObjectClientMakePath; }
	EMapObject_DrawType			Get_MakeMapObjectDrawType()			const { return m_eMakeMapObjectDrawType; }
public:
	HRESULT						Render();
public:
	HRESULT						Export_SaveSceneData(DTO::ECategory eCategory, CDataDocumentBase* pDocument);
public:
	HRESULT						Set_GPU_EnvData();
#pragma region Camera Cinematic Sequence
public:
	HRESULT						Load_Camera_Cinematic_Sequence(const wstring& wstrFindKey);
	HRESULT						Save_Camera_Cinematic_Sequence(const wstring& wstrSaveKey);

	HRESULT						Reset_Camera_Cinematic_Sequence();
	HRESULT						Update_Camera_Cinematic_Sequence_Names();

	HRESULT						Ready_CinematicSequenceDebugRender();
public:
	void						Select_MapTexture();
private:
	/* Camera Cinematic Sequence 관련 데이터를 복사로 받아와서 작업하고 저장하는용도 */	
	Camera_Cinematic_Sequence*	m_pCamCinematicSequence{nullptr};
	vector<string>				m_vecCamCinematicSequenceNames{};
	CModel*						m_pCamCinematicSequenceRenderModel{nullptr};
	CShader*					m_pCamCinematicSequenceRenderShader{ nullptr };
#pragma endregion
private:
	/* Texture Splating을 사용하는 것들을 위해 데이터를 미리 받아오고 활용할 수 있게 한다... */
	map<wstring, TEXTURE_SPLATTING_INFO >				m_mapTextureSplatingInfoDatas{};
private:
	ID3D11Device*										m_pDevice{};
	ID3D11DeviceContext*								m_pContext{};
	
	/* 생성되기 직전 PreviewObject */
	CMapObject*											m_pPreviewMapobject{};
	/* GameInstance */
	CGameInstance*										m_pGameInstance{ nullptr };

	/* Batch Mode */
	EMapToolObjectBatchMode								m_eMapTooObjectBatchMode{ EMapToolObjectBatchMode::Single};
	EMapTool_EmplaceType								m_eMapToolEmplaceType{EMapTool_EmplaceType::Free};

	/*  */
	_int						m_iMakeSectionNumber{0};
	/* 내가 생성시킬 때 기본 Defautl 값으로 들어갈 타입들 모임 */
	EMapObject_Type				m_eMakeMapObjectType{EMapObject_Type::STATICMODEL};
	EMapObject_DrawType			m_eMakeMapObjectDrawType{ EMapObject_DrawType::Default};
	EClientMakePath				m_eMakeMapObjectClientMakePath{EClientMakePath::StaticObject};
	EClientLevelType			m_eMakeMapObjectClientLevelType{ EClientLevelType::LOGO };

	/* 생성된 Map Level */
	CLevel_Map*					m_pLevelMap{nullptr};

	/* 마우스 피킹 관련 */
	float						m_fMouseRange{};
	float						m_fMouseWheelSpeed{};

	/* Brush Mode Option */
	BRUSH_MODE_OPTION			m_tBrushModeOption{};


	/* 월드의 광선 위치 */
	Vec3						m_vRayWorldPos{};

	_int						m_flagMixTexture{};


	MapObjectCloneFactory		m_funcMapObjectCloneFactory{nullptr};

	CImGui_ToolManager*													m_pImGui_ToolManager{nullptr};
	array< MapObjectCloneFactory, ENUM_TO_UINT(EMapObject_Type::END)>	m_arrayMapObjectCloneFactory{};

	Vec3																m_vLastPlacedPos{ 0.f, 0.f, 0.f };
	_uint																m_iCurGroupCount{0};

	/* 내가 가져온 Texture들의 목록을 들고있어준다 */
	/* wstring은 나눠줄 영역 담당 */
	unordered_map< wstring , vector<CTextureBase*>>						m_umapMapTextures{};

	CShader*															m_pMesh_Shader{nullptr};
	CShader*															m_pInstMesh_Shader{nullptr};



	TEXTURE_SPLATTING_INFO												m_tTextureSplattingInfo{};

	ID3D11ShaderResourceView*											m_pDefaultWhiteSRV;
	ID3D11ShaderResourceView*											m_pDefaultBlackSRV;



	const _tchar*														TextureSplatingInfoDataPath = L"../../Resources/Data/MapData/TextureSplatingInfoData.json";



	CLevelData*															m_pLevelData{nullptr};



	/* Discard Test 용 */
	Vec4																m_vDiscardColor{0.1f,0.1f ,0.1f ,0.1f };


	/* Map Texture Binding 함수용 */
	CTextureBase**				m_ppTargetSlot{nullptr};
	_bool						m_isTexArraySelect{false};
	_bool						m_isTex_DH_ArraySelect{false};
	_bool						m_isTex_NBR_ArraySelect{false};
	wstring						m_selectedCategoryName{L""};




	vector<string>				m_vecEnvEffectTags{};


private:
	virtual void Free() override;
public:
	friend class CPanel_MapTool;
	friend class CPanel_MapObjectList;
};

NS_END