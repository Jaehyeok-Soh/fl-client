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

	HRESULT					Change_Instance_To_OtherDrawType(CMapObject* pChangeMapObject, EMapObject_DrawType eChangeType);
	void					Delete_Preview();
	void					DrawImGui_Preview();
public:
	HRESULT					Check_And_Bind();
public:
	void					Set_LevelMap(CLevel_Map* pLevelMap) { m_pLevelMap = pLevelMap; }
	void					Set_MouseRange(float fMouseRange) { m_fMouseRange = fMouseRange; }
	void					Set_MouseWheelPos(float fMouseWheelPos) { m_fMouseWheelSpeed = fMouseWheelPos; }

	void					Set_MakeMapObjectClientMakePath(EClientMakePath eClientMakePathType)	{ m_eMakeMapObjectClientMakePath = eClientMakePathType; }
	void					Set_MakeMapObjectClientLevelType(EClientLevelType	eClientLevelType)	{ m_eMakeMapObjectClientLevelType = eClientLevelType; }
	void					Set_MakeMapObjectDrawType(EMapObject_DrawType eMapObjectDrawType)		{ m_eMakeMapObjectDrawType = eMapObjectDrawType; }
public:
	CMapObject*				Get_Preview()						const { return m_pPreviewMapobject; }
	const float&			Get_MouseRange()					const { return m_fMouseRange; }
	const float&			Get_MouseWheelSpeed()				const { return m_fMouseRange; }

	EClientLevelType		Get_MakeMapObejctClientLevelType()	const { return m_eMakeMapObjectClientLevelType; }
	EClientMakePath			Get_MakeMapObjectClientMakePath()	const { return m_eMakeMapObjectClientMakePath; }
	EMapObject_DrawType		Get_MakeMapObjectDrawType()			const { return m_eMakeMapObjectDrawType; }
private:

	ID3D11Device*			m_pDevice{};
	ID3D11DeviceContext*	m_pContext{};
	
	/* 생성되기 직전 PreviewObject */
	CMapObject*				m_pPreviewMapobject{};
	/* GameInstance */
	CGameInstance*			m_pGameInstance{ nullptr };

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

	/* 월드의 광선 위치 */
	Vec3					m_vRayWorldPos{};


	MapObjectCloneFactory	m_funcMapObjectCloneFactory{nullptr};

	CImGui_ToolManager*												  m_pImGui_ToolManager{nullptr};
	array< MapObjectCloneFactory, ENUM_TO_UINT(EMapObject_Type::END)> m_arrayMapObjectCloneFactory{};

private:
	virtual void Free() override;
public:
	friend class CPanel_MapTool;
};

NS_END