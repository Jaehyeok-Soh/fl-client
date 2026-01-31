#pragma once
#include "Level.h"

NS_BEGIN(Tool)

class CLevel_Logo final : public CLevel
{
	using Super = CLevel;
private:
	explicit CLevel_Logo(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CLevel_Logo() = default;

	HRESULT Initialize();
public:
	virtual HRESULT Awake(const _uint iLevelID) override;
	virtual void Update(const _float fTimeDelta) override;
	virtual HRESULT Render() override;
private:
	class CImGui_ToolManager* m_pImGuiManager = { nullptr };
public:
	static CLevel_Logo* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual void Free() override;
};

NS_END