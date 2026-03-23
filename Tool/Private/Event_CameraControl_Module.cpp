#include "pch.h"
#include "Event_CameraControl_Module.h"
#include "AnimObj.h"
#include "Model.h"
#include "CameraTypeConverters.h"
#include "ModelAnimation.h"
#include "GameInstance.h"

CEvent_CameraControl_Module::CEvent_CameraControl_Module()
{
}

HRESULT CEvent_CameraControl_Module::Initialize()
{
	return S_OK;
}

void CEvent_CameraControl_Module::Set_Owner(CAnimObj* pOwner)
{
	if (m_pOwner != nullptr && m_pOwner == pOwner)
		return;

	Safe_Release(m_pOwner);
	m_pOwner = pOwner;

	if (m_pOwner == nullptr)
		return;

    m_pOwnerModel = m_pOwner->Get_Component<CModel>();
	Ready_CameraPreviewState();
}

void CEvent_CameraControl_Module::Rebuild(const vector<DTO::CAMERACOTRNOL_EVENT>& vecEvents)
{
	m_vecEvents = vecEvents;
	Ready_CameraPreviewState();
}

void CEvent_CameraControl_Module::Modify_CAMERACOTRNOL_EVENT(const vector<DTO::CAMERACOTRNOL_EVENT>& vecEvents)
{
	m_vecEvents = vecEvents;
	Ready_CameraPreviewState();
}

void CEvent_CameraControl_Module::Modify_CAMERACOTRNOL_EVENT(_uint iIndex, const DTO::CAMERACOTRNOL_EVENT& tEvent)
{
	if (iIndex >= m_vecEvents.size())
		return;

	m_vecEvents[iIndex] = tEvent;
	Ready_CameraPreviewState();
}

void CEvent_CameraControl_Module::Preview_Event(_int iIndex)
{
    if (iIndex < 0 || iIndex >= (_int)m_vecEvents.size())
        return;

    const auto& evt = m_vecEvents[iIndex];

    auto* pGameInstance = CGameInstance::GetInstance();
    if (pGameInstance == nullptr)
        return;

    switch (evt.eCommand)
    {
    case DTO::EAnimCameraControlCommand::Shake:
        pGameInstance->Request_MainCameraShake(Engine::ToRuntimeDesc(evt.shake));
        break;

    case DTO::EAnimCameraControlCommand::FOV:
        pGameInstance->Request_MainCameraFOV(Engine::ToRuntimeDesc(evt.fov));
        break;

    case DTO::EAnimCameraControlCommand::RotationOffset:
        pGameInstance->Request_MainCameraRotationOffset(Engine::ToRuntimeDesc(evt.rotationOffset));
        break;

    case DTO::EAnimCameraControlCommand::PositionOffset:
        pGameInstance->Request_MainCameraPositionOffset(Engine::ToRuntimeDesc(evt.positionOffset));
        break;
    }
}

void CEvent_CameraControl_Module::Clear_PreviewState()
{
    Release_CameraPreview();
    Clear_CameraNotifies();
}

HRESULT CEvent_CameraControl_Module::Ready_CameraPreviewState()
{
    Release_CameraPreview();
    Clear_CameraNotifies();

    if (m_pOwnerModel == nullptr)
        return E_FAIL;

    auto& animations = m_pOwnerModel->Get_Animations();

    for (_uint i = 0; i < (_uint)m_vecEvents.size(); ++i)
    {
        auto& evt = m_vecEvents[i];

        if (evt.strAnimTag.empty())
            continue;

        const _int iResolvedAnimIndex =
            m_pOwnerModel->Get_AnimationIndex(Engine_Utils::ToWString(evt.strAnimTag));

        evt.iAnimIndex = iResolvedAnimIndex;

        if (iResolvedAnimIndex < 0 || iResolvedAnimIndex >= (_int)animations.size())
            continue;

        CModelAnimation* pAnimation = animations[iResolvedAnimIndex];
        if (pAnimation == nullptr)
            continue;

        AnimNotifyKey key = Build_CameraNotifyKey(evt, i);
        pAnimation->Pushback_Notifies(evt.ePhase, key);
    }

    for (auto& pAnimation : animations)
    {
        if (pAnimation)
            pAnimation->Sort_Notifies();
    }

    m_hCameraPreviewNotify = m_pOwnerModel->OnNotify.Subscribe(
        [this](const AnimNotifyKey& key)
        {
            this->Callback_CameraPreview(key);
        });

    return S_OK;
}

void CEvent_CameraControl_Module::Release_CameraPreview()
{
    if (m_pOwnerModel == nullptr)
        return;

    m_pOwnerModel->OnNotify.Unsubscribe(m_hCameraPreviewNotify);
}

void CEvent_CameraControl_Module::Clear_CameraNotifies()
{
    if (m_pOwnerModel == nullptr)
        return;

    auto& animations = m_pOwnerModel->Get_Animations();
    for (auto& pAnimation : animations)
    {
        if (pAnimation)
            pAnimation->Clear_Notifies(EAnimNotifyId::CameraControl);
    }
}

AnimNotifyKey CEvent_CameraControl_Module::Build_CameraNotifyKey(const DTO::CAMERACOTRNOL_EVENT& evt, _uint eventIndex) const
{
    AnimNotifyKey key{};
    key.eID = EAnimNotifyId::CameraControl;
    key.fTrackPosition = evt.fStartTrackPosition;
    key.iParam0 = eventIndex;
    return key;
}

void CEvent_CameraControl_Module::Callback_CameraPreview(const AnimNotifyKey& key)
{
    if (key.eID != EAnimNotifyId::CameraControl)
        return;

    const _int idx = static_cast<_int>(key.iParam0);
    if (idx < 0 || idx >= (_int)m_vecEvents.size())
        return;

    const auto& evt = m_vecEvents[idx];

    auto* pGameInstance = CGameInstance::GetInstance();
    if (pGameInstance == nullptr)
        return;

    switch (evt.eCommand)
    {
    case DTO::EAnimCameraControlCommand::Shake:
    {
        auto desc = Engine::ToRuntimeDesc(evt.shake);
        pGameInstance->Request_MainCameraShake(desc);
    }
    break;

    case DTO::EAnimCameraControlCommand::FOV:
    {
        auto desc = Engine::ToRuntimeDesc(evt.fov);
        pGameInstance->Request_MainCameraFOV(desc);
    }
    break;

    case DTO::EAnimCameraControlCommand::RotationOffset:
    {
        auto desc = Engine::ToRuntimeDesc(evt.rotationOffset);
        pGameInstance->Request_MainCameraRotationOffset(desc);
    }
    break;

    case DTO::EAnimCameraControlCommand::PositionOffset:
    {
        auto desc = Engine::ToRuntimeDesc(evt.positionOffset);
        pGameInstance->Request_MainCameraPositionOffset(desc);
    }
    break;
    }
}

CEvent_CameraControl_Module* CEvent_CameraControl_Module::Create()
{
	CEvent_CameraControl_Module* pInstance = new CEvent_CameraControl_Module();
	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("CEvent_CameraControl_Module::Create(), Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEvent_CameraControl_Module::Free()
{
    Clear_PreviewState();
	Super::Free();
}