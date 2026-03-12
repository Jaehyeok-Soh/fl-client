#include "Engine_pch.h"
#include "Camera_Manager.h"
#include "Constant_Buffer.h"
#include "GameInstance.h"

CCamera_Manager::CCamera_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: m_pDevice(pDevice)
	, m_pDeviceContext(pDeviceContext)
	, m_pGameInstnace{CGameInstance::GetInstance()}
{
	Safe_AddRef(m_pGameInstnace);
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);
	m_matView = Matrix::Identity;
	m_matProjection = Matrix::Identity;
	m_matView_UI = Matrix::Identity;
	m_matProjection_UI = Matrix::Identity;
}

HRESULT CCamera_Manager::Initiailize()
{
	D3D11_VIEWPORT          ViewportDesc{};
	_uint					iNumViewports = { 1 };
	m_pDeviceContext->RSGetViewports(&iNumViewports, &ViewportDesc);
	m_matProjection_UI = ::XMMatrixOrthographicLH(ViewportDesc.Width, ViewportDesc.Height, 0.f, 1.f);
	Create_ConstantBuffer();
	return S_OK;
}

void CCamera_Manager::Change_Target(CGameObject* pGo)
{
	if (nullptr == pGo)
		return;

	if (m_pMainCamera)
	{
		if (m_pMainCamera->Get_Type() == CameraType::STATIC)
			return;
	}
	else
		return;

	auto itr = m_Actors.find(pGo);
	if (itr == m_Actors.end())
		return;

	pGo->Set_CameraTargeter(m_pMainCamera);
	m_pMainCamera->Change_Actor(pGo);
}

void CCamera_Manager::Change_MainCamera(CameraType eType, const wstring& wstrTag)
{
	if (wstrTag.empty())
		return;

	auto itr = m_Cameras[ENUM_TO_UINT(eType)].find(wstrTag);
	if (itr == m_Cameras[ENUM_TO_UINT(eType)].end())
		return;

	Safe_Release(m_pMainCamera);
	CCameraMan* pCameraMan = itr->second;
	Safe_AddRef(pCameraMan);
	m_pMainCamera = pCameraMan;
	Update_ViewMatrix();
}

HRESULT CCamera_Manager::Change_Target_Next()
{
	if (m_Actors.size() <= 0)
		return E_FAIL;;

	if (m_pMainCamera->Get_Type() == CameraType::STATIC)
	{
		auto itr = m_Cameras[ENUM_TO_UINT(CameraType::DYNAMIC)].find(g_MainActorCameraName);
		if (itr == m_Cameras[ENUM_TO_UINT(CameraType::DYNAMIC)].end())
			return E_FAIL;

		Safe_Release(m_pMainCamera);
		CCameraMan* pCameraMan = itr->second;
		Safe_AddRef(pCameraMan);
		m_pMainCamera = pCameraMan;
	}
	else if (m_pMainCamera->Get_Type() == CameraType::DYNAMIC)
	{
		CGameObject* pActor = m_pMainCamera->Get_Actor();
		auto itr = m_Actors.find(pActor);
		if (itr == m_Actors.end() || ++itr == m_Actors.end())
			itr = m_Actors.begin();

		CGameObject* pTarget = *(itr);
		pTarget->Set_CameraTargeter(m_pMainCamera);
		m_pMainCamera->Change_Actor(pTarget);
	}

	return S_OK;
}

HRESULT CCamera_Manager::Camera_Shaking(const CAM_SHAKING_DATA& tData)
{
	if (m_pMainCamera == nullptr) return E_FAIL;

	m_pMainCamera->Camera_Shaking(tData);

	return S_OK;
}

HRESULT CCamera_Manager::Play_CameraCinematic(Camera_Cinematic_Sequence* pCameraCinematicSequence)
{
	if (pCameraCinematicSequence == nullptr) return E_FAIL;

	m_pMainCamera->Cinematic(pCameraCinematicSequence);

	return S_OK;
}

void CCamera_Manager::Add_Actor_Object(CGameObject* pGo, _bool bImmediatelyChange)
{
	if (nullptr == pGo)
		return;

	auto itr = m_Actors.find(pGo);
	if (itr != m_Actors.end())
		return;

	Safe_AddRef(pGo);
	m_Actors.emplace(pGo);
	if (bImmediatelyChange && m_pMainCamera->Get_Type() == CameraType::DYNAMIC)
		m_pMainCamera->Change_Actor(pGo);
}

