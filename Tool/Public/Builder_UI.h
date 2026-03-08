#pragma once
#include "BuilderBase.h"
#include "DataDocument_UI.h"

NS_BEGIN(Engine)
class CDataDocumentBase;
NS_END

NS_BEGIN(Tool)
class CToolCanvas;
class CToolUI;

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
	HRESULT Create_TextDTO(const DTO::TUI_TextData& data);
	HRESULT Create_DImageDTO(const DTO::TUI_DImageData& data);
	HRESULT Create_GenericUIDTO(const DTO::TUI_GenericUIData& data);
private:
	map<_string, CToolCanvas* > m_pCanvasCache;
	map<_string, CToolUI* > m_pUICache;

	// Data Cache
	map<_string, DTO::TUI_TextData> m_TextDataCache;
	map<_string, DTO::TUI_DImageData> m_DImageDataCache;
	int32_t m_iIndex = {};

public:
	static CBuilder_UI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, _uint iLevelID);
	virtual void Free() override;
};

NS_END