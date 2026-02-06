#include "pch.h"
#include "MapToolManager.h"
#include "GameInstance.h"
#include "MapObject.h"
#include "ImGui_ToolManager.h"
#include "Picking_ToolManager.h"
#include "StaticModel.h"
#include "InstanceModel.h"
#include "DebugLine.h"
#include "Model.h"
#include "Level_Map.h"

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
		PairKey Key{};
		if (!MapObject)
			continue;
		CMapObject* pMapObject = static_cast<CMapObject*>(MapObject);
		CModel* pModel = pMapObject->Get_Component<CModel>();
		if (!pModel)
			continue;
		Key.first = pMapObject->Get_UsingModelInfo().wstrName;
		Key.second =  pMapObject->Get_TotalUseMtlsName();
		mapSameModels[Key].push_back(static_cast<CMapObject*>(pMapObject));
	}

	CInstanceModel::INSTANCEMODEL_DESC tInstanceModelDesc{};
	tInstanceModelDesc.isLoaded = true;
	tInstanceModelDesc.eType = EMapObject_Type::INSTANCEMODEL;
	tInstanceModelDesc.iLevelIndex = ENUM_TO_UINT(ELevelType::MAP);
	tInstanceModelDesc.eState = CMapObject::EState::Default;
	tInstanceModelDesc.wstrLayerTag = g_wszInstanceModelLayer;

	for (auto& Key : mapSameModels)
	{
		for (auto& MapObject : Key.second)
		{
			if (!MapObject) continue;

			EMapObject_Type eMapObjectType = MapObject->Get_MapObjectType();
			if (eMapObjectType != EMapObject_Type::STATICMODEL && eMapObjectType != EMapObject_Type::INSTANCEMODEL) continue;

			vector<SRT_DATA> vecSRTData = MapObject->Get_SRTDatas();
			tInstanceModelDesc.tData.vecSRT.insert(tInstanceModelDesc.tData.vecSRT.end() , vecSRTData.begin() , vecSRTData.end());
		} 
		if (tInstanceModelDesc.tData.vecSRT.size() <= 1)
		{
			tInstanceModelDesc.tData.vecSRT.clear();
			continue;
		}
		else
		{
			tInstanceModelDesc.tData.tUsingModelInfo = Key.second.front()->Get_UsingModelInfo();
			Make_MapObject(EMapObject_Type::INSTANCEMODEL, &tInstanceModelDesc);
			tInstanceModelDesc.tData.vecSRT.clear();
			for (auto& MapObject : Key.second)
				m_pGameInstance->Request_DeleteGameObject(ENUM_TO_UINT(ELevelType::MAP), MapObject->Get_MapObjectType() ==  EMapObject_Type::STATICMODEL ?
					g_wszStaticModelLayer : g_wszInstanceModelLayer, MapObject);
		}
	}

	/* 그리고 한개짜리인 */



	return S_OK;
}




CMapObject* CMapToolManager::Make_MapObject(EMapObject_Type eType, void* pArg, _bool isPreview)
{
	auto& Factory = m_arrayMapObjectCloneFactory[ENUM_TO_UINT(eType)];

	if (Factory == nullptr) return nullptr;

	CGameObject* pCreatObject{ nullptr };

	if (!(pCreatObject = Factory(pArg)))
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

	m_arrayMapObjectCloneFactory[ENUM_TO_UINT(EMapObject_Type::STATICMODEL)] =
		[=](void* pArg)->CGameObject* { return m_pGameInstance->Add_GameObject(ENUM_TO_UINT(ELevelType::MAP),L"Prototype_GameObject_StaticModel",
			ENUM_TO_UINT(ELevelType::MAP),g_wszStaticModelLayer,pArg);};

	m_arrayMapObjectCloneFactory[ENUM_TO_UINT(EMapObject_Type::INSTANCEMODEL)] =
		[=](void* pArg)->CGameObject* { return m_pGameInstance->Add_GameObject(ENUM_TO_UINT(ELevelType::MAP), L"Prototype_GameObject_InstanceModel",
			ENUM_TO_UINT(ELevelType::MAP), g_wszInstanceModelLayer, pArg); };

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
