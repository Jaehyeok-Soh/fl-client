#pragma once
#include "ImGui_Panel.h"
#include "MapToolManager.h"

NS_BEGIN(Engine)

class CCameraMan;
class CCamera;

NS_END


NS_BEGIN(Tool)

class CMapToolManager;

class CPanel_MapTool :
	public CImGui_Panel
{
	using Super = CImGui_Panel;
protected:
	explicit CPanel_MapTool(const _char* pLabel, CLevel* pOwner, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CPanel_MapTool() = default;
private:
	HRESULT					Initialize();
	HRESULT					Make_DefaultTexture();
public:
	virtual HRESULT			Render(CToolObject* pGo)override;
	virtual void			Update(const _float fTimeDelta)override;
	HRESULT					Update_MapObjectList();
	HRESULT					Update_TextureSplatingInfoDataName();
private:

	/* 전체적인 맵지형에 사용될 Texture 선택 ImGUi */
	HRESULT					Render_SplatingTextureSetting();
	HRESULT					Render_Single_Channel_Setting(const char* szLabel, const ImVec4& vColor,OUT int& iConnectedIndex, OUT float& fForce, OUT float& fHeightForce, int& iFlag,OUT bool* pIsOpenPopup);

	HRESULT					Render_RaySetting();
	HRESULT					Render_CameraSetting();
	HRESULT					Render_PreViewInfo();
	
	/* Save Scene Data Setting */
	HRESULT					Render_SaveLevelDataSetting();

	/* MapTool Setting Render */
	HRESULT					Render_CheckAndBind();
	HRESULT					Render_MakeMapObjectSetting();

	/* Camera Cinematic Sequence */
	HRESULT					Render_CameraCinematicSequnce();

private:
	void					Select_MapTexture();
	void					Select_MiXTextureIndex();
private:

	CGameInstance*			m_pGameInstance{ nullptr };
	CMapToolManager*		m_pMapToolManager{ nullptr };

private:
	CCamera*				m_pCamera{ nullptr };
	CCameraMan*				m_pCameraMan{ nullptr };
private:
	float					m_fFixRayRange{};
	float					m_fFixMouseWheelSpeed{};

	_int					m_iBuffer{};
	string					m_strBuffer{};
	char					m_szBuffer[MAX_PATH];


	ID3D11ShaderResourceView* m_pDefaultSRV{nullptr};

	_int*					m_pSelectMixTileTextureIndex{nullptr};



	_int					m_iSelectCamCinematicSequenceName{0};


	vector<string>			m_vecTextureSplatingInfoDataName{};
	_int					m_iSelectTextureSplatingInfoData{};

	char					m_szTextureSplatingInfoData_SaveName[MAX_PATH];
	char					m_szCameraCinematicSequence_SaveName[MAX_PATH];


	char					m_szLevelTypeName[ENUM_TO_UINT(EClientLevelType::END)][MAX_PATH];


	/* 카메라 쉐이킹 임시 Debuging 용 */


public:
	static  CPanel_MapTool* Create(const _char* pLabel, CLevel* pOwner, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual void Free() override;
};

NS_END