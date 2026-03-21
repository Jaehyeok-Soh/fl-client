#pragma once
#include "Base.h"
#include "json_forward.h"


NS_BEGIN(Engine)

class CModel;
class CShader;
class CGameInstance;
class CGameObject;

/* 각 움직임별 카메라 움직임 정보 */
struct ENGINE_DLL Camera_Keyframe_Data
{
public:
	/* Json 저장용 데이터 */
	/* 카메라가 움직일 기준이될 Target정보  */
	EObjectEnumTag::Enum	eMoveBaseTarget{ EObjectEnumTag::DEFAULT };	/* 타겟 정보를 들고온다 */
	_int					iMoveBaseTarget_EventHandleIndex{NONE_INDEX};	/* Target을 가져올 HandleIndex */
	_int					iMoveBaseTargetBoneIndex{NONE_INDEX};			/* BoneIndex 정보 없으면 -1 있다면 0 이상 */
	Vec3					vPosition{ Vec3::Zero };						/* 포지션 Target이 있으면 Offset 값으로 적용된다 */
	
	ELerpType				eMoveLerpType{ ELerpType::Linear };			/* 이번 포지션값   이동에 Lerp를 쓸건지 말건지 */
	ELerpType				eLookAtLerpType{ ELerpType::Linear };			/* 이번 LookAt    이동에  Lerp를 쓸지 말지 */
	ELerpType				eFovLerpType{ ELerpType::NONE };				/* 이번 Fov값에   이동에  Lerp를 쓸지 말지 */

	/* Position , Look At , fov 가 전부 동일하게 사용하게된다 3개로 쪼갤려니 크게 어려워질거같아서 못쪼갬 ㅇㅇ */
	_float					fDuration{ 1.f };								/* 걸리는 시간 (초) */
	_float					fHoldTime{ 0.f };								/* 도착하고 다음 위치 이동까지 대기하는 시간 (초) */

	_float					fFov{ 60.f };									/* 카메라 줌인용 Fov 값 */

	/* 카메라가 바라볼 대상 */
	EObjectEnumTag::Enum    eLookAtTarget{ EObjectEnumTag::DEFAULT };	/* 바라볼 대상 */
	_bool					isUseRotation{ true } ;						/* 기본적으로 Rotation으로 설정 회전으로 할지 LookAt 바라볼건지 둘중 하나 */
	Vec3					vPitchYawRoll{ 0.f,0.f,0.f };				/* 바라볼 대상이 없다면 사용할 데이터 */
	Vec3					vLookAtOffset{ Vec3::Zero };				/* 타겟 위치에서 약간 위/아래를 볼 때 쓰는 오프셋 // 타겟이있으면 Offset 없으면 Target으로 잡힌다 */
	/* 바라볼 대상이 있을때 사용할 데이터들 */
	_int					iLookAtBoneIndex{ NONE_BONE_INDEX };		/* 바라볼 대상의 특정 뼈 */

public:
	/* Event 관련 */
	vector<CCS_EVENT_DESC>	vecOnReach_CCS_EventDesc{};					/* 이 KeyFrame에 도착했을떄 발송할 이벤트 Desc */
	vector<CCS_EVENT_DESC>	vecDepart_CCS_EventDesc{};					/* 이 KeyFrame이 다음 Index를 향해 출발할때 발송될 이벤트 Desc */
public:
	_bool					isOnReachEventWork{ false };
	_bool					isDepartEventWork{ false };
public:
	/* Cashing 용 데이터 */

	/* MoveBase */
	class CGameObject*		pCinematicMoveBaseTarget{ nullptr };
	/* LookAt Base */
	class CGameObject*		pCinematicLookAtTarget{ nullptr };
public:
	Camera_Keyframe_Data();
	Camera_Keyframe_Data(class CCameraMan* pCameraman);
	Camera_Keyframe_Data(const Camera_Keyframe_Data& rhs);
	Camera_Keyframe_Data& operator=(const Camera_Keyframe_Data& rhs);
	~Camera_Keyframe_Data();
public:
	Matrix				Get_WorldMatrix() const;

	/* BroadCast */
	void				BroadCast_DepartEvent();		/* Hold Time이 끝나고 불릴 함수 정확히말하면 HoldTime이 끝나고 또 불릴 일이 있나 카메라시퀀스 진입시, HoldTime이 끝나면 진입 완료 */
	void				BroadCast_OnReachEvent();	/* 다음 인덱스로 도착했을때 ? */
	void				Reset_EventWork();
public:
	void				Reset();
	void				Copy_Camera(class CCameraMan* pCameraman);
public:
	void				Save_Json(json& SaveJson);
	void				Load_Json(const json& LoadJson);
private:
	void				UnBind_CashingData();
};

struct ENGINE_DLL CinematicCameraSequence
{
	using Super = CBase;
public:
	CinematicCameraSequence(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CinematicCameraSequence(const CinematicCameraSequence& rhs);
	CinematicCameraSequence& operator=(const CinematicCameraSequence& rhs);
	virtual ~CinematicCameraSequence();
private:
	HRESULT									Initialize();
	HRESULT									Ready_Batch();
public:
	/* Key Frame 추가 관련 함수들 */
	void									Delete_KeyFrameData(_int iDeleteIndex);
	void									Reset_KeyFrameData(_int iResetIndex = -1);
	void									Copy_Camera_KeyFrameData(class CCameraMan* pCamera, _int iCopyIndex = -1);
	void									Add_KeyFrameData(_int iCopyBeforeDataIndex = false);
	void									Insert_KeyFrameData(_uint iCurIndex, CCameraMan* pCamera = nullptr);
public:
	/* Event 발송 관련 */
	void									BroadCast(CCS_BROADCAST_TYPE eType , _int iIndex = NONE_INDEX);
	void									BroadCast_BeginEvent();
	void									BroadCast_EndEvent();
public:
	/* Debug 렌더 관련 */
	HRESULT									Render_Debug(_uint iPassIndex, CModel* pCameraModel, CShader* pShader);
public:
	string											strName{};
public:
	/* Key Frame 관련 */
	vector<Camera_Keyframe_Data>					vecCamKeyFrameDatas{};
public:
	/* Event 관련 */
	vector<CCS_EVENT_DESC>							vecBegin_CCS_EventDesc{};
	vector<CCS_EVENT_DESC>							vecEnd_CCS_EventDesc{};
private:
	/* Debug Line을 위한 용도 */
	PrimitiveBatch<DirectX::VertexPositionColor>*	pBatch;
	BasicEffect*									pEffect;
	ID3D11InputLayout*								pInputLayout;
private:
	ID3D11Device*									pDevice;
	ID3D11DeviceContext*							pContext;
	CGameInstance*									pGameInstance;
public:
	void											Save_Json(json& SaveJson);
	void											Load_Json(const json& LoadJson);
};

NS_END

