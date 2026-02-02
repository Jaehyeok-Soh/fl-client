#include "pch.h"
#include "MapToolManager.h"
#include "GameInstance.h"
#include "MapObject.h"
#include "ImGui_ToolManager.h"
#include "Picking_ToolManager.h"
#include "StaticModel.h"
#include "MapToolManager.h"
#include "DebugLine.h"


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
	/* ÈÙ */
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

	return S_OK;
}

void CMapToolManager::Free()
{
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	Safe_Release(m_pGameInstance);



	for (auto& Factory : m_arrayMapObjectCloneFactory)
		Factory = nullptr;
}
