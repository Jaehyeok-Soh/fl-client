#pragma once
#include "ToolObject.h"


NS_BEGIN(Engine)
class CMaterial;
struct  CLIENT_MAKEPATH_DESC_BASE;
NS_END

NS_BEGIN(Tool)

class CMapObject final : public CToolObject
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
		/* UE 에서 추출되었거나 Or 로드한 데이터 인지 아닌지 */
		bool					isUELoaded	{false};
		bool					isLoaded	{ false };

		/* Client에서 생성할 LevelType */
		EClientLevelType					eClientLevelType{ EClientLevelType::LOGO };

		/* Client에서 생성될 Class 연결 및 같이 넘겨줄 Desc */
		EClientMakePath						eClientMakePath{ EClientMakePath::StaticObject };
		vector<CLIENT_MAKEPATH_DESC_BASE*>	vecClientMakePathDesc{};
		vector<Tool::SRT_DATA>				vecSRTs{};

		/* 사용할 Draw 방법과 모델관련 */
		EMapObject_DrawType					eMapObjectDrawType{ EMapObject_DrawType::Default };


		Tool::tagUsingModelInfo				tUsingModelInfo{};
		

		/* 현재 MapObject의 상태를 나타내준다 EState 등등 */
		CMapObject::EState					eState{CMapObject::EState::Default};
	}MAPOBJECT_DESC;

