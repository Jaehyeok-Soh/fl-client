#pragma once
#include "DataDocumentBase.h"
#include "DataStruct_CameraControlEvent.h"

NS_BEGIN(Engine)

class ENGINE_DLL CDataDocument_CameraControlEvent final : public CDataDocumentBase
{
	using Super = CDataDocumentBase;
private:
	CDataDocument_CameraControlEvent();
	virtual ~CDataDocument_CameraControlEvent() = default;

	HRESULT Initialize() { return S_OK; }
public:
	virtual DTO::ECategory Get_Category() const override { return DTO::ECategory::CAMERACONTROLEVENT; }
	virtual json ToJson() const override;
	virtual HRESULT FromJson(const json& j) override;

	HRESULT Upsert(const DTO::CAMERACONTROL_EVENT_INFO_DESC& data);
	const DTO::CAMERACONTROL_EVENT_INFO_DESC* Find_Data(const string& strOwnerTag) const;
private:
	IObjectDataBase* Create_ObjectData();
public:
	static CDataDocument_CameraControlEvent* Create();
	virtual void Free() override;
};

NS_END