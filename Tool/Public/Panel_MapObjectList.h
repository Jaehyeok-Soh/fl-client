#pragma once
#include "ImGui_Panel.h"

NS_BEGIN(Engine)
class CGameInstance;
class CCameraMan;
class CCamera;
class CModel;
struct STATICOBJECT_DESC;
struct LANDSCAPE_DESC;
NS_END


NS_BEGIN(Tool)

struct tagOverrideMaterials;
class CMapToolManager;
class CImGui_Layout_Transform;
class CMapObject;

class CPanel_MapObjectList : public CImGui_Panel
{
	using Super = CImGui_Panel;
protected:
	explicit CPanel_MapObjectList(const _char* pLabel, CLevel* pOwner, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CPanel_MapObjectList() = default;
private:
	HRESULT	Initialize();
	HRESULT	Ready_LayerTag();
public:
	virtual HRESULT Render(CToolObject* pGo)override;
	virtual void Update(const _float fTimeDelta)override;
	HRESULT	Update_MapObjectList();
	void	Update_SelectObject();

	void	Reset_SelectValue();

private:
	HRESULT	Render_MapObjectList();
	HRESULT	Render_SelectInfo();

private:

	HRESULT	Render_ModelInfo();
	HRESULT	Render_SelectMaterial();
	HRESULT	Render_Description();

	HRESULT	Render_TransformInfo();

	HRESULT	Render_SelectOverrideMaterialInfo();
	HRESULT	Render_SelectOriginMaterialInfo();


private:
	
	void						ImGuiUpdate_StaticObject_Desc(STATICOBJECT_DESC* pDesc);
	void						ImGuiUpdate_LandScape_Desc(LANDSCAPE_DESC* pDesc);

private:
	void						Compute_LandScape_TextureUV(_uint iLandScapeIndex , OUT Vec2& vOut_LT , OUT Vec2& vOut_RB );

private:
	CMapToolManager*			m_pMapToolManager{nullptr};

	CGameInstance*				m_pGameInstance{nullptr};

	wchar_t						m_wszMapObjectLayerTag[ENUM_TO_UINT(EMapObject_Type::END)][MAX_PATH];

	char						m_szFindName[MAX_PATH];

	CImGui_Layout_Transform*	m_pTransformLayout{nullptr};
	CMapObject*					m_pSelectMapObject{nullptr};

	CCameraMan*					m_pCamera{nullptr};
	CCamera*					m_pCameraCom{nullptr};

	_uint						m_iObjectCount{};

	string						m_strBuffer{};
	_int						m_iBuffer{};

	_int						m_iSelectInstanceID{};
	_int						m_iSelectLayerTag{};

	_uint						m_iSelectOverrideMtlTextureID{ 0 };
	_int						m_iSelectOverrideMtlID{ -1 };

	EClientMakePath				m_eShowMapObjectFilter{ EClientMakePath::END };

	list<CGameObject*>*			m_pLayer{nullptr};
	
	CMaterial*					m_pSelectMaterial{nullptr};
	_int						m_iSelectMaterialIndex{0};

	_uint						m_iSelectOriginMtlTexture{ 0 };
	string						m_strOriginMtlName{};
	string						m_strOriginMtlPath{};


	std::array<ID3D11ShaderResourceView*, ENUM_TO_SZET(EMaterialTextureType::MAX_COUNT)>	m_arrayMtl_SRVs{};
	std::array<wstring, ENUM_TO_SZET(EMaterialTextureType::MAX_COUNT)>						m_arrayMtl_Textures{};
	_uint						m_iSelectMtSlot{};

	ImVec2						m_vTextureInfoTableSize{ ImVec2(0,50) };

	uintptr_t					m_uptrPreSelectObject{0};


	/* Land Scape 전용 Index값 계산하기 위한 멤버변수들 */
	/* 전체 Land Scape 크기 */
	/* 가로 세로 개수 */
	_int	m_iLandScape_Col{1}; // 세로
	_int	m_iLandScape_Row{1}; // 가로

public:
	static  CPanel_MapObjectList* Create(const _char* pLabel, CLevel* pOwner, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual void Free() override;
};

NS_END

