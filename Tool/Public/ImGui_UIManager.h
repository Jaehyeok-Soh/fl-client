#pragma once
#include "Base.h"
#include "UIData_Repository.h"

NS_BEGIN(Tool)

class CImGui_UIManager final : public CBase
{
	using Super = CBase;
	DECLARE_SINGLETON(CImGui_UIManager)

private:
	CImGui_UIManager();
	virtual ~CImGui_UIManager() = default;

public:
	void Add_CanvasData(const CANVAS_DATA& tData) { 
		m_vecCanvasData.push_back(tData); 
		m_iCurCanvasIndex = static_cast<uint32_t>(m_vecCanvasData.size() - 1); 
	}
	void Remove_CanvasData(uint32_t index) { 
		m_vecCanvasData.erase(m_vecCanvasData.begin() + index);
		m_iCurCanvasIndex = 0;
	}
	void Remove_CurCanvasData() { 
		m_vecCanvasData.erase(m_vecCanvasData.begin() + m_iCurCanvasIndex); 
		m_iCurCanvasIndex = 0;
	}
	uint32_t Get_CanvasData_Size() { return static_cast<uint32_t>(m_vecCanvasData.size()); }

	const CANVAS_DATA& Get_CanvasData(uint32_t index)const { return m_vecCanvasData[index]; }
	CANVAS_DATA& Get_CanvasDataRef(uint32_t index) { return m_vecCanvasData[index]; }

	const CANVAS_DATA& Get_CurCanvasData()const;
	CANVAS_DATA& Get_CurCanvasDataRef();

	const uint32_t Get_CurCanvasIndex()const { return m_iCurCanvasIndex; }
	void Set_CurCanvasIndex(uint32_t i) { m_iCurCanvasIndex = i; }

	_bool HasCanvasTag(const _string& tag);
	uint32_t CanvasTagToIndex(const _string& tag);

private:
	CANVAS_DATA m_EmptyData = {};
	vector<CANVAS_DATA> m_vecCanvasData;
	uint32_t m_iCurCanvasIndex = {};

public:
	virtual void Free()override;
};

NS_END