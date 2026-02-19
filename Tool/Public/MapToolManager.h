#pragma once
#include "Base.h"




NS_BEGIN(Engine)

class  CGameInstance;
struct CLIENT_MAKEPATH_DESC_BASE;

class CTexture;
class CShader;
class CTextureBase;

NS_END

NS_BEGIN(Tool)


class CMapObject;
class CImGui_ToolManager;
class CLevel_Map;

using MapObjectCloneFactory = std::function<CGameObject*(void* pArg)>;
using PairKey = std::pair<wstring, vector<wstring>>;


 
struct MIX_RGBA_DATA
{
	// Mix할떄 UV좌표에 곱해주어 정밀한 표현을 담당해준다
	_float		fRGBA_Mix_Forces[MAX_RGBA]{ 1.f, 1.f, 1.f, 1.f };
	// Mix될 RGBA 맵에서 각 R , G , B , A 가 연결된 Splating Texture들의 Index
	_int		iRGBA_Connected_Tile_Index[MAX_RGBA]{ 0 , 0 , 0 , 0 };
	// Mix될 RGBA 맵에서 각 R , G , B , A 가 Splating을 사용할건지 안할건지에 대한 Flag값 false => BaseTexture가 그대로 들어감 true => Splating
	_int		iUseFlags[MAX_RGBA]{ true , true , true , true };
};

/* Shader에 넘길때 사용할 constant Buffer 데이터들 */
struct CB_MIX_RGBA_INFO
{
	MIX_RGBA_DATA g_MIX_RGBA_DATA[MAX_RGBA_TEXTURE_COUNT];

	int    g_iUse_Mix_RGBA_Count = { 0 };
	int    g_Use_Mix_RGBA_Map_Count_Dummy[3];
};


/* Tool에서 사용할 데이터들  */
struct MIX_RGBA_INFO
{
	vector<CTextureBase*>							vecMixRGBATexture{}; //RGB
	vector<MIX_RGBA_DATA>							vecMix_RGBA_Data{};
	_int											iUse_Mix_RGBA_Count{};
public:
	MIX_RGBA_INFO()
		: iUse_Mix_RGBA_Count{ 0 }, vecMix_RGBA_Data{}
	{
		vecMixRGBATexture.reserve(MAX_RGBA_TEXTURE_COUNT);
	}
public:
	void Add_Texture_And_Data()
	{
		if (iUse_Mix_RGBA_Count >= MAX_RGBA_TEXTURE_COUNT)
			return;

		iUse_Mix_RGBA_Count++;
		vecMixRGBATexture.push_back(nullptr);
		vecMix_RGBA_Data.push_back(MIX_RGBA_DATA());

	}
	void Delete_Texture_And_Data(_int iIndex)
	{
		if (iIndex >= iUse_Mix_RGBA_Count)return;
		if (iIndex < 0)					  return;

		iUse_Mix_RGBA_Count--;

		/* vector 메모리 정리 */
		Safe_Release(vecMixRGBATexture[iIndex]);
		vecMixRGBATexture.erase(vecMixRGBATexture.begin() + iIndex);
		vecMix_RGBA_Data.erase(vecMix_RGBA_Data.begin() + iIndex);
	}
	void Free()
	{
		for (auto& TexBase : vecMixRGBATexture)
			Safe_Release(TexBase);
	}

};

typedef struct tagTexture_Splatting_Info
{
	/* Land Scape 에서 쓰는 것 뿐만아니라 지형의 전체적인 Color를 담당해준다 추후 바위 Normal위에 들어갈 색깔을 담당하기도한다 */
	/* Land Scape 에서 쓰는 것 뿐만아니라 지형의 전체적인 Color를 담당해준다 추후 바위 Normal위에 들어갈 색깔을 담당하기도한다 */
	CTextureBase*													pBase_Texture{nullptr};

	/* RGBA Texture와 함꼐 Mix될 Tile Texture / Texture 2D Array로 해서 들어가게 된다 한장씩만 Binding */
	CTextureBase*													pMix_DH_Tile_Texture{nullptr};
	CTextureBase*													pMix_NBR_Tile_Texture{nullptr};

	/* Mix_DH_Tile_Texture 가 Texture 2D Array로 들어가 있기 때문에 이를 따로따로 모아두고 ImGui에서 Rendering 해주기위함 SRV 모아놓기 */
	vector<ID3D11ShaderResourceView*>								vecDHTextureArraySlices{};

	/* 현재 RGBA Texture 및 info 를 사용하는 개수  */
	MIX_RGBA_INFO													tMix_RGBA_Info{};

public:
	void Free()
	{
		for (auto& SRV : vecDHTextureArraySlices)
			Safe_Release(SRV);

		Safe_Release(pBase_Texture);
		Safe_Release(pMix_DH_Tile_Texture);
		Safe_Release(pMix_NBR_Tile_Texture);


		tMix_RGBA_Info.Free();
	}
}TEXTURE_SPLATTING_INFO;


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

enum MapTexture_MixFlag : _uint
{
	//NONE ,
	//RGB  ,
	//RGBA ,
	//BASE ,
};

enum class Mix_Tile_Texture_TYPE
{
	RGBA,
	RGB,
	END,
};



class CMapToolManager : public CBase
{
public:
	using Super = CBase;
	DECLARE_SINGLETON(CMapToolManager)
private:
	CMapToolManager();
	virtual ~CMapToolManager() = default;
public:
	HRESULT						Initialize(ID3D11Device* pDevice , ID3D11DeviceContext* pContext);
	CMapObject*					Make_MapObject(void* pArg , _bool isPreview = false);
	HRESULT						Batch_Preview();
	HRESULT						Register_MapObjectCloneFactory();
public:
	HRESULT						Bind_SplatingTextureInfo();
	HRESULT						Bind_MapTexture();

	HRESULT						Bind_Mix_RGBA_Info();
	HRESULT						Bind_Mix_RGBA_Texture();
	HRESULT						Bind_Mix_RGBA_Data_And_Count();
public:
	HRESULT						Make_DefaultTexture();
	HRESULT						Slice_DH_Texture();
public:
	HRESULT						Register_MapTexture();
	HRESULT						UnRegister_MapTexture();
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

private:

	ID3D11Device*				m_pDevice{};
	ID3D11DeviceContext*		m_pContext{};
	
	/* 생성되기 직전 PreviewObject */
	CMapObject*					m_pPreviewMapobject{};
	/* GameInstance */
	CGameInstance*				m_pGameInstance{ nullptr };

	/* Batch Mode */
	EMapToolObjectBatchMode		m_eMapTooObjectBatchMode{ EMapToolObjectBatchMode::Single};
	EMapTool_EmplaceType		m_eMapToolEmplaceType{EMapTool_EmplaceType::Free};
	
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



	TEXTURE_SPLATTING_INFO												m_tTextureSplattingInfo{};

	ID3D11ShaderResourceView*											m_pDefaultWhiteSRV;
	ID3D11ShaderResourceView*											m_pDefaultBlackSRV;



private:
	virtual void Free() override;
public:
	friend class CPanel_MapTool;
};

NS_END