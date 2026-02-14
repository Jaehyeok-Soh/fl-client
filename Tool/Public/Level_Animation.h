#pragma once
#include "Level.h"

namespace fs = std::filesystem;

NS_BEGIN(Tool)

class CLevel_Animation final : public CLevel
{
public:
	struct Event
	{
		enum Enum
		{
			LOAD,
			LOAD_OVERLAP_SCRIPT,
			END
		};
	};

	struct Elements
	{
		enum Enum
		{
			FILE,
			LOAD,
			MODEL,
			ANIMATION,
			PARTS,
			END
		};
	};

private:
	using Super = CLevel;

private:
	explicit CLevel_Animation(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CLevel_Animation() = default;

	virtual HRESULT Initialize() override;
public:
	virtual HRESULT Awake(const _uint iLevelID) override;
	virtual void Update(const _float fTimeDelta) override;
	virtual void Update_Picking() override;
	virtual HRESULT Render() override;

	/* Ready Funcs*/
private:
	HRESULT Ready_Camera(const _wstring wstrLayerTag);
	HRESULT	Ready_Camera_Setting(const _uint iLevelID);
	HRESULT Ready_Lights();
	HRESULT Ready_Panels();
	HRESULT Ready_Event();
	HRESULT Release_Event();

private:
	void	Update_Elements(const _float fTimeDelta);
	void	Render_Elements();

	void	Render_Grid();

private:
	void Load_AnimModel(fs::path animModelPath);
	void Create_AnimModel(fs::path animModelPath);
	wstring Create_AnimModelPrototype(fs::path animModelPath);
	void SetAnimationInfo();

private:
	class CImGui_ToolManager*	m_pImGuiManager		= { nullptr };
	class CPicking_ToolManager* m_pPickingManager	= { nullptr };
	class CAnimTool_Manager*	m_pAnimToolManager = { nullptr };
	class CToolObject*			m_pSelectedObject	= { nullptr };
	std::array<DelegateHandle, Event::END> m_EventHandles;


private:
	wstring m_wstrLayer = { L"Animation_Model_Layer" };
	array<class CImGui_Panel*, Elements::END> m_GuiElements = { nullptr };

#ifdef _DEBUG
private:
	PrimitiveBatch<DirectX::VertexPositionColor>* m_pBatch = { nullptr };
	BasicEffect* m_pEffect = { nullptr };
	ID3D11InputLayout* m_pInputLayout = { nullptr };
	ID3D11DepthStencilState* m_pDSS = { nullptr };
#endif

public:
	static CLevel_Animation* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual void Free() override;
};

NS_END
