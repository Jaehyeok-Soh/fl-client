#pragma once
#include "Level.h"

NS_BEGIN(Client)

class CLevel_Loading final : public CLevel
{
	using Super = CLevel;
private:
	explicit CLevel_Loading(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CLevel_Loading() = default;

	HRESULT Initialize(ELevelType eNextLevelID);
public:
	virtual HRESULT Awake(const _uint iLevelID) override;
	virtual void Update(const _float fTimeDelta) override;
	virtual HRESULT Render() override;
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