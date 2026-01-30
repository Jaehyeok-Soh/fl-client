#pragma once
#include "ImGui_Panel.h"

NS_BEGIN(Engine)
class CGameInstance;
class CCameraMan;
class CCamera;
NS_END


NS_BEGIN(Tool)

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

private:
	HRESULT	Render_MapObjectList();
	HRESULT	Render_CamInfo();
	HRESULT	Render_SelectInfo();
private:
	CGameInstance*				m_pGameInstance{nullptr};

	wchar_t						m_wszMapObjectLayerTag[ENUM_TO_UINT(EMapObject_Type::END)][MAX_PATH];
	array<list<CGameObject*>*,	ENUM_TO_UINT(EMapObject_Type::END)> m_arrayMapObjectList{};

	CImGui_Layout_Transform*	m_pTransformLayout{nullptr};
	CMapObject*					m_pSelectMapObject{nullptr};

	_uint						m_fOriginSRTFlag{};

	CCameraMan*					m_pCamera{nullptr};
	CCamera*					m_pCameraCom{nullptr};
public:
	static  CPanel_MapObjectList* Create(const _char* pLabel, CLevel* pOwner, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual void Free() override;
};

NS_END

