#include "Engine_pch.h"
#include "DataStruct_CameraControlEvent.h"
#pragma push_macro("new")
#undef new
#include "json.hpp"
using json = nlohmann::json;
#pragma pop_macro("new")

NS_BEGIN(Engine)

void to_json(json& j, const Float3& data)
{
	j["x"] = data.x;
	j["y"] = data.y;
	j["z"] = data.z;
}

void from_json(const json& j, Float3& data)
{
	if (j.contains("x"))
		j.at("x").get_to(data.x);
	if (j.contains("y"))
		j.at("y").get_to(data.y);
	if (j.contains("z"))
		j.at("z").get_to(data.z);
}

void to_json(json& j, const CAMERA_SHAKE_DATA& data)
{
	j["fPosAmplitude"] = data.fPosAmplitude;
	j["fYawAmplitudeDeg"] = data.fYawAmplitudeDeg;
	j["fPitchAmplitudeDeg"] = data.fPitchAmplitudeDeg;
	j["fFrequency"] = data.fFrequency;
	j["fDuration"] = data.fDuration;
}

void from_json(const json& j, CAMERA_SHAKE_DATA& data)
{
	if (j.contains("fPosAmplitude")) j.at("fPosAmplitude").get_to(data.fPosAmplitude);
	if (j.contains("fYawAmplitudeDeg")) j.at("fYawAmplitudeDeg").get_to(data.fYawAmplitudeDeg);
	if (j.contains("fPitchAmplitudeDeg")) j.at("fPitchAmplitudeDeg").get_to(data.fPitchAmplitudeDeg);
	if (j.contains("fFrequency")) j.at("fFrequency").get_to(data.fFrequency);
	if (j.contains("fDuration")) j.at("fDuration").get_to(data.fDuration);
}

void to_json(json& j, const CAMERA_FOV_DATA& data)
{
	j["eMode"] = data.eMode;
	j["fValueDeg"] = data.fValueDeg;
	j["fBlendInTime"] = data.fBlendInTime;
	j["fHoldTime"] = data.fHoldTime;
	j["fBlendOutTime"] = data.fBlendOutTime;
	j["bRestoreOnFinish"] = data.bRestoreOnFinish;
}

void from_json(const json& j, CAMERA_FOV_DATA& data)
{
	if (j.contains("eMode")) j.at("eMode").get_to(data.eMode);
	if (j.contains("fValueDeg")) j.at("fValueDeg").get_to(data.fValueDeg);
	if (j.contains("fBlendInTime")) j.at("fBlendInTime").get_to(data.fBlendInTime);
	if (j.contains("fHoldTime")) j.at("fHoldTime").get_to(data.fHoldTime);
	if (j.contains("fBlendOutTime")) j.at("fBlendOutTime").get_to(data.fBlendOutTime);
	if (j.contains("bRestoreOnFinish")) j.at("bRestoreOnFinish").get_to(data.bRestoreOnFinish);
}

void to_json(json& j, const CAMERA_POSITION_OFFSET_DATA& data)
{
	j["vOffset"] = data.vOffset;
	j["eSpace"] = data.eSpace;
	j["fBlendInTime"] = data.fBlendInTime;
	j["fHoldTime"] = data.fHoldTime;
	j["fBlendOutTime"] = data.fBlendOutTime;
}

void from_json(const json& j, CAMERA_POSITION_OFFSET_DATA& data)
{
	if (j.contains("vOffset")) j.at("vOffset").get_to(data.vOffset);
	if (j.contains("eSpace")) j.at("eSpace").get_to(data.eSpace);
	if (j.contains("fBlendInTime")) j.at("fBlendInTime").get_to(data.fBlendInTime);
	if (j.contains("fHoldTime")) j.at("fHoldTime").get_to(data.fHoldTime);
	if (j.contains("fBlendOutTime")) j.at("fBlendOutTime").get_to(data.fBlendOutTime);
}

void to_json(json& j, const CAMERA_ROTATION_OFFSET_DATA& data)
{
	j["fYawDeg"] = data.fYawDeg;
	j["fPitchDeg"] = data.fPitchDeg;
	j["fRollDeg"] = data.fRollDeg;
	j["fBlendInTime"] = data.fBlendInTime;
	j["fHoldTime"] = data.fHoldTime;
	j["fBlendOutTime"] = data.fBlendOutTime;
}

void from_json(const json& j, CAMERA_ROTATION_OFFSET_DATA& data)
{
	if (j.contains("fYawDeg")) j.at("fYawDeg").get_to(data.fYawDeg);
	if (j.contains("fPitchDeg")) j.at("fPitchDeg").get_to(data.fPitchDeg);
	if (j.contains("fRollDeg")) j.at("fRollDeg").get_to(data.fRollDeg);
	if (j.contains("fBlendInTime")) j.at("fBlendInTime").get_to(data.fBlendInTime);
	if (j.contains("fHoldTime")) j.at("fHoldTime").get_to(data.fHoldTime);
	if (j.contains("fBlendOutTime")) j.at("fBlendOutTime").get_to(data.fBlendOutTime);
}

NS_END

NS_BEGIN(DTO)
void to_json(json& j, const CAMERACOTRNOL_EVENT& data)
{
	to_json(j, static_cast<const ANIM_EVENT_BASE1&>(data));

	j["eCommand"] = data.eCommand;
	j["shake"] = data.shake;
	j["fov"] = data.fov;
	j["rotationOffset"] = data.rotationOffset;
	j["positionOffset"] = data.positionOffset;
}

void from_json(const json& j, CAMERACOTRNOL_EVENT& data)
{
	from_json(j, static_cast<ANIM_EVENT_BASE1&>(data));

	if (j.contains("eCommand")) j.at("eCommand").get_to(data.eCommand);

	if (j.contains("shake")) j.at("shake").get_to(data.shake);
	if (j.contains("fov")) j.at("fov").get_to(data.fov);
	if (j.contains("rotationOffset")) j.at("rotationOffset").get_to(data.rotationOffset);
	if (j.contains("positionOffset")) j.at("positionOffset").get_to(data.positionOffset);
}

void to_json(json& j, const CAMERACONTROL_EVENT_INFO_DESC& data)
{
	j["strOwnerTag"] = data.strOwnerTag;
	j["vecCameraControlEvents"] = data.vecCameraControlEvents;
}

void from_json(const json& j, CAMERACONTROL_EVENT_INFO_DESC& data)
{
	if (j.contains("strOwnerTag")) j.at("strOwnerTag").get_to(data.strOwnerTag);
	if (j.contains("vecCameraControlEvents")) j.at("vecCameraControlEvents").get_to(data.vecCameraControlEvents);
}
NS_END

NS_BEGIN(Engine)

json CDataStruct_CameraControlEvent::ToJson() const
{
	json j = json(m_Data);
	j["Type"] = Get_Type();
	return j;
}

HRESULT CDataStruct_CameraControlEvent::FromJson(const json& j)
{
	m_Data = j.get<DTO::CAMERACONTROL_EVENT_INFO_DESC>();
	return S_OK;
}

NS_END