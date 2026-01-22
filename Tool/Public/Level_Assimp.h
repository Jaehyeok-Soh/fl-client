#pragma once
#include "Level.h"

NS_BEGIN(Tool)

class CLevel_Assimp final : public CLevel
{
	using Super = CLevel;
private:
	explicit CLevel_Assimp(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CLevel_Assimp() = default;

	virtual HRESULT Initialize() override;
public:
	virtual HRESULT Awake(const _uint iLevelID) override;
	virtual void Update(const _float fTimeDelta) override;
	virtual HRESULT Render() override;
private:
	class CImGui_ToolManager* m_pImGuiManager = { nullptr };
public:
	static CLevel_Assimp* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual void Free() override;
};

NS_END