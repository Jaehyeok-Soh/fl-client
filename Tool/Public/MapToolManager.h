#pragma once
#include "Base.h"




NS_BEGIN(Engine)

class  CGameInstance;

NS_END

NS_BEGIN(Tool)

class CMapObject;
class CImGui_ToolManager;


using MapObjectCloneFactory = std::function<CGameObject*(void* pArg)>;


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
	CMapObject*				Make_MapObject(EMapObject_Type eType, void* pArg , _bool isPreview = false);
	HRESULT					Batch_Preview();
private:
	HRESULT					Register_MapObjectCloneFactory();
public:
	void					Update(float DT);
	void					Input_Update(float DT);
	void					Mouse_Update(float DT);
	void					Preview_Update(float DT);
public:
	void					Delete_Preview();
	void					DrawImGui_Preview();
public:
	void					Set_MouseRange(float fMouseRange) { m_fMouseRange = fMouseRange; }
	void					Set_MouseWheelPos(float fMouseWheelPos) { m_fMouseWheelSpeed = fMouseWheelPos; }
public:
	CMapObject*				Get_Preview()			const { return m_pPreviewMapobject; }
	const float&			Get_MouseRange()		const { return m_fMouseRange; }
	const float&			Get_MouseWheelSpeed()   const { return m_fMouseRange; }
private:
	ID3D11Device*			m_pDevice{};
	ID3D11DeviceContext*	m_pContext{};
	CMapObject*				m_pPreviewMapobject{};
	CGameInstance*			m_pGameInstance{ nullptr };

	float					m_fMouseRange{};
	float					m_fMouseWheelSpeed{};

	Vec3					m_vRayWorldPos{};

	CImGui_ToolManager*												  m_pImGui_ToolManager{nullptr};
	array< MapObjectCloneFactory, ENUM_TO_UINT(EMapObject_Type::END)> m_arrayMapObjectCloneFactory{};

private:
	virtual void Free() override;
public:
	friend class CPanel_MapTool;
};

NS_END