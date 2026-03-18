#include "Engine_pch.h"
#include "Cinematic_Manager.h"
#include "CinematicCameraSequnce.h"
#include "GameInstance.h"

CCinematic_Manager::CCinematic_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const _tchar* wszCameraCinematicDataJsonPath)
	: m_pDevice{ nullptr }
	, m_pDeviceContext{ nullptr }
	, m_pGameInstance{ CGameInstance::GetInstance() }
	, m_mapCameraCinematicSequence{}
	, m_wszCameraCinematicDataPath{ nullptr }
{
	Safe_AddRef(m_pGameInstance);
}






HRESULT CCinematic_Manager::Load_CameraCinematicSequence(const wstring& wstrFindKey, OUT CCamera_Cinematic_Sequence* pOutCamCinematicSequence)
{
	return S_OK;
}

HRESULT CCinematic_Manager::Save_CameraCinematicSequence(const wstring& wstrFindKey, const CCamera_Cinematic_Sequence* pSaveCamCinematicSequence)
{
	return S_OK;
}


void CCinematic_Manager::Free()
{
	Super::Free();
}
