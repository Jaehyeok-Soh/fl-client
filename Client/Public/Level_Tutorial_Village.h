#pragma once
#include "Level.h"

NS_BEGIN(Client)

class CLevel_Tutorial_Village : public CLevel
{
	using Super = CLevel;
private:
	explicit CLevel_Tutorial_Village(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CLevel_Tutorial_Village() = default;

	virtual HRESULT Initialize() override;
public:
	virtual HRESULT Awake(const _uint iLevelID) override;
	virtual void	Update(const _float fTimeDelta) override;
	virtual HRESULT Render() override;
private:
	HRESULT Build_Prototype();
	HRESULT Build_Files();
	HRESULT Ready_Player_SkillObjPool();
	HRESULT Ready_Player_Layer(const wstring& wstrLayerTag);
	HRESULT Ready_UI_Layer(const wstring& wstrLayerTag);
	HRESULT Ready_Camera_Layer(const wstring& wstrLayerTag);
	HRESULT Ready_Lights();
	HRESULT Ready_Map();
	HRESULT	Ready_SkyBox();
private:
	HRESULT Ready_Camera_Setting(const _uint iLevelIndex);
	HRESULT Ready_Octree();
	void Ready_ShaderSetting();
private:
	ECursorMode m_eCursorMode = ECursorMode::LockedHiddenCenter;

	_bool m_isDebugging = { false };
	_bool m_isDebugging2 = { false };
	_float m_fTimeScale = {};

public:
	static CLevel_Tutorial_Village*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual void					Free() override;
};

NS_END
