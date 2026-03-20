#pragma once
#include "ToolObject.h"


NS_BEGIN(Engine)
class CMaterial;
struct  CLIENT_MAKEPATH_DESC_BASE;
class CModel;
class CShader;
NS_END

NS_BEGIN(Tool)
class CMapToolManager;
class CEffect_Env;

class CMapObject final : public CToolObject
{
	using Super = CToolObject;
public:
	enum class COMPONENT
	{
		END
	};
public:

	enum class EState : _uint
	{
		Default,
		Select,
		Preview,
		Multi_Select,
		END,
	};

	typedef struct tagMapObjectDesc : public CToolObject::TOOLOBJECT_DESC
	{
		/* UE 에서 추출되었거나 Or 로드한 데이터 인지 아닌지 */
		bool								isUELoaded	{false};
		wstring								wstrUERawDataPath{L""};
		bool								isLoaded	{ false };

		/* 다불러와놓고 판단을 해야할거같은데..  */
		_uint								iSectionNumber{0};

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
	virtual HRESULT						Initialize_Prototype()							override;
	virtual HRESULT						Initialize(void* pArg)							override;
	HRESULT								Ready_SRTDatas(CMapObject::MAPOBJECT_DESC* pDesc);
	HRESULT								Ready_Component();
	HRESULT								Ready_ColliderType();
	HRESULT								Ready_ClientMakePath(CMapObject::MAPOBJECT_DESC* pDesc);
	HRESULT								Ready_OverrideMtl(const USING_MODEL_INFO& tUsingModelInfo);

public:
	HRESULT								Ready_PlusData_ByClientMakePath();

public:
	HRESULT								Ready_Plants();
	HRESULT								Ready_Batch_Player();
	HRESULT								Ready_Batch_Monster();
	HRESULT								Ready_Batch_Object();
	HRESULT								Ready_Batch_InteractiveObject();
	HRESULT								Ready_TriggerBox_MonsterSpawner();

	HRESULT								Ready_Water();
	HRESULT								Ready_Env();


	HRESULT								Ready_InvisibleWall();

private:
	HRESULT								Ready_ColliderTypeName();
	_bool								Check_OutBound(_int iIndex) const;
private:
	void								Check_ClientMakePathAndDrawType_TriggerBox();

	HRESULT								Change_Instance_To_Default();
	HRESULT								Update_Instance_WorldMinMax(const Vec3* pModelMinMax, const vector<Matrix>* vecInstanceMatrixPointer);
public:
	HRESULT								Add_MapToolComponent(CMapObject::COMPONENT eType);
public:
	void								Reset_OriginTransform(_int iIndex = -1);
	void								Override_OriginTransform(_int iIndex = -1);
public:
	void								Update_InstanceWorldMatrix(_bool isAllUpdate , _int iIndex = -1);
	void								Update_Bounds(_uint iIndex);
	void								Update_Collider();
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
	void								Set_SectionNumber(_uint iSectionNumber) { m_iSectionNum = iSectionNumber; }

public:

	_int								Get_InstanceCount();
	/* Load 관련 */
	_bool								Get_IsLoaded()				const { return m_isLoaded; }
	_bool								Get_IsUELoaded()			const { return m_isUELoaded; }
	wstring								Get_UERawDataPath()			const { return m_wstrUERawDataPath; }


	/* SRT Data 관련 */
	const vector< Tool::SRT_DATA >&		Get_SRTDatas(bool isOrigin) const { return isOrigin == true ? m_vecOriginSRTs : m_vecSRTs; }
	const SRT_DATA&						Get_SRTData(bool isOrigin, _int iIndex = -1) const;
	
	Vec3								Get_Scale(_int iIndex = -1 )		const;
	Quat								Get_Quaternion(_int iIndex = -1 )	const;
	Vec3								Get_Position(_int iIndex = -1 )		const;
	Matrix								Get_Matrix(_int iIndex =-1)			const;

	/* Type관련 */
	EMapObject_Type						Get_MapObjectType()			const	{ return m_eMapObjectType;}
	CMapObject::EState					Get_MapObjectState()		const	{ return m_eMapObjectState; }
	EClientMakePath						Get_ClientMakePath()		const	{ return m_eClientMakePath; }
	EClientLevelType					Get_ClientLevelType()		const	{ return m_eClientLevelType; }
	EMapObject_DrawType					Get_MapObjectDrawType()		const	{ return m_eMapObjectDrawType; }

	wstring								Get_ModelPath()				const	{ return m_wstrModelPath; }

	const USING_MODEL_INFO& Get_UsingModelInfo()					const { return m_tUsingModelInfo; }

	/* Override Mtl 관련 */
	bool								Get_IsUseOverrideMtl()		const	{ return m_isUseOverrideMaterials; }
	vector<CMaterial*>					Get_OverrideMtls()			const	{ return m_vecOverrideMaterials; }
	vector<wstring>						Get_OverrideMtlsName()		const;
	vector<wstring>						Get_TotalUseMtlsName();
	const vector<Tool::SRT_DATA>&		Get_SRTDatas()						{ return m_vecSRTs;}

	_int								Get_SelectedInstanceID()	const	 { return m_iSelectedInstanceID; }
	_uint								Get_SectionNumber()			const		{ return m_iSectionNum; }
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

	HRESULT								Set_GPU_BeforeRender(CShader* pShader);
	HRESULT								Set_GPU_MapObjectState(CShader* pShader);
public:
	_bool								IntsersectWithPlane(OUT Vec3& vOut, const Vec3& vLocalCamPos);
	_bool								Picking(OUT Vec3& vOut);
	virtual _bool						Export_Data(DTO::ECategory eCategory, CDataDocumentBase* pDocument)override;
private:
	HRESULT								Check_DrawType_ByClientPath();
public:
	void								ImGuiUpdate_EffectEnv();
	std::vector < std::pair<CEffect_Env*, EFFECT_ENV_DESC>>* Get_EffectEnvData() { return &m_vEnvEffectList; }
#pragma region BeforeRender
	void								BatchObject_BeforeRender(const _float fTimeDelta);
	void								Env_BeforeRender(const _float fTimeDelta);
#pragma endregion

#pragma region Render 함수 모음

	HRESULT								Render_MapObject();
	HRESULT								Render_StaticObject();
	HRESULT								Render_LandScape();

	HRESULT								Render_Plants(_uint iPassIndex);


	HRESULT								Bind_PlantBuffer(CShader* pShader = nullptr);
	HRESULT								Render_Grass();
	HRESULT								Render_Tree();
	HRESULT								Render_Moss();
	HRESULT								Render_Vine();
	HRESULT								Render_Bush();
	HRESULT								Render_Rock();

	HRESULT								Render_Water();
	HRESULT								Render_Env();

	HRESULT								Render_Batch_Player();
	HRESULT								Render_Batch_Monster();
	HRESULT								Render_Batch_Object();

	HRESULT								Render_TriggerBox_ChangeLevel();
	HRESULT								Render_TriggerBox_MonsterSpawner();
	HRESULT								Render_TriggerBox_GlobalEvent_BroadCaster();

	/* Collider Type 전용 Render함수 */
	HRESULT								Render_Collider();
#pragma endregion 


public:
	/* 기본적으로 사용할 애들 */
	/* Client Make Path를  */
	HRESULT								Render_Default(_int iPass = 0 );
	HRESULT								Render_Instance(_int iPass = 0);

protected:
	static constexpr	Vec3			m_vDefaultMinMax[2] = { Vec3(-5.f,-5.f, -5.f), Vec3(+5.f,+5.f, +5.f) };
protected:
	wstring								m_wstrUERawDataPath{L""};
	_uint								m_iSectionNum{};
	bool								m_isBatced{false};
	CMapToolManager*					m_pMapToolManager{ nullptr };

	EMapObject_Type						m_eMapObjectType	{ EMapObject_Type::END };
	EMapObject_DrawType					m_eMapObjectDrawType{ EMapObject_DrawType::Default };
	std::wstring						m_wstrModelPath		{ L"" };

	/* 이 오브젝트가 Client 에서 생성될 Path ID */
	EClientMakePath						m_eClientMakePath	{ EClientMakePath::END };
	EClientLevelType					m_eClientLevelType	{ EClientLevelType::END };
	/* 생성될 Path ID의 Desc를 가지고 있는다 */
	vector<CLIENT_MAKEPATH_DESC_BASE*>	m_vecClientMakePathDesc{};


	/* UE or Data Load 인지 판별 */
	bool								m_isLoaded{ false };
	bool								m_isUELoaded{ false };

	EState								m_eMapObjectState{ EState::Default};

	/* Instance 전용 Select ID */
	_int								m_iSelectedInstanceID{0};

	/* SRT Data */
	vector<Tool::SRT_DATA>				m_vecOriginSRTs{};	//
	vector<Matrix>						m_vecOriginMatrix{};

	vector<Tool::SRT_DATA>				m_vecSRTs{};
	vector<Matrix>						m_vecMatrix{};


	/* 임시 패기처분 */
	/* 다시 살리기 프로젝트 */
	/* Override Material을 담아줄 변수 */
	vector<CMaterial*>					m_vecOverrideMaterials;
	bool								m_isUseOverrideMaterials{ false };



	tagUsingModelInfo					m_tUsingModelInfo{};


	_float								m_fDT{};

	/* Instance Draw 컬링용 Min Max들고있기 */
	Vec3								m_vInstanceWorldMinMax[2]{ Vec3(FLT_MAX,FLT_MAX,FLT_MAX) , Vec3(-FLT_MAX,-FLT_MAX,-FLT_MAX)};

public:
	// pair로 이펙트를 key값으로 가지고 value로 SpawnDesc을 가지고 있는다 
	void								Add_EnvEffect(const string& EffectTag);
	CEffect_Env*						Get_EnvEffect(_uint iIndex);
	EFFECT_ENV_DESC						Get_EnvEffectDesc(_uint iIndex);
	void								Set_EnvEffectDesc(_uint iIndex, const EFFECT_ENV_DESC& Desc);
	void								Delete_EnvEffect(_uint iIndex);


	_bool								m_isModelRender{true};
	const Matrix*						m_pWorldMatPtr = {nullptr};	// 이거 이중포인터 때문에 반 강제적으로 캐싱해서 들고 있어야함ㅋ
																	// 폭파의 신 최정우 강림. (클라쪽에서 한번 Awake나 이떄 캐싱해서 들고있어라.)
	std::vector<std::pair<CEffect_Env*, EFFECT_ENV_DESC>>	m_vEnvEffectList = {};
public:
	static CMapObject*		Create(EToolObjectType eType, ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

	static CMapObject*		Clone(CMapObject* pPrototype  , const SRT_DATA& tSRT );
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free()									override;
};

NS_END

