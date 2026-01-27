#pragma once
#include "ImGui_Panel.h"

NS_BEGIN(Tool)

class CUEMapdataParser;

class CPanel_MapDataController : public CImGui_Panel
{
	using Super = CImGui_Panel;
protected:
	explicit CPanel_MapDataController(const _char* pLabel, CLevel* pOwner, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CPanel_MapDataController() = default;
public:
	virtual HRESULT Render(CToolObject* pGo)override;
	virtual void Update(const _float fTimeDelta)override;

private:
	HRESULT Render_ConvertedList();
	HRESULT Render_Converted_UnrealRawMapData_Button();
	HRESULT Render_Filtering_UnrealRawMapData_Button();
private:
	const wstring m_wstrMapDatPath{L"../../Resources/Data/MapData/"};
public:
	static  CPanel_MapDataController* Create(const _char* pLabel, CLevel* pOwner, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual void Free() override;
private:
	vector<wstring>	  m_vecConvertedUEMapDataPath{};
	_uint			  m_iSelectConvertedUEMapDataPath{};
	CUEMapdataParser* m_pUEMapdataParser{nullptr};
};

NS_END

