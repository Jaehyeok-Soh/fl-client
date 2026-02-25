#pragma once
#include "BuilderBase.h"
#include "DataDocument_UI.h"
#include "GenericUI.h"

NS_BEGIN(Engine)
class CDataDocumentBase;
NS_END

NS_BEGIN(Client)
class CCanvas;
class CUITrigger;
class CBuilder_UI final : public CBuilderBase
{
	using Super = CBuilderBase;
private:
	CBuilder_UI(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, _uint iLevelID);
	virtual ~CBuilder_UI() = default;

public:
	HRESULT Initialize();
	virtual HRESULT Build(const CDataDocumentBase& document) override;
private:
	HRESULT Create_CanvasDTO(const DTO::TUI_CanvasData& data);
	HRESULT Create_GenericUIDTO(const DTO::TUI_GenericUIData& data);

	// Data Create
	HRESULT Create_TextDTO(const DTO::TUI_TextData& data);
	HRESULT Create_TriggerDTO(const DTO::TUI_TriggerData& data);
	HRESULT Create_DImageDTO(const DTO::TUI_DImageData& data);

	HRESULT Register_Class(DTO::EUIClassType eClassType, const DTO::TUI_GenericUIData& data, CCanvas* pCanvas);
	CGenericUI::GENERIC_UI_DESC Make_DefaultInfo(const DTO::TUI_GenericUIData& data, CCanvas* pCanvas);

private:
	unordered_map<_string, CCanvas*> m_MapCanvasCache;
	unordered_map<_string, CGenericUI* >m_pMapUICache;

	// Data Cache 
	unordered_map<_string, DTO::TUI_TextData> m_MapTextDataCache;
	unordered_map<_string, DTO::TUI_TriggerData> m_MapTriggerDataCache;
	unordered_map<_string, DTO::TUI_DImageData> m_MapDImageDataCache;

	vector<CUITrigger*> m_vecTriggerUIs;
		 
	Vec2 m_vAspect = {};
	Vec2 m_vViewportSIze = {};

	Client::EUIPrefabType m_ePrefabtype = { Client::EUIPrefabType::NOT_PREFAB };

public:
	static CBuilder_UI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, _uint iLevelID);
	virtual void Free() override;
};

NS_END