#pragma once
#include "Base.h"

NS_BEGIN(Engine)
class CGameInstance;
class CModel;
NS_END

NS_BEGIN(Tool)

class CAnimObj;

class CEvent_CameraControl_Module final : public CBase
{
	using Super = CBase;
private:
	CEvent_CameraControl_Module();
	virtual ~CEvent_CameraControl_Module() = default;
	HRESULT Initialize();
public:
    void Set_Owner(CAnimObj* pOwner);
	void Rebuild(const vector<DTO::CAMERACOTRNOL_EVENT>& vecEvents);
	void Modify_CAMERACOTRNOL_EVENT(const vector<DTO::CAMERACOTRNOL_EVENT>& vecEvents);
	void Modify_CAMERACOTRNOL_EVENT(_uint iIndex, const DTO::CAMERACOTRNOL_EVENT& tEvent);

	const vector<DTO::CAMERACOTRNOL_EVENT>& GetEvents() const { return m_vecEvents; }
	vector<DTO::CAMERACOTRNOL_EVENT>& GetEvents() { return m_vecEvents;	}

    void Preview_Event(_int iIndex);
    void Clear_PreviewState();
private:
    HRESULT Ready_CameraPreviewState();
    void Release_CameraPreview();
    void Clear_CameraNotifies();

    AnimNotifyKey Build_CameraNotifyKey(const DTO::CAMERACOTRNOL_EVENT& evt, _uint eventIndex) const;
    void Callback_CameraPreview(const AnimNotifyKey& key);

private:
    CAnimObj* m_pOwner = nullptr;
    CModel* m_pOwnerModel = nullptr;

    vector<DTO::CAMERACOTRNOL_EVENT> m_vecEvents;

    DelegateHandle m_hCameraPreviewNotify = {};

public:
    static CEvent_CameraControl_Module* Create();
    virtual void Free() override;
};

NS_END