#pragma once
#include "BuilderBase.h"
#include "DataDocument_UI.h"

NS_BEGIN(Engine)
class CDataDocumentBase;
NS_END

NS_BEGIN(Client)
class CCanvas;
class CGenericUI;
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

private:
	unordered_map<_string, CCanvas*> m_MapCanvasCache;
	unordered_map<_string, CGenericUI* >m_pMapUICache;

	Vec2 m_vAspect = {};
	Vec2 m_vViewportSIze = {};

public:
	static CBuilder_UI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, _uint iLevelID);
	virtual void Free() override;
};

NS_END