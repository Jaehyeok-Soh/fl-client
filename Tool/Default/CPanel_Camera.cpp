#include "pch.h"
#include "CameraTypeConverters.h"
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
		if (FAILED(Render_CameraPreview()))
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

void CPanel_Camera::Reset_CameraPreviewDefaults()
{
    m_tCamShakingData = {};
    m_tCamShakingData.fPosAmplitude = 0.03f;
    m_tCamShakingData.fYawAmplitudeDeg = 0.4f;
    m_tCamShakingData.fPitchAmplitudeDeg = 0.25f;
    m_tCamShakingData.fFrequency = 25.f;
    m_tCamShakingData.fDuration = 0.12f;

    m_tCamFovData = {};
    m_tCamFovData.eMode = Engine::ECameraFovMode::Delta;
    m_tCamFovData.fValueDeg = -6.f;
    m_tCamFovData.fBlendInTime = 0.04f;
    m_tCamFovData.fHoldTime = 0.02f;
    m_tCamFovData.fBlendOutTime = 0.08f;
    m_tCamFovData.bRestoreOnFinish = true;

    m_tCamRotationOffsetData = {};
    m_tCamRotationOffsetData.fBlendInTime = 0.02f;
    m_tCamRotationOffsetData.fHoldTime = 0.f;
    m_tCamRotationOffsetData.fBlendOutTime = 0.08f;

    m_tCamPositionOffsetData = {};
    m_tCamPositionOffsetData.eSpace = Engine::ECameraSpace::Camera_Local;
    m_tCamPositionOffsetData.fBlendInTime = 0.02f;
    m_tCamPositionOffsetData.fHoldTime = 0.f;
    m_tCamPositionOffsetData.fBlendOutTime = 0.08f;
}

HRESULT CPanel_Camera::Render_CameraPreview()
{
    if (ImGui::CollapsingHeader("Shake", ImGuiTreeNodeFlags_DefaultOpen))
        Render_CameraShaking();

    if (ImGui::CollapsingHeader("FOV", ImGuiTreeNodeFlags_DefaultOpen))
        Render_CameraFov();

    if (ImGui::CollapsingHeader("Rotation Offset", ImGuiTreeNodeFlags_DefaultOpen))
        Render_CameraRotationOffset();

    if (ImGui::CollapsingHeader("Position Offset", ImGuiTreeNodeFlags_DefaultOpen))
        Render_CameraPositionOffset();

    return S_OK;
}

void CPanel_Camera::Render_CameraShaking()
{
    ImGui::Separator();

    if (ImGui::Button("Preview Shake", ImVec2(160.f, 0.f)))
    {
        m_pGameInstance->Request_MainCameraShake(
            ToRuntimeDesc(m_tCamShakingData));
    }

    ImGui::Spacing();
    ImGui::SeparatorText("Shake Data");

    ImGui::DragFloat("Pos Amplitude", &m_tCamShakingData.fPosAmplitude, 0.001f, 0.f, 10.f, "%.3f");
    ImGui::DragFloat("Yaw Amplitude Deg", &m_tCamShakingData.fYawAmplitudeDeg, 0.01f, 0.f, 180.f, "%.2f");
    ImGui::DragFloat("Pitch Amplitude Deg", &m_tCamShakingData.fPitchAmplitudeDeg, 0.01f, 0.f, 180.f, "%.2f");
    ImGui::DragFloat("Frequency", &m_tCamShakingData.fFrequency, 0.1f, 0.f, 200.f, "%.2f");
    ImGui::DragFloat("Duration", &m_tCamShakingData.fDuration, 0.01f, 0.f, 10.f, "%.2f");

    ImGui::Separator();
}

