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
	std::string strTag;
	uint32_t iLayerIndex;
	uint32_t iRectTransformType;
	uint32_t iUIType;
	_float fWidth;
	_float fHeight;
	_float fPosX;
	_float fPosY;
	_float fPosZ;
	_string strTextureTag;
	uint32_t iTextureIndex;
};

struct TUI_LayerData
{
	static constexpr EUIType eType = EUIType::LAYER;
	std::string strTag;
};

struct TUI_CanvasData
{
	static constexpr EUIType eType = EUIType::CANVAS;
	std::string strTag;

	_float fWidth;
	_float fHeight;
	_float fPosX;
	_float fPosY;
	_float fPosZ;
};

/////////////////-------------------  to_json, from_json  -------------------/////////////////
void to_json(json& j, const TUI_GenericUIData& data);
void from_json(const json& j, TUI_GenericUIData& data);
void to_json(json& j, const TUI_LayerData& data);
void from_json(const json& j, TUI_LayerData& data);
void to_json(json& j, const TUI_CanvasData& data);
void from_json(const json& j, TUI_CanvasData& data);
NS_END

/////////////////-------------------  Wrapping Class  -------------------/////////////////

NS_BEGIN(Engine)

class ENGINE_DLL CUI_GenericUI_DTO final : public IObjectDataBase
{
	using Super = IObjectDataBase;
private:
	CUI_GenericUI_DTO() = default;
	virtual ~CUI_GenericUI_DTO() = default;
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
	static CUI_GenericUI_DTO* Create() { return new CUI_GenericUI_DTO(); }
	virtual void Free() override { Super::Free(); }
};

class ENGINE_DLL CUI_Layer_DTO final : public IObjectDataBase
{
	using Super = IObjectDataBase;
private:
	CUI_Layer_DTO() = default;
	virtual ~CUI_Layer_DTO() = default;
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
	static CUI_Layer_DTO* Create() { return new CUI_Layer_DTO(); }
	virtual void Free() override { Super::Free(); }
};

class ENGINE_DLL CUI_Canvas_DTO final : public IObjectDataBase
{
	using Super = IObjectDataBase;
private:
	CUI_Canvas_DTO() = default;
	virtual ~CUI_Canvas_DTO() = default;
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
	static CUI_Canvas_DTO* Create() { return new CUI_Canvas_DTO(); }
	virtual void Free() override { Super::Free(); }
};

NS_END