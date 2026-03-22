#pragma once
#include "ImGui_Panel.h"
#include "CameraMan.h"

NS_BEGIN(Tool)

class CPanel_Camera : public CImGui_Panel
{
	using Super = CImGui_Panel;
protected:
	explicit CPanel_Camera(const _char* pLabel, CLevel* pOwner, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CPanel_Camera() = default;
private:
	HRESULT	Initialize();
public:
	virtual HRESULT		Render(CToolObject* pGo)override;
	virtual void		Update(const _float fTimeDelta)override;
private:
	void				Reset_CameraPreviewDefaults();
	HRESULT				Render_CameraPreview();
	void				Render_CameraShaking();
	void				Render_CameraFov();
	void				Render_CameraRotationOffset();
	void				Render_CameraPositionOffset();
private:
	CAMERA_SHAKE_DATA m_tCamShakingData = {};
	CAMERA_FOV_DATA m_tCamFovData = {};
	CAMERA_ROTATION_OFFSET_DATA m_tCamRotationOffsetData = {};
	CAMERA_POSITION_OFFSET_DATA m_tCamPositionOffsetData = {};
private:
	CGameInstance* m_pGameInstance{ nullptr };
public:
	static			CPanel_Camera* Create(const _char* pLabel, CLevel* pOwner, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual void	Free() override;
};

NS_END