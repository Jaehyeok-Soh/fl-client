#pragma once
#include "Level_Client.h"

NS_BEGIN(Client)

class CLevel_Loading final : public CLevel_Client
{
	using Super = CLevel_Client;
private:
	explicit CLevel_Loading(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CLevel_Loading() = default;

	HRESULT Initialize(ELevelType eNextLevelID);
public:
	virtual HRESULT Awake(const _uint iLevelID) override;
	virtual void Update(const _float fTimeDelta) override;
	virtual HRESULT Render() override;
private:
	HRESULT Ready_UI_Layer(const wstring& wstrLayerTag);

	HRESULT Ready_Camera_Layer(const wstring& wstrLayerTag);

private:
	HRESULT Build_Prototype();
	HRESULT Build_Files();
private:
	class CLoader* m_pLoader = { nullptr };
	ELevelType m_eNextLevelID = { ELevelType::END };
public:
	static CLevel_Loading* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, ELevelType eNextLevelID);
	virtual void Free() override;
};

NS_END