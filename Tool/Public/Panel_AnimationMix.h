#pragma once
#include "ImGui_Panel.h"
#include "Animation_Defines.h"

#include "Model.h"

NS_BEGIN(Tool)
class CPanel_AnimationMix final : public CImGui_Panel
{
private:
	using Super = CImGui_Panel;
private:
	explicit CPanel_AnimationMix(const _char* pLabel, CLevel* pOwner, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CPanel_AnimationMix() = default;

private:
	HRESULT	Initialize();

public:
	virtual HRESULT Render(CToolObject* pGo) override;
	virtual void Update(const _float fTimeDelta) override;

private:
	class CAnimTool_Manager* m_pAnimToolManager = { nullptr };
	// imgui
	EAnimEvent::Enum m_eEventTypeCombo = EAnimEvent::NONE;

private:
	//_bool			m_bHasModel = { false };

	_bool			m_bMix	= { true };
	vector<_int>	m_vecMixIdx;

	CModel* pModel			= { nullptr };

	_int m_iMixSize			= { 0 };

	_int m_iMixAnimIdx		= { -1 };
	_int m_iMixVectorIdx	= { -1 };

	vector<CModel::DATA_ANIMIX>	m_vecMixData;


private:
	void Current_Info();

	void Set_AnimationMixInfo();

	void Set_AnimMationMix();
	void Set_MixData();

public:
	static			CPanel_AnimationMix* Create(const _char* pLabel, CLevel* pOwner, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual void	Free() override;
};

NS_END

