#include "pch.h"
#include "MapToolManager.h"
#include "MapObject.h"
#include "ImGui_ToolManager.h"
#include "Picking_ToolManager.h"
#include "DebugLine.h"
#include "Model.h"
#include "DataStruct_Map.h"
#include "Level_Map.h"
#include "GameInstance.h"

IMPLEMENT_SINGLETON(CMapToolManager)

CMapToolManager::CMapToolManager()
	: m_pGameInstance(CGameInstance::GetInstance())
	, m_pImGui_ToolManager(CImGui_ToolManager::GetInstance())
	, m_arrayMapObjectCloneFactory{}
	, m_pPreviewMapobject(nullptr)
{
	Safe_AddRef(m_pGameInstance);
	m_arrayMapObjectCloneFactory.fill(nullptr);
}

HRESULT CMapToolManager::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	m_pDevice  = pDevice;
	m_pContext = pContext;

	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);


	m_fMouseWheelSpeed = 0.001f;
	m_fMouseRange = 1.f;

	if (FAILED(Register_MapObjectCloneFactory()))
		return E_FAIL;

	return S_OK;
}

void	CMapToolManager::Update(float DT)
{
	Input_Update(DT);
	
	Mouse_Update(DT);

	Preview_Update(DT);
}

void CMapToolManager::Input_Update(float DT)
{
	/* 휠 */
	_long lMouseWheelDt = m_pGameInstance->Get_DIMouseMove(MOUSEMOVESTATE::WHEEL);
	m_fMouseRange += (float)lMouseWheelDt * m_fMouseWheelSpeed;

	if (m_fMouseRange < 1.f)
		m_fMouseRange = 1.f;


	if (m_pGameInstance->Mouse_Up(MOUSEKEYSTATE::LB))
	{
		if (m_pPreviewMapobject != nullptr)
		{
			m_pPreviewMapobject->Set_MapObjectState(CMapObject::EState::Default);
			m_pPreviewMapobject->Get_Component<CTransform>()->Set_Info(TRANSFORM_INFO_STATE::POS, m_vRayWorldPos);
			m_pPreviewMapobject = nullptr;
		}
	}

	return;
}

void CMapToolManager::Mouse_Update(float DT)
{
	m_pImGui_ToolManager->RayUpdate();

	Vec3 vRayPos = m_pGameInstance->Picking_Get_RayPos();
	Vec3 vRayDir = m_pGameInstance->Picking_Get_RayDir();
	vRayDir.Normalize();

	m_vRayWorldPos = vRayPos + vRayDir * m_fMouseRange;

}

void CMapToolManager::Preview_Update(float DT)
{
	if (!m_pPreviewMapobject) return;

	CTransform* pTransform = m_pPreviewMapobject->Get_Component<CTransform>();
	if (!pTransform) return;

	pTransform->Set_Info(TRANSFORM_INFO_STATE::POS , m_vRayWorldPos );

	return;
}

CLIENT_MAKEPATH_DESC_BASE* CMapToolManager::Make_Client_MakePathDesc(EClientMakePath eClientMakePath ,CLIENT_MAKEPATH_DESC_BASE* pPrototype)
{
	CLIENT_MAKEPATH_DESC_BASE* pDesc{nullptr};

	switch (eClientMakePath)
	{
	case Tool::EClientMakePath::StaticObject:	return nullptr;
	default:									return nullptr;
	}

	return pDesc;
}

HRESULT CMapToolManager::Change_Instance_To_OtherDrawType(CMapObject* pChangeMapObject , EMapObject_DrawType eChangeType)
{
	/* 이미 한번 모델이 생성된 객체 */

	if (pChangeMapObject == nullptr) return E_FAIL;
	CMapObject::MAPOBJECT_DESC tDesc{};

	tDesc.eClientLevelType				= pChangeMapObject->Get_ClientLevelType();
	tDesc.eClientMakePath				= pChangeMapObject->Get_ClientMakePath();
	tDesc.eMapObjectDrawType			= eChangeType;
	tDesc.eState						= pChangeMapObject->Get_MapObjectState();
	tDesc.isUELoaded					= pChangeMapObject->Get_IsUELoaded();
	tDesc.isLoaded						= pChangeMapObject->Get_IsLoaded();
	tDesc.wstrLayerTag					= g_wszMapObjectLayer;
	tDesc.tUsingModelInfo.wstrPath		= pChangeMapObject->Get_ModelPath();
	tDesc.tUsingModelInfo.wstrName		= Engine_Utils::ToWString(pChangeMapObject->Get_ModelFileName());

	vector<SRT_DATA> vecSRTData			= pChangeMapObject->Get_SRTDatas();
	vector<CLIENT_MAKEPATH_DESC_BASE*> vecClientMakePathDesc = pChangeMapObject->Get_ClientMakePathDescs();

	CGameObject* pResult{ nullptr };

	_uint iCount =  ENUM_TO_UINT(vecSRTData.size());

	for (_uint i = 0; i < iCount; ++i)
	{
		/* SRT 하나만 Push 하고 하나씩 생성 */
		tDesc.vecSRTs.clear();
		tDesc.vecClientMakePathDesc.clear();

		tDesc.vecSRTs.push_back(vecSRTData[i]);
		tDesc.vecSRTs.back().Update_World();

		CLIENT_MAKEPATH_DESC_BASE* pClientMakePathDesc{ nullptr };
		if (!vecClientMakePathDesc.empty())
		{
			/* Description Type집이넣어서 Desc 복사생성해서 뱉어주는 함수 */
			/* Empty가 아니였기 떄문에 new로 알아서 생성되어서 나올예정 */
			pClientMakePathDesc = Make_Client_MakePathDesc(tDesc.eClientMakePath , vecClientMakePathDesc[i]);
		}

		pResult = Make_MapObject(&tDesc);

		if (pResult == nullptr)
		{
			/* 실패했을시 누수처리 */
			Safe_Delete(pClientMakePathDesc);
			return E_FAIL;
		}
	}

	pChangeMapObject->Set_Dead(pChangeMapObject->Get_LayerTag());

	return S_OK;
}

