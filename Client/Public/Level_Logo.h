#pragma once
#include "Client_Defines.h"
#include "level.h"

NS_BEGIN(Client)

class CLevel_Logo final : public CLevel
{
	using Super = CLevel;
private:
	explicit CLevel_Logo(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CLevel_Logo() = default;

	virtual HRESULT Initialize() override;
public:
	virtual HRESULT Awake(const _uint iLevelID) override;
	virtual void Update(const _float fTimeDelta) override;
	virtual HRESULT Render() override;
private:
	HRESULT Build_Prototype();
	HRESULT Build_Files();
	HRESULT Ready_UI_Layer(const wstring& wstrLayerTag);
	HRESULT Ready_Camera_Layer(const wstring& wstrLayerTag);
	HRESULT Ready_Camera_Setting(const _uint iLevelIndex);
	HRESULT Ready_Lights();
private:
	ECursorMode m_eCursorMode = ECursorMode::LockedHiddenCenter;
public:
	static CLevel_Logo* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual void Free() override;
};

NS_END