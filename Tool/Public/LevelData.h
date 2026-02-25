#pragma once
#include "ToolObject.h"



NS_BEGIN(Tool)

class CMapToolManager;
class CPanel_MapTool;


class CLevelData : public CToolObject
{
	using Super = CGameObject;
public:
	friend CPanel_MapTool;
	friend CMapToolManager;
public:
	typedef struct tagLevelData_Desc
	{
		std::string strTextureSplatingInfoName{"None"};
		std::string strLevelTypeName{"STATIC"};
	}LevelData_DESC;
protected:
	CLevelData(EToolObjectType eType, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CLevelData(const CLevelData& rhs);
	virtual ~CLevelData() = default;

	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
public:
	virtual HRESULT Awake(const _uint iCurrentLevelID) override;
	virtual void	Update_Priority(const _float fTimeDelta) override;
	virtual void	Update(const _float fTimeDelta) override;
	virtual void	Update_Late(const _float fTimeDelta) override;
	virtual void	Ready_Before_Render(const _float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual _bool	Picking(OUT Vec3& vOut) override;
	virtual void	Draw_ImGui()override;

public:
	virtual _bool	Export_Data(DTO::ECategory eCategory, CDataDocumentBase* pDocument)override;
private:
	/* 클라이언테이서 오브젝트들이 추가될 Level Path 값들을 한번에 관리하기 위한 Path값 */
	EClientLevelType	m_eClientLevelType{EClientLevelType::STATIC};
	std::string			m_strTextureSplatingInfoName{"None"};
public:
	static	CLevelData*		Create(EToolObjectType eType, ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg)override;
	virtual void			Free() override;
};


NS_END