protected:

	CMapObject(EToolObjectType eType, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CMapObject(const CMapObject& rhs);
	virtual ~CMapObject() = default;

private:
	virtual HRESULT					Initialize_Prototype()							override;
	virtual HRESULT					Initialize(void* pArg)							override;
	HRESULT							Ready_SRTDatas(CMapObject::MAPOBJECT_DESC* pDesc);
	HRESULT							Ready_Component();
	HRESULT							Ready_ClientMakePath(CMapObject::MAPOBJECT_DESC* pDesc);
	HRESULT							Ready_OverrideMtl(const USING_MODEL_INFO& tUsingModelInfo);
	_bool							Check_OutBound(_int iIndex) const;
private:
	HRESULT							Change_Instance_To_Default();
public:

	HRESULT							Add_MapToolComponent(CMapObject::COMPONENT eType);
public:

	void							Reset_OriginTransform(_int iIndex = -1);
	void							Override_OriginTransform(_int iIndex = -1);

	/* 기능 관련 */
public:

	void								Update_InstanceWorldMatrix(_bool isAllUpdate , _int iIndex = -1);

public:
	virtual bool						Get_SRT(OUT  Vec3& vOutScale, OUT Quat& vQuat, OUT Vec3& vPosition)override;
	virtual Matrix						Get_WorldMatrix()override;
	virtual void						Set_WorldMatrix(const Vec3& vScale, const Quat& vQuat, const Vec3& vPosition) override;
	virtual void						Set_WorldMatrix(const Matrix& WorldMatrix)override;

	/* MapObject Type 관련 */
	void								Set_MapObjectState(CMapObject::EState eState)	{ m_eMapObjectState = eState; }
	void								Set_MapObjectType(EMapObject_Type eType)		{ m_eMapObjectType = eType; }
	void								Set_ClientLeveType(EClientLevelType eType)		{ m_eClientLevelType = eType; }
	void								Set_ClientMakePath(EClientMakePath eClientMakePath);
	void								Set_MapObjectDrawType(EMapObject_DrawType eDrawType);

	/* InstanceBuffer 관련 */
	void								Set_SelectedInstanceID(_int iID)				{ if (m_eMapObjectDrawType != EMapObject_DrawType::Instance) return; m_iSelectedInstanceID = iID; }

	/* SRT Data 관련 */
	void								Add_InstanceData( const  SRT_DATA& tData  );
	void								Delete_InstanceData(_int iIndex = -1);

	void								Set_SRTDatas(const vector<SRT_DATA>& vecSRTDatas) { m_vecSRTs = m_vecSRTs; }
	void								Set_SRTData(const Vec3& vScale , const Quat vQuat , const Vec3 vPosition  , _int iIndex = -1 );

	void								Set_Scale(const Vec3& vScale , _int iIndex = -1);
	void								Set_Position(const Vec3& vPosition , _int iIndex = -1);
	void								Set_Quaternion(const Quat& vQuat , _int iIndex = -1);

	void								Set_IsUseOverrideMaterial(_bool isUse) { m_isUseOverrideMaterials = isUse; }


public:

	_int								Get_InstanceCount();
	/* Load 관련 */
	_bool								Get_IsLoaded()				const { return m_isLoaded; }
	_bool								Get_IsUELoaded()			const { return m_isUELoaded; }

	/* SRT Data 관련 */
	const vector< Tool::SRT_DATA >&		Get_SRTDatas(bool isOrigin) const { return isOrigin == true ? m_vecOriginSRTs : m_vecSRTs; }
	const SRT_DATA&						Get_SRTData(bool isOrigin, _int iIndex = -1) const;
	
	Vec3								Get_Scale(_int iIndex = -1 )		const;
	Quat								Get_Quaternion(_int iIndex = -1 )	const;
	Vec3								Get_Position(_int iIndex = -1 )	const;

	/* Type관련 */
	EMapObject_Type						Get_MapObjectType()			const	{ return m_eMapObjectType;}
	CMapObject::EState					Get_MapObjectState()		const	{ return m_eMapObjectState; }
	EClientMakePath						Get_ClientMakePath()		const	{ return m_eClientMakePath; }
	EClientLevelType					Get_ClientLevelType()		const	{ return m_eClientLevelType; }
	EMapObject_DrawType					Get_MapObjectDrawType()		const	{ return m_eMapObjectDrawType; }

	wstring								Get_ModelPath()				const	{ return m_wstrModelPath; }

	/* Override Mtl 관련 */
	bool								Get_IsUseOverrideMtl()		const	{ return m_isUseOverrideMaterials; }
	vector<CMaterial*>					Get_OverrideMtls()			const	{ return m_vecOverrideMaterials; }
	vector<wstring>						Get_OverrideMtlsName()		const;
	vector<wstring>						Get_TotalUseMtlsName();
	const vector<Tool::SRT_DATA>&		Get_SRTDatas()						{ return m_vecSRTs;}

	_int								Get_SelectedInstanceID()	const	 { return m_iSelectedInstanceID; }
	vector<CLIENT_MAKEPATH_DESC_BASE*>	Get_ClientMakePathDescs()			 { return m_vecClientMakePathDesc; }
	CLIENT_MAKEPATH_DESC_BASE*			Get_ClientMakePathDesc(_int iIndex = -1);
public:

	virtual HRESULT						Awake(const _uint iCurrentLevelID)				override;
	virtual void						Update_Priority(const _float fTimeDelta)		override;
	virtual void						Update(const _float fTimeDelta)					override;
	virtual void						Update_Late(const _float fTimeelta)				override;
	virtual void						Ready_Before_Render(const _float fTimeDelta)	override;
	virtual HRESULT						Render()										override;
	virtual void						Draw_ImGui()									override;		

public:
	_bool								IntsersectWithPlane(OUT Vec3& vOut);
	_bool								Picking(OUT Vec3& vOut);
	virtual _bool						Export_Data(DTO::ECategory eCategory, CDataDocumentBase* pDocument)override;
private:

	HRESULT								Render_Default();
	HRESULT								Render_Instance();

protected:
	bool							m_isBatced{false};


	EMapObject_Type					m_eMapObjectType	{ EMapObject_Type::END };
	EMapObject_DrawType				m_eMapObjectDrawType{ EMapObject_DrawType::Default };
	std::wstring					m_wstrModelPath		{ L"" };

	/* 이 오브젝트가 Client 에서 생성될 Path ID */
	EClientMakePath					m_eClientMakePath	{ EClientMakePath::END };
	EClientLevelType				m_eClientLevelType	{ EClientLevelType::END };
	/* 생성될 Path ID의 Desc를 가지고 있는다 */
	vector<CLIENT_MAKEPATH_DESC_BASE*> m_vecClientMakePathDesc{};


	/* UE or Data Load 인지 판별 */
	bool							m_isLoaded{ false };
	bool							m_isUELoaded{ false };

	EState							m_eMapObjectState{ EState::Default};

	/* Instance 전용 Select ID */
	_int							m_iSelectedInstanceID{0};

	/* SRT Data */
	vector<Tool::SRT_DATA>			m_vecOriginSRTs{};	//
	vector<Tool::SRT_DATA>			m_vecSRTs{};


	/* 임시 패기처분 */
	/* Override Material을 담아줄 변수 */
	vector<CMaterial*>				m_vecOverrideMaterials;
	bool							m_isUseOverrideMaterials{ false };

public:
	static CMapObject*		Create(EToolObjectType eType, ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

	static CMapObject*		Clone(CMapObject* pPrototype  , const SRT_DATA& tSRT );
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free()									override;
};

NS_END

