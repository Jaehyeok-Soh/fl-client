#pragma once
#include "ObjectDataBase.h"
#include "CameraDataTypes.h"
#include "Anim_Event_Base.h"

NS_BEGIN(Engine)
NLOHMANN_JSON_SERIALIZE_ENUM(ECameraSpace,
    {
        {ECameraSpace::World, "World"},
        {ECameraSpace::Camera_Local, "Camera_Local"},
    }
)

NLOHMANN_JSON_SERIALIZE_ENUM(ECameraFovMode,
    {
        {ECameraFovMode::Delta, "Delta"},
        {ECameraFovMode::Absolute, "Absolute"},
    }
)

void to_json(json& j, const Float3& data);
void from_json(const json& j, Float3& data);
void to_json(json& j, const CAMERA_SHAKE_DATA& data);
void from_json(const json& j, CAMERA_SHAKE_DATA& data);
void to_json(json& j, const CAMERA_FOV_DATA& data);
void from_json(const json& j, CAMERA_FOV_DATA& data);
void to_json(json& j, const CAMERA_POSITION_OFFSET_DATA& data);
void from_json(const json& j, CAMERA_POSITION_OFFSET_DATA& data);
void to_json(json& j, const CAMERA_ROTATION_OFFSET_DATA& data);
void from_json(const json& j, CAMERA_ROTATION_OFFSET_DATA& data);
NS_END

NS_BEGIN(DTO)
enum class EAnimCameraControlCommand : unsigned int
{
    Shake = 0,
    FOV,
    RotationOffset,
    PositionOffset,
    END
};

NLOHMANN_JSON_SERIALIZE_ENUM(EAnimCameraControlCommand,
    {
        {EAnimCameraControlCommand::Shake, "Shake"},
        {EAnimCameraControlCommand::FOV, "FOV"},
        {EAnimCameraControlCommand::RotationOffset, "RotationOffset"},
        {EAnimCameraControlCommand::PositionOffset, "PositionOffset"},
        {EAnimCameraControlCommand::END, "END"},
    }
)

typedef struct tagCameraControlEvent : public ANIM_EVENT_BASE1
{
    EAnimCameraControlCommand   eCommand = { EAnimCameraControlCommand::END };
    
    Engine::CAMERA_SHAKE_DATA shake{};
    Engine::CAMERA_FOV_DATA fov{};
    Engine::CAMERA_POSITION_OFFSET_DATA positionOffset{};
    Engine::CAMERA_ROTATION_OFFSET_DATA rotationOffset{};
}CAMERACOTRNOL_EVENT;

typedef struct tagCameraControlEventInfoDesc
{
    string              strOwnerTag = { "Sample" };
    vector<CAMERACOTRNOL_EVENT>  vecCameraControlEvents;
}CAMERACONTROL_EVENT_INFO_DESC;

void to_json(json& j, const CAMERACOTRNOL_EVENT& data);
void from_json(const json& j, CAMERACOTRNOL_EVENT& data);
void to_json(json& j, const CAMERACONTROL_EVENT_INFO_DESC& data);
void from_json(const json& j, CAMERACONTROL_EVENT_INFO_DESC& data);
NS_END


NS_BEGIN(Engine)

class CDataStruct_CameraControlEvent final : public IObjectDataBase
{
    using Super = IObjectDataBase;
private:
    CDataStruct_CameraControlEvent() = default;
    virtual ~CDataStruct_CameraControlEvent() = default;
public:
    virtual _uint Get_Type() const override { return ENUM_TO_UINT(DTO::ECategory::CAMERACONTROLEVENT); }
    virtual const _string& Get_Tag() const override { return m_Data.strOwnerTag; }

    virtual json ToJson() const override;
    virtual HRESULT FromJson(const json& j) override;

    const DTO::CAMERACONTROL_EVENT_INFO_DESC& Get_Data() const { return m_Data; }
    DTO::CAMERACONTROL_EVENT_INFO_DESC& Get_Data() { return m_Data; }
private:
    DTO::CAMERACONTROL_EVENT_INFO_DESC m_Data;

public:
    static CDataStruct_CameraControlEvent* Create() { return new CDataStruct_CameraControlEvent(); }
    virtual void Free() override { Super::Free(); }
};

NS_END