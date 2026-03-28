#pragma once
#include "Level.h"

NS_BEGIN(Client)

class CLevel_Tutorial_Boss : public CLevel
{
	using Super = CLevel;
private:
	explicit CLevel_Tutorial_Boss(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CLevel_Tutorial_Boss() = default;

	virtual HRESULT Initialize() override;
public:
	virtual HRESULT Awake(const _uint iLevelID) override;
	virtual void	Update(const _float fTimeDelta) override;
	virtual HRESULT Render() override;
private:
	/* Logo Scene에서 Player를 만들예정 */
	HRESULT Build_Prototype();
	HRESULT Build_Files();
	HRESULT Ready_Player_SkillObjPool();
	HRESULT Ready_Player_Layer(const wstring& wstrLayerTag);
	HRESULT Ready_UI_Layer(const wstring& wstrLayerTag);
	HRESULT Ready_Camera_Layer(const wstring& wstrLayerTag);
	HRESULT Ready_Lights();
	HRESULT Ready_SkillObjectLayer();
	HRESULT Ready_Map();
	HRESULT Ready_Monster();
	HRESULT Ready_Camera_Setting(const _uint iLevelIndex);
	HRESULT Ready_Octree();
	HRESULT Ready_Dissolve();
private:
	ECursorMode m_eCursorMode = ECursorMode::LockedHiddenCenter;
public:
	static CLevel_Tutorial_Boss*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual void						Free() override;
};

NS_END