void CPanel_Camera::Render_CameraFov()
{
    static const char* s_FovModeItems[] =
    {
        "Delta",
        "Absolute"
    };

    ImGui::Separator();

    if (ImGui::Button("Preview FOV", ImVec2(160.f, 0.f)))
    {
        m_pGameInstance->Request_MainCameraFOV(
            ToRuntimeDesc(m_tCamFovData));
    }

    ImGui::Spacing();
    ImGui::SeparatorText("FOV Data");

    _int iMode = static_cast<_int>(m_tCamFovData.eMode);
    if (ImGui::Combo("FOV Mode", &iMode, s_FovModeItems, IM_ARRAYSIZE(s_FovModeItems)))
    {
        m_tCamFovData.eMode = static_cast<ECameraFovMode>(iMode);
    }

    ImGui::DragFloat("FOV Value Deg", &m_tCamFovData.fValueDeg, 0.1f, -180.f, 180.f, "%.2f");
    ImGui::DragFloat("Blend In", &m_tCamFovData.fBlendInTime, 0.01f, 0.f, 10.f, "%.2f");
    ImGui::DragFloat("Hold", &m_tCamFovData.fHoldTime, 0.01f, 0.f, 10.f, "%.2f");
    ImGui::DragFloat("Blend Out", &m_tCamFovData.fBlendOutTime, 0.01f, 0.f, 10.f, "%.2f");
    ImGui::Checkbox("Restore On Finish", &m_tCamFovData.bRestoreOnFinish);

    ImGui::Separator();
}

void CPanel_Camera::Render_CameraRotationOffset()
{
    ImGui::Separator();

    if (ImGui::Button("Preview Rotation Offset", ImVec2(160.f, 0.f)))
    {
        m_pGameInstance->Request_MainCameraRotationOffset(
            ToRuntimeDesc(m_tCamRotationOffsetData));
    }

    ImGui::Spacing();
    ImGui::SeparatorText("Rotation Offset Data");

    ImGui::DragFloat("Yaw Deg", &m_tCamRotationOffsetData.fYawDeg, 0.1f, -180.f, 180.f, "%.2f");
    ImGui::DragFloat("Pitch Deg", &m_tCamRotationOffsetData.fPitchDeg, 0.1f, -180.f, 180.f, "%.2f");
    ImGui::DragFloat("Roll Deg", &m_tCamRotationOffsetData.fRollDeg, 0.1f, -180.f, 180.f, "%.2f");

    ImGui::Spacing();
    ImGui::SeparatorText("Timing");

    ImGui::DragFloat("Blend In", &m_tCamRotationOffsetData.fBlendInTime, 0.01f, 0.f, 10.f, "%.2f");
    ImGui::DragFloat("Hold", &m_tCamRotationOffsetData.fHoldTime, 0.01f, 0.f, 10.f, "%.2f");
    ImGui::DragFloat("Blend Out", &m_tCamRotationOffsetData.fBlendOutTime, 0.01f, 0.f, 10.f, "%.2f");

    ImGui::Separator();
}

void CPanel_Camera::Render_CameraPositionOffset()
{
    static const char* s_SpaceItems[] =
    {
        "World",
        "Camera Local"
    };

    ImGui::Separator();

    if (ImGui::Button("Preview Position Offset", ImVec2(160.f, 0.f)))
    {
        m_pGameInstance->Request_MainCameraPositionOffset(
            ToRuntimeDesc(m_tCamPositionOffsetData));
    }

    ImGui::Spacing();
    ImGui::SeparatorText("Position Offset Data");

    ImGui::DragFloat3("Offset", &m_tCamPositionOffsetData.vOffset.x, 0.01f, -100.f, 100.f, "%.2f");

    _int iSpace = static_cast<_int>(m_tCamPositionOffsetData.eSpace);
    if (ImGui::Combo("Space", &iSpace, s_SpaceItems, IM_ARRAYSIZE(s_SpaceItems)))
    {
        m_tCamPositionOffsetData.eSpace = static_cast<ECameraSpace>(iSpace);
    }

    ImGui::Spacing();
    ImGui::SeparatorText("Timing");

    ImGui::DragFloat("Blend In", &m_tCamPositionOffsetData.fBlendInTime, 0.01f, 0.f, 10.f, "%.2f");
    ImGui::DragFloat("Hold", &m_tCamPositionOffsetData.fHoldTime, 0.01f, 0.f, 10.f, "%.2f");
    ImGui::DragFloat("Blend Out", &m_tCamPositionOffsetData.fBlendOutTime, 0.01f, 0.f, 10.f, "%.2f");

    ImGui::Separator();
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