void CMapToolManager::Delete_Preview()
{
	if (m_pPreviewMapobject)
	{
		m_pGameInstance->Request_DeleteGameObject(ENUM_TO_UINT(ELevelType::MAP), m_pPreviewMapobject->Get_LayerTag(), m_pPreviewMapobject);
		m_pPreviewMapobject = nullptr;
	}
}

void CMapToolManager::DrawImGui_Preview()
{
	if (!m_pPreviewMapobject) return;

	m_pPreviewMapobject->Draw_ImGui();
}

HRESULT CMapToolManager::Check_And_Bind()
{
	list<CGameObject*>* pStaticModelList = m_pGameInstance->Get_GameObject_List(ENUM_TO_UINT(ELevelType::MAP) , g_wszStaticModelLayer);
	list<CGameObject*>* pInstanceModelList = m_pGameInstance->Get_GameObject_List(ENUM_TO_UINT(ELevelType::MAP), g_wszInstanceModelLayer);

	if (m_pLevelMap)
		m_pLevelMap->On_ChangeSelectedObject(nullptr);

	/* 미리 한곳으로 합치기 */
	vector<CGameObject*> vecMapObject{};

	size_t iAllSize = (pStaticModelList == nullptr ? 0 : pStaticModelList->size()) + (pInstanceModelList == nullptr ? 0 : pInstanceModelList->size());
	vecMapObject.reserve(iAllSize);
	if(pStaticModelList) vecMapObject.insert(vecMapObject.end() , pStaticModelList->begin(), pStaticModelList->end());
	if(pInstanceModelList) vecMapObject.insert(vecMapObject.end() , pInstanceModelList->begin(), pInstanceModelList->end());

	/* 사용하는 모델주소가 같은 StaticModel을 모아둘 장소 */
	map<PairKey, vector<CMapObject*> > mapSameModels{};

	for (auto& MapObject : vecMapObject)
	{
	//	PairKey Key{};
	//	if (!MapObject)
	//		continue;
	//	CMapObject* pMapObject = static_cast<CMapObject*>(MapObject);
	//	CModel* pModel = pMapObject->Get_Component<CModel>();
	//	if (!pModel)
	//		continue;
	//	Key.first = pMapObject->Get_UsingModelInfo().wstrName;
	//	Key.second =  pMapObject->Get_TotalUseMtlsName();
	//	mapSameModels[Key].push_back(static_cast<CMapObject*>(pMapObject));
	//}

	//for (auto& Key : mapSameModels)
	//{
	//	for (auto& MapObject : Key.second)
	//	{
	//		if (!MapObject) continue;

	//		EMapObject_Type eMapObjectType = MapObject->Get_MapObjectType();
	//		if (eMapObjectType != EMapObject_Type::STATICMODEL && eMapObjectType != EMapObject_Type::INSTANCEMODEL) continue;

	//		vector<SRT_DATA> vecSRTData = MapObject->Get_SRTDatas();
	//		tInstanceModelDesc.tData.vecSRT.insert(tInstanceModelDesc.tData.vecSRT.end() , vecSRTData.begin() , vecSRTData.end());
	//	} 
	//	if (tInstanceModelDesc.tData.vecSRT.size() <= 1)
	//	{
	//		tInstanceModelDesc.tData.vecSRT.clear();
	//		continue;
	//	}
	//	else
	//	{
	//		tInstanceModelDesc.tData.tUsingModelInfo = Key.second.front()->Get_UsingModelInfo();
	//		Make_MapObject(EMapObject_Type::INSTANCEMODEL, &tInstanceModelDesc);
	//		tInstanceModelDesc.tData.vecSRT.clear();
	//		for (auto& MapObject : Key.second)
	//			m_pGameInstance->Request_DeleteGameObject(ENUM_TO_UINT(ELevelType::MAP), MapObject->Get_MapObjectType() ==  EMapObject_Type::STATICMODEL ?
	//				g_wszStaticModelLayer : g_wszInstanceModelLayer, MapObject);
	//	}
	}

	/* 그리고 한개짜리인 */



	return S_OK;
}

CMapObject* CMapToolManager::Make_MapObject(void* pArg, _bool isPreview)
{
	if ( m_funcMapObjectCloneFactory == nullptr) return nullptr;

	CGameObject* pCreatObject{ nullptr };

	if (!(pCreatObject = m_funcMapObjectCloneFactory(pArg)))
	{
		Safe_Release(pCreatObject);
		return nullptr;
	}

	if (isPreview)
		Delete_Preview();

	return  isPreview == true ? m_pPreviewMapobject = static_cast<CMapObject*>(pCreatObject) : static_cast<CMapObject*>(pCreatObject);
}


HRESULT CMapToolManager::Batch_Preview()
{
	return S_OK;
}

HRESULT CMapToolManager::Register_MapObjectCloneFactory()
{

	m_funcMapObjectCloneFactory =
		[=](void* pArg)->CGameObject* { return m_pGameInstance->Add_GameObject(ENUM_TO_UINT(ELevelType::MAP), L"Prototype_GameObject_MapObject",
			ENUM_TO_UINT(ELevelType::MAP), g_wszMapObjectLayer, pArg); };

	return S_OK;
}

void CMapToolManager::Free()
{
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	Safe_Release(m_pGameInstance);

	m_pLevelMap = nullptr;

	for (auto& Factory : m_arrayMapObjectCloneFactory)
		Factory = nullptr;
}
