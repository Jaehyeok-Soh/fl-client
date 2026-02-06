#pragma once
#include "ImGui_Panel.h"

NS_BEGIN(Engine)
class CGameInstance;
class CCameraMan;
class CCamera;
class CModel;
NS_END


NS_BEGIN(Tool)

struct tagOverrideMaterials;

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

	void	Reset_SelectValue();

private:
	HRESULT	Render_MapObjectList();
	HRESULT	Render_SelectInfo();
	HRESULT	Render_StaticModel();
	HRESULT	Render_InstanceModel();

private:
	HRESULT	Render_ModelInfo(Tool::USING_MODEL_INFO& tModelInfo ,CModel* pModel);
	HRESULT	Render_SelectOverrideMaterialInfo();
	HRESULT	Render_SelectOriginMaterialInfo();
private:

	CGameInstance*				m_pGameInstance{nullptr};

	wchar_t						m_wszMapObjectLayerTag[ENUM_TO_UINT(EMapObject_Type::END)][MAX_PATH];
	array<list<CGameObject*>*,	ENUM_TO_UINT(EMapObject_Type::END)> m_arrayMapObjectList{};

	char						m_szFindName[MAX_PATH];

	CImGui_Layout_Transform*	m_pTransformLayout{nullptr};
	CMapObject*					m_pSelectMapObject{nullptr};

	CCameraMan*					m_pCamera{nullptr};
	CCamera*					m_pCameraCom{nullptr};

	_int						m_iSelectInstanceID{};
	_int						m_iSelectLayerTag{};

	_uint						m_iSelectOverrideMtlTextureID{ 0 };
	_int						m_iSelectOverrideMtlID{ -1 };


	_uint						m_iSelectOriginMtlTexture{ 0 };
	_bool						m_isShowOriginMtlInfo{};
	array<string,ENUM_TO_UINT(EMaterialTextureType::MAX_COUNT)>	 m_arrayOriginMtlUsingTexturesName{};
	string						m_strOriginMtlName{};
	string						m_strOriginMtlPath{};

	ImVec2						m_vTextureInfoTableSize{ ImVec2(0,50) };

	uintptr_t					m_uptrPreSelectObject{0};

public:
	static  CPanel_MapObjectList* Create(const _char* pLabel, CLevel* pOwner, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual void Free() override;
};

NS_END

