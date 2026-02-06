#pragma once
#include "ToolObject.h"


NS_BEGIN(Engine)
class CMaterial;
NS_END

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
		Default,
		Select,
		Preview,
		END,
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
	HRESULT					Ready_OverrideMtl(const USING_MODEL_INFO& tUsingModelInfo);

public:
	HRESULT					Add_MapToolComponent(CMapObject::COMPONENT eType);
public:
	void					Set_MapObjectState(CMapObject::EState eState) { m_eMapObjectState = eState; }
	void					Set_MapObjectType(EMapObject_Type eType) { m_eMapObjectType = eType; }
public:
	virtual void			Reset_OriginTransform(_uint iIndex);
	void					Override_OriginTransform(_uint iIndex);
public:
	SimpleMath::Matrix		Get_OriginTransform(_uint iIndex);
public:
	EMapObject_Type			Get_MapObjectType() const { return m_eMapObjectType;}
	CMapObject::EState		Get_MapObjectState()  const { return m_eMapObjectState; }
	bool					Get_IsUseOverrideMtl() const { return m_iUseOverrideMaterials; }
	vector<CMaterial*>		Get_OverrideMtls()		const { return m_vecOverrideMaterials; }
	vector<wstring>			Get_OverrideMtlsName() const;
	vector<wstring>			Get_TotalUseMtlsName();
	virtual vector<SRT_DATA>		Get_SRTDatas()  { return vector<SRT_DATA>();}
	virtual USING_MODEL_INFO		Get_UsingModelInfo() const { return USING_MODEL_INFO();}
public:
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
	bool					m_iUseOverrideMaterials{false};
	vector<CMaterial*>		m_vecOverrideMaterials;
	bool					m_isLoaded{false};
	vector<SRT_DATA>		m_vecOriginSRTs{};

	void*					m_pDesc{nullptr};

	array< CMonoBehaviour*, ENUM_TO_UINT(CMapObject::COMPONENT::END)> m_arrayMapToolComponent{};

public:
	virtual void			Free()									override;
};

NS_END

