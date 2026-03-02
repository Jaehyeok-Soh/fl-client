#include "pch.h"
#include "CPanel_Camera.h"
#include "GameInstance.h"




CPanel_Camera::CPanel_Camera(const _char* pLabel, CLevel* pOwner, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CImGui_Panel(pLabel , pOwner , pDevice ,pDeviceContext) , m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}


HRESULT CPanel_Camera::Initialize()
{


	return S_OK;
}

HRESULT CPanel_Camera::Render(CToolObject* pGo)
{
	if (ImGui::CollapsingHeader(" Camera Shaking "))
	{
		if (FAILED(Render_CameraShaking()))
		{
			ImGui::TreePop();
			return E_FAIL;
		}
	}

	return S_OK;
}

void CPanel_Camera::Update(const _float fTimeDelta)
{

}

HRESULT CPanel_Camera::Render_CameraShaking()
{
	ImGui::Separator();

	if (ImGui::Button(" Play Shaking "))
	{
		/* 현재 데이터 기준으로 카메라 쉐이킹 */
		m_pGameInstance->Camera_Shaking(m_tCamShakingData);
	}

	ImGui::Separator();

	ImGui::NewLine();

	ImGui::SeparatorText(" Camera Shaking Data Info ");

	ImGui::InputFloat(" Power ", &m_tCamShakingData.fPower);

	ImGui::NewLine();

	ImGui::InputFloat(" Time ", &m_tCamShakingData.fTime);


	ImGui::Separator();

	return S_OK;
}



CPanel_Camera* CPanel_Camera::Create(const _char* pLabel, CLevel* pOwner, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CPanel_Camera* pPanel_Camera = new CPanel_Camera(pLabel, pOwner, pDevice, pDeviceContext);

	if (FAILED(pPanel_Camera->Initialize()))
	{
		Safe_Release(pPanel_Camera);
		MSG_BOX(" Panel Camera is Failed To Craet ");
		return nullptr;
	}

	return pPanel_Camera;
}

void CPanel_Camera::Free()
{
	Super::Free();

	Safe_Release(m_pGameInstance);
}


