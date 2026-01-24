#pragma once
#include "Level.h"

NS_BEGIN(Tool)

class CImGui_Panel;


class CLevel_Assimp final : public CLevel
{
	using Super = CLevel;
public:
	enum class Elements
	{
		ModelConverter,
		END,
	};
private:
	explicit CLevel_Assimp(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CLevel_Assimp() = default;

	virtual HRESULT Initialize() override;

	HRESULT	Ready_GUI();

public:
	virtual HRESULT Awake(const _uint iLevelID) override;
	virtual void Update(const _float fTimeDelta) override;
	virtual HRESULT Render() override;
private:
	class CImGui_ToolManager* m_pImGuiManager = { nullptr };
	array< CImGui_Panel*, ENUM_TO_SZET(CLevel_Assimp::Elements::END)> m_arrayImGuiPanel;
public:
	static CLevel_Assimp* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual void Free() override;
};

NS_END