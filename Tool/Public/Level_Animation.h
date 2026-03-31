#pragma once
#include "Level.h"
#include "Animation_Defines.h"

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
			LOAD_PART,
			LOAD_OVERLAP_SCRIPT,
			LOAD_EFFECT_SCRIPT,

			ChangeSelectedObject,
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
			DESCRIPTION,
			MODELINFO,
			PARTSINFO,
			STATEEDITOR,
			MIXER,
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
	HRESULT Ready_Sounds();
private:
	void	Update_Elements(const _float fTimeDelta);
	void	Render_Elements();

	void	Render_Grid();

private:
	void Load_AnimModel(fs::path animModelPath, ANIM_SRT pretransform);
	void Create_AnimModel(fs::path animModelPath, ANIM_SRT pretransform);
	wstring Create_AnimModelPrototype(fs::path animModelPath, ANIM_SRT pretransform);
	void Load_PartObject(fs::path animPartModelPath, ANIM_SRT pretransform, _int iSocketBondIdx, _bool bCombine, _bool bStatic, _int iRootBoneIdx);
	void SetAnimationInfo(fs::path animModelPath);
	void On_ChangeSelectedObject(CGameObject* pGo);

private:
	class CImGui_ToolManager*	m_pImGuiManager		= { nullptr };
	class CPicking_ToolManager* m_pPickingManager	= { nullptr };
	class CAnimTool_Manager*	m_pAnimToolManager = { nullptr };
	class CToolObject*			m_pSelectedObject	= { nullptr };
	std::array<DelegateHandle, Event::END> m_EventHandles;

private:
	HRESULT Ready_PlayerSound();
	HRESULT Ready_XibiSound();

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
