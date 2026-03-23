#pragma once
#include "Component.h"

NS_BEGIN(Engine)

class CModel;

class ENGINE_DLL CCameraEvent_Handler final : public CComponent
{
    using Super = CComponent;
public:
    constexpr static EComponentType _ID = EComponentType::CAMERAEVENT_HANDLER;
    typedef struct tagCameraEventHandlerDesc
    {
        vector<DTO::CAMERACOTRNOL_EVENT> vecCameraEvents;
    }CAMERA_EVENT_HANDLER_DESC;
private:
    CCameraEvent_Handler();
    CCameraEvent_Handler(const CCameraEvent_Handler& rhs);
    virtual ~CCameraEvent_Handler() = default;

    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
public:

    HRESULT Ready_Desc(void* pArg);
    void Set_Desc(const CAMERA_EVENT_HANDLER_DESC& desc);
    void Setup_ForOwner(CModel* pModel);

private:
    void Clear_CameraNotifies();
    HRESULT Ready_CameraState();
    void Release_Event();

    AnimNotifyKey Build_CameraNotifyKey(const DTO::CAMERACOTRNOL_EVENT& event, _uint iEventIndex) const;
    void CallbackEvent(const AnimNotifyKey& key);

private:
    CAMERA_EVENT_HANDLER_DESC m_tDesc = {};
    CModel* m_pOwnerModel = { nullptr };
    DelegateHandle m_EventHandle = {};
public:
    static CCameraEvent_Handler* Create();
    virtual CComponent* Clone(void* pArg) override;
    virtual void Free() override;
};

NS_END