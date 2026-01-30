#pragma once
#include "ObjectDataBase.h"

NS_BEGIN(DTO)

/////////////////-------------------  UI  -------------------/////////////////
enum class EUIType : _uint
{
	CANVAS,
	LAYER,
	GENERICUI,
	END
};
inline constexpr _uint g_UITypeCount{ ENUM_TO_UINT(EUIType::END) };

/////////////////-------------------  Data Struct  -------------------/////////////////
struct TUI_GenericUIData
{
	static constexpr EUIType eType = EUIType::GENERICUI;
	std::string strTag{ "GenericUI" };
};

struct TUI_LayerData
{
	static constexpr EUIType eType = EUIType::LAYER;
	std::string strTag{ "Layer" };
};

struct TUI_CanvasData
{
	static constexpr EUIType eType = EUIType::CANVAS;
	std::string strTag{ "Canvas" };
};

/////////////////-------------------  to_json, from_json  -------------------/////////////////
inline void to_json(json& j, const TUI_GenericUIData& data);
inline void from_json(const json& j, TUI_GenericUIData& data);

inline void to_json(json& j, const TUI_LayerData& data);
inline void from_json(const json& j, TUI_LayerData& data);

inline void to_json(json& j, const TUI_CanvasData& data);
inline void from_json(const json& j, TUI_CanvasData& data);
NS_END

/////////////////-------------------  Wrapping Class  -------------------/////////////////

NS_BEGIN(Engine)

class ENGINE_DLL CUI_GenericUI final : public IObjectDataBase
{
	using Super = IObjectDataBase;
private:
	CUI_GenericUI() = default;
	virtual ~CUI_GenericUI() = default;
public:
	_uint Get_Type() const override { return ENUM_TO_UINT(DTO::EUIType::GENERICUI); }
	const _string& Get_Tag() const override { return m_Data.strTag; }

	json ToJson() const override;
	HRESULT FromJson(const json& j) override;

	const DTO::TUI_GenericUIData& Get_Data() const { return m_Data; }
	DTO::TUI_GenericUIData& Get_Data() { return m_Data; }
private:
	DTO::TUI_GenericUIData m_Data;
public:
	static CUI_GenericUI* Create() { return new CUI_GenericUI(); }
	virtual void Free() override { Super::Free(); }
};

class ENGINE_DLL CUI_Layer final : public IObjectDataBase
{
	using Super = IObjectDataBase;
private:
	CUI_Layer() = default;
	virtual ~CUI_Layer() = default;
public:
	_uint Get_Type() const override { return ENUM_TO_UINT(DTO::EUIType::LAYER); }
	const std::string& Get_Tag() const override { return m_Data.strTag; }

	json ToJson() const override;
	HRESULT FromJson(const json& j) override;

	const DTO::TUI_LayerData& Get_Data() const { return m_Data; }
	DTO::TUI_LayerData& Get_Data() { return m_Data; }
private:
	DTO::TUI_LayerData m_Data;
public:
	static CUI_Layer* Create() { return new CUI_Layer(); }
	virtual void Free() override { Super::Free(); }
};

class ENGINE_DLL CUI_Canvas final : public IObjectDataBase
{
	using Super = IObjectDataBase;
private:
	CUI_Canvas() = default;
	virtual ~CUI_Canvas() = default;
public:
	_uint Get_Type() const override { return ENUM_TO_UINT(DTO::EUIType::CANVAS); }
	const std::string& Get_Tag() const override { return m_Data.strTag; }

	json ToJson() const override;
	HRESULT FromJson(const json& j) override;

	const DTO::TUI_CanvasData& Get_Data() const { return m_Data; }
	DTO::TUI_CanvasData& Get_Data() { return m_Data; }
private:
	DTO::TUI_CanvasData m_Data;
public:
	static CUI_Canvas* Create() { return new CUI_Canvas(); }
	virtual void Free() override { Super::Free(); }
};

NS_END