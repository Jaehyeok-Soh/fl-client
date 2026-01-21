#pragma once
#include "Tool_Defines.h"
#include "Level.h"

NS_BEGIN(Tool)

class CLevel_Loading final : public CLevel
{
	using Super = CLevel;
private:
	explicit CLevel_Loading(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CLevel_Loading() = default;

	HRESULT Initialize(LEVELID eNextLevelID);
public:
	virtual HRESULT Awake(const _uint iLevelID) override;
	virtual void Update(const _float fTimeDelta) override;
	virtual HRESULT Render() override;
private:
	class CImGui_ToolManager* m_pImGuiManager = { nullptr };
	class CLoader* m_pLoader = { nullptr };
	LEVELID m_eNextLevelID = { LEVELID::END };
public:
	static CLevel_Loading* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, LEVELID eNextLevelID);
	virtual void Free() override;
};

NS_END