void CCamera_Manager::Remove_Actor_Object(CGameObject* pGo)
{
	if (nullptr == pGo)
		return;

	auto itr = m_Actors.find(pGo);
	if (itr == m_Actors.end())
		return;

	Safe_Release(pGo);
	m_Actors.erase(itr);
}

void CCamera_Manager::Add_Camera(CameraType eType, const wstring& wstrTag, CCameraMan* pGo)
{
	if (nullptr == pGo || wstrTag.empty())
		return;

	auto itr = m_Cameras[ENUM_TO_UINT(eType)].find(wstrTag);
	if (itr != m_Cameras[ENUM_TO_UINT(eType)].end())
		return;

	Safe_AddRef(pGo);
	m_Cameras[ENUM_TO_UINT(eType)].insert(map<wstring, CCameraMan*>::value_type(wstrTag, pGo));
}

void CCamera_Manager::Remove_Camera(CameraType eType, const wstring& wstrTag)
{
	if (wstrTag.empty())
		return;

	auto itr = m_Cameras[ENUM_TO_UINT(eType)].find(wstrTag);
	if (itr == m_Cameras[ENUM_TO_UINT(eType)].end())
		return;

	CCameraMan* pCameraMan = itr->second;
	Safe_Release(pCameraMan);
	m_Cameras[ENUM_TO_UINT(eType)].erase(itr);
}

void CCamera_Manager::Update_ViewMatrix()
{
	if (!m_pMainCamera)
		return;
	m_matView = m_pMainCamera->Get_Component<CCamera>()->Get_ViewMatrix();
	m_matProjection = m_pMainCamera->Get_Component<CCamera>()->Get_ProjectionMatrix();
}

void CCamera_Manager::Setup_ViewProj_ToCBuffer()
{
	m_tGlobalDesc.matView = m_matView;
	m_tGlobalDesc.matProj = m_matProjection;
	m_tGlobalDesc.matVP = m_tGlobalDesc.matView * m_tGlobalDesc.matProj;
	m_pGlobal_CBuffer->Copy_Data(m_tGlobalDesc);
}

void CCamera_Manager::Setup_UIViewProj_ToCBuffer()
{
	m_tGlobalDesc.matView = m_matView_UI;
	m_tGlobalDesc.matProj = m_matProjection_UI;
	m_tGlobalDesc.matVP = m_tGlobalDesc.matView * m_tGlobalDesc.matProj;
	m_pGlobal_CBuffer->Copy_Data(m_tGlobalDesc);
}

void CCamera_Manager::Setup_Inv_ToCBuffer()
{
	m_tInvDesc.matCamView = m_matView;
	m_tInvDesc.matCamProj = m_matProjection;
	m_tInvDesc.matInvView = m_matView.Invert();
	m_tInvDesc.matInvProj = m_matProjection.Invert();
	m_pInv_CBuffer->Copy_Data(m_tInvDesc);
}

ID3D11Buffer* CCamera_Manager::Get_Inv_ConstantBuffer()
{
	return m_pInv_CBuffer->Get_Buffer();
}

ID3D11Buffer* CCamera_Manager::Get_Global_ConstantBuffer()
{
	return m_pGlobal_CBuffer->Get_Buffer();
}

void CCamera_Manager::Clear()
{
	for (CGameObject* pElement : m_Actors)
	{
		Safe_Release(pElement);
	}
	m_Actors.clear();

	for (auto& Container : m_Cameras)
	{
		for (auto& Pair : Container)
		{
			Safe_Release(Pair.second);
		}
		Container.clear();
	}
}

void CCamera_Manager::Create_ConstantBuffer()
{
	// Global
	{
		m_pGlobal_CBuffer = CConstant_Buffer<SHADER_GLOBALDESC>::Create(m_pDevice, m_pDeviceContext);
	}
	// Inv
	{
		m_pInv_CBuffer = CConstant_Buffer<SHADER_INVDESC>::Create(m_pDevice, m_pDeviceContext);
	}
}

CCamera_Manager* CCamera_Manager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CCamera_Manager* pInstance = new CCamera_Manager(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initiailize()))
	{
		MSG_BOX("CCamera_Manager::Create, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCamera_Manager::Free()
{
	Clear();
	Safe_Release(m_pGameInstnace);
	Safe_Release(m_pInv_CBuffer);
	Safe_Release(m_pGlobal_CBuffer);
	Safe_Release(m_pMainCamera);
	Safe_Release(m_pDeviceContext);
	Safe_Release(m_pDevice);
	Clear();
}
