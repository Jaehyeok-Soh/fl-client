#pragma once
#include "ToolObject.h"


NS_BEGIN(Tool)

class CMapObject abstract : public CToolObject
{
	using Super = CToolObject;
public:
	enum class COMPONENT
	{
		END
	};
public:

	enum class EState
	{
		Preview,
		Default,
		Select,
		END,
	};

	enum EReset_Type
	{
		NONE = 0,
		S = 1 << 0,
		R = 1 << 1,
		T = 1 << 2,
		END 
	};

	typedef struct tagMapObjectDesc : public CToolObject::TOOLOBJECT_DESC
	{
		bool				isLoaded{ false };

		EMapObject_Type		eType{EMapObject_Type::END};
		CMapObject::EState	eState{CMapObject::EState::Default};
	}MAPOBJECT_DESC;

protected:

	CMapObject(EToolObjectType eType, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CMapObject(const CMapObject& rhs);
	virtual ~CMapObject() = default;

public:

	virtual HRESULT			Initialize_Prototype()							override;
	virtual HRESULT			Initialize(void* pArg)							override;
	HRESULT					Ready_Component();
public:

	void					Reset_SRT(Engine::Flags fResetTypeFlag);
	virtual void			Register_OriginSRT(Engine::Flags fResetTypeFlag);
public:
	HRESULT					Add_MapToolComponent(CMapObject::COMPONENT eType);
public:
	void					Set_MapObjectState(CMapObject::EState eState) { m_eMapObjectState = eState; }
public:

	Vec3					Get_OriginScale();
	Vec3					Get_OriginDegree();
	Vec3					Get_OriginPosition();
	CMapObject::EState		Get_MapObjectState()  const { return m_eMapObjectState; }

public:

	virtual HRESULT			Awake(const _uint iCurrentLevelID)				override;
	virtual void			Update_Priority(const _float fTimeDelta)		override;
	virtual void			Update(const _float fTimeDelta)					override;
	virtual void			Update_Late(const _float fTimeelta)			override;
	virtual void			Ready_Before_Render(const _float fTimeDelta)	override;
	virtual HRESULT			Render()										override;
	virtual void			Draw_ImGui()override;

protected:
	EMapObject_Type			m_eMapObjectType{ EMapObject_Type::END };
	EState					m_eMapObjectState{ EState::Default};

	bool					m_isLoaded{false};
	bool					m_isRegisterSRT{false};
	void*					m_pDesc{nullptr};


	Vec3					m_vOriginScale{};
	Vec3					m_vOriginDegree{};
	Vec3					m_vOriginPosition{};

	Vec3					m_vImGuiPitchYawRoll{};
	Quat					m_vImGuiQuat{};

	array< CMonoBehaviour*, ENUM_TO_UINT(CMapObject::COMPONENT::END)> m_arrayMapToolComponent{};

public:
	virtual void			Free()									override;
};

NS_END

