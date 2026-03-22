#include "Engine_pch.h"
#include "Camera_Manager.h"
#include "Constant_Buffer.h"
#include "CinematicCamera.h"
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
	m_matPrevCameraWorld = Matrix::Identity;
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



HRESULT CCamera_Manager::Register_CinematicCamera(_uint iPrototypeLevelIndex, const wstring& wstrFindPrototypeTag, _uint iCloneLevelIndex, const wstring& wstrAddLagerTag, void* pCinematicCameraDesc)
{
	const  auto& iter = m_Cameras[ENUM_TO_UINT(CameraType::DYNAMIC)].find(g_wszCinematicCameraTag);

	if (iter != m_Cameras[ENUM_TO_UINT(CameraType::DYNAMIC)].end())
	{
		MSG_BOX(" 이미 Cinematic Camera가 등록되었습니다 ");
		return E_FAIL;
	}

	CCinematicCamera::CINEMATICCAMER_DESC* pDesc{ static_cast<CCinematicCamera::CINEMATICCAMER_DESC*>(pCinematicCameraDesc) };
	if (!pDesc)
	{
		/* 전역으로 설정 */
		static CCinematicCamera::CINEMATICCAMER_DESC tDesc{};
		tDesc.iLevelIndex = m_pGameInstnace->Get_CurrentLevelIndex();
		static CCamera::CAMERA_DESC tCamDesc{};
		/* 카메라 기본값 세팅 */
		tCamDesc.eProjectionType = EProjectionType::PERSPECTIVE;
		tCamDesc.fFar = 1000.f;
		tCamDesc.fNear = 0.1f;
		tCamDesc.fFov = XMConvertToRadians(60.f);
		tDesc.pCamera_Desc = &tCamDesc;
		pDesc = &tDesc;
	}

	CGameObject* pResult{nullptr};
	if (nullptr == (pResult = m_pGameInstnace->Add_GameObject(iPrototypeLevelIndex, wstrFindPrototypeTag, iCloneLevelIndex, wstrAddLagerTag, pDesc)))
		return E_FAIL;

	Add_Camera(CameraType::DYNAMIC , g_wszCinematicCameraTag , static_cast<CCameraMan*>(pResult));

	return S_OK;
}

void CCamera_Manager::Change_MainCamera(CameraType eType, const wstring& wstrTag)
{
	if (wstrTag.empty())
		return;

	auto itr = m_Cameras[ENUM_TO_UINT(eType)].find(wstrTag);
	if (itr == m_Cameras[ENUM_TO_UINT(eType)].end())
		return;

	/* Main Camera가 바뀌기전의 위치를 건내준다 */
	if (m_pMainCamera)
	{
		m_matPrevCameraWorld = m_pMainCamera->Get_Component<CTransform>()->Get_WorldMatrix();
		m_wstrPrevCameraName = m_pMainCamera->Get_WName();
		m_ePrevCameraType = m_pMainCamera->Get_Type();
	}

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

HRESULT CCamera_Manager::Play_MainCameraShake(const CAMERA_SHAKE_DESC& desc)
{
	if (m_pMainCamera == nullptr)
		return E_FAIL;

	m_pMainCamera->Play_CameraShake(desc);
	return S_OK;
}

HRESULT CCamera_Manager::Play_MainCameraFOV(const CAMERA_FOV_DESC& desc)
{
	if (m_pMainCamera == nullptr)
		return E_FAIL;

	m_pMainCamera->Play_CameraFOV(desc);
	return S_OK;
}

HRESULT CCamera_Manager::Play_MainCameraPositionOffset(const CAMERA_POSITION_OFFSET_DESC& desc)
{
	if (m_pMainCamera == nullptr)
		return E_FAIL;

	m_pMainCamera->Play_CameraPositionOffset(desc);
	return S_OK;
}

HRESULT CCamera_Manager::Play_MainCameraRotationOffset(const CAMERA_ROTATION_OFFSET_DESC& desc)
{
	if (m_pMainCamera == nullptr)
		return E_FAIL;

	m_pMainCamera->Play_CameraRotationOffset(desc);
	return S_OK;
}

HRESULT CCamera_Manager::Play_CameraCinematic(CinematicCameraSequence* pCameraCinematicSequence)
{
	if (pCameraCinematicSequence == nullptr) return E_FAIL;

	/* Main Camera 교체 */

	map<wstring, CCameraMan*>::iterator iter = m_Cameras[ENUM_TO_UINT(CameraType::DYNAMIC)].find(g_wszCinematicCameraTag);
	if (iter == m_Cameras[ENUM_TO_UINT(CameraType::DYNAMIC)].end()) return E_FAIL;
	CCinematicCamera* pCinematiCam = dynamic_cast<CCinematicCamera*>(iter->second);
	if (pCinematiCam == nullptr) return E_FAIL;

	pCinematiCam->Play_Cinematic(pCameraCinematicSequence);

	this->Change_MainCamera(CameraType::DYNAMIC,g_wszCinematicCameraTag);

	return S_OK;
}

HRESULT CCamera_Manager::End_CameraCinematic()
{
	Change_MainCamera(m_ePrevCameraType ,m_wstrPrevCameraName);

	/* 끝나면 바로 카메라 원상 복귀 */
	Update_ViewMatrix();

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

	pGo->Set_Name(wstrTag);

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

void CCamera_Manager::Update_AccTime_ForShader(const _float fTimeDelta)
{
	m_tGlobalDesc.fAccTime += fTimeDelta;
	if (m_tGlobalDesc.fAccTime > 10000.f)
		m_tGlobalDesc.fAccTime -= 10000.f;
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
