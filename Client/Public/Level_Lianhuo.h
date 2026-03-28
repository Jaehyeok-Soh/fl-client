#pragma once
#include "Level.h"

NS_BEGIN(Engine)
class CSingleSkillSpawner;
class CProjectileSpawner_Fan;
NS_END

NS_BEGIN(Client)

class CLevel_Lianhuo : public CLevel
{
	using Super = CLevel;
private:
	explicit CLevel_Lianhuo(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CLevel_Lianhuo() = default;

	virtual HRESULT Initialize() override;
public:
	virtual HRESULT Awake(const _uint iLevelID) override;
	virtual void	Update(const _float fTimeDelta) override;
	virtual HRESULT Render() override;
private:
	HRESULT Build_Prototype();
	HRESULT Build_Files();
	HRESULT Ready_ShaderSetting();
	HRESULT Ready_Player_Layer(const wstring& wstrLayerTag);
	HRESULT Ready_UI_Layer(const wstring& wstrLayerTag);
	HRESULT Ready_Camera_Layer(const wstring& wstrLayerTag);
	HRESULT Ready_Boss_Layer(const wstring& wstrLayerTag);
	HRESULT Ready_Lights();
	HRESULT Ready_Map();
	HRESULT Ready_Camera_Setting(const _uint iLevelIndex);
	HRESULT Ready_Octree();
	HRESULT Ready_Dissolve();
private:
	ECursorMode m_eCursorMode = ECursorMode::LockedHiddenCenter;
public:
	static CLevel_Lianhuo* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual void				Free() override;
};
NS_END

