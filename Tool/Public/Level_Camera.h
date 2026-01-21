#pragma once
#include "Level.h"

NS_BEGIN(Tool)

class CLevel_Camera final : public CLevel
{
	using Super = CLevel;
private:
	explicit CLevel_Camera(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CLevel_Camera() = default;

	virtual HRESULT Initialize() override;
public:
	virtual HRESULT Awake(const _uint iLevelID) override;
	virtual void Update(const _float fTimeDelta) override;
	virtual HRESULT Render() override;
private:
	HRESULT Ready_Player_Layer(const wstring& wstrLayerTag);
	HRESULT Ready_UI_Layer(const wstring& wstrLayerTag);
	HRESULT Ready_Enemy_Beach_Layer(const wstring& wstrLayerTag);
private:
	class CImGui_ToolManager* m_pImGuiManager = { nullptr };
public:
	static CLevel_Camera* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual void Free() override;
};

NS_END
