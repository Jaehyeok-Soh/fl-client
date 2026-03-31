#include "Engine_pch.h"
#include "CinematicCameraSequence.h"
#include "CameraMan.h"
#include "Camera.h"
#include "Shader.h"
#include "Model.h"

#include "json.hpp"
#include "GameInstance.h"


#pragma region Camera KeyFrame Data

Camera_Keyframe_Data::Camera_Keyframe_Data()
	: eMoveBaseTarget(EObjectEnumTag::Enum::DEFAULT)
	, iMoveBaseTargetBoneIndex(NONE_BONE_INDEX)
	, vPosition(Vec3::Zero)
	, eMoveLerpType(ELerpType::Linear)
	, eLookAtLerpType(ELerpType::Linear)
	, eFovLerpType(ELerpType::NONE)
	, fDuration(1.f)
	, fHoldTime(0.f)
	, fFov(60.f)
	, eLookAtTarget(EObjectEnumTag::Enum::DEFAULT)
	, vPitchYawRoll(0.f, 0.f, 0.f)
	, iLookAtBoneIndex(NONE_BONE_INDEX)
	, vLookAtOffset(Vec3::Zero)
	, pCinematicMoveBaseTarget(nullptr)
	, pCinematicLookAtTarget(nullptr)
	, vecDepart_CCS_EventDesc{}
	, vecOnReach_CCS_EventDesc{}
{
}
Camera_Keyframe_Data::Camera_Keyframe_Data(CCameraMan* pCameraman)
	: eMoveBaseTarget(EObjectEnumTag::Enum::DEFAULT)
	, iMoveBaseTargetBoneIndex(NONE_BONE_INDEX)
	, vPosition(Vec3::Zero)
	, eMoveLerpType(ELerpType::Linear)
	, eLookAtLerpType(ELerpType::Linear)
	, eFovLerpType(ELerpType::NONE)
	, fDuration(1.f)
	, fHoldTime(0.f)
	, fFov(60.f)
	, eLookAtTarget(EObjectEnumTag::Enum::DEFAULT)
	, vPitchYawRoll(0.f, 0.f, 0.f)
	, iLookAtBoneIndex(NONE_BONE_INDEX)
	, vLookAtOffset(Vec3::Zero)
	, pCinematicMoveBaseTarget(nullptr)
	, pCinematicLookAtTarget(nullptr)
	, vecDepart_CCS_EventDesc{}
	, vecOnReach_CCS_EventDesc{}
{
	Copy_Camera(pCameraman);
}
Camera_Keyframe_Data::Camera_Keyframe_Data(const Camera_Keyframe_Data& rhs)
	: eMoveBaseTarget(rhs.eMoveBaseTarget)
	, iMoveBaseTargetBoneIndex(rhs.iMoveBaseTargetBoneIndex)
	, vPosition(rhs.vPosition)
	, eMoveLerpType(rhs.eMoveLerpType)
	, eLookAtLerpType(rhs.eLookAtLerpType)
	, eFovLerpType(rhs.eFovLerpType)
	, fDuration(rhs.fDuration)
	, fHoldTime(rhs.fHoldTime)
	, fFov(rhs.fFov)
	, eLookAtTarget(rhs.eLookAtTarget)
	, vPitchYawRoll(rhs.vPitchYawRoll)
	, iLookAtBoneIndex(rhs.iLookAtBoneIndex)
	, vLookAtOffset(rhs.vLookAtOffset)
	, vecDepart_CCS_EventDesc{ rhs.vecDepart_CCS_EventDesc }
	, vecOnReach_CCS_EventDesc{ rhs.vecOnReach_CCS_EventDesc }
{
	/* 내 현재 Cashing 데이터가 있다면 Unbind Cashing 해줘야한다 */
	this->UnBind_CashingData();

	this->pCinematicMoveBaseTarget = rhs.pCinematicMoveBaseTarget;
	Safe_AddRef(this->pCinematicMoveBaseTarget);

	this->pCinematicLookAtTarget = rhs.pCinematicLookAtTarget;
	Safe_AddRef(this->pCinematicLookAtTarget);

}
Camera_Keyframe_Data& Camera_Keyframe_Data::operator=(const Camera_Keyframe_Data& rhs)
{
	if (this == &rhs)
		return *this;

	this->eMoveBaseTarget = rhs.eMoveBaseTarget;
	this->iMoveBaseTargetBoneIndex = rhs.iMoveBaseTargetBoneIndex;
	this->vPosition = rhs.vPosition;

	this->eMoveLerpType = rhs.eMoveLerpType;
	this->eLookAtLerpType = rhs.eLookAtLerpType;
	this->eFovLerpType = rhs.eFovLerpType;

	this->fDuration = rhs.fDuration;
	this->fHoldTime = rhs.fHoldTime;
	this->fFov = rhs.fFov;

	this->eLookAtTarget = rhs.eLookAtTarget;
	this->vPitchYawRoll = rhs.vPitchYawRoll;
	this->iLookAtBoneIndex = rhs.iLookAtBoneIndex;
	this->vLookAtOffset = rhs.vLookAtOffset;

	this->vecDepart_CCS_EventDesc = rhs.vecDepart_CCS_EventDesc;
	this->vecOnReach_CCS_EventDesc = rhs.vecOnReach_CCS_EventDesc;


	/* 내 현재 Cashing 데이터가 있다면 Unbind Cashing 해줘야한다 */
	this->UnBind_CashingData();

	this->pCinematicMoveBaseTarget = rhs.pCinematicMoveBaseTarget;
	Safe_AddRef(this->pCinematicMoveBaseTarget);


	this->pCinematicLookAtTarget = rhs.pCinematicLookAtTarget;
	Safe_AddRef(this->pCinematicLookAtTarget);

	return *this;
}
Camera_Keyframe_Data::~Camera_Keyframe_Data()
{
	UnBind_CashingData();
}

Matrix Camera_Keyframe_Data::Get_WorldMatrix() const
{
	return Matrix::CreateFromYawPitchRoll(XMConvertToRadians(this->vPitchYawRoll.y), XMConvertToRadians(this->vPitchYawRoll.x), XMConvertToRadians(this->vPitchYawRoll.z))
		* Matrix::CreateTranslation(this->vPosition);
}

void Camera_Keyframe_Data::BroadCast_OnReachEvent()
{
	if (this->isOnReachEventWork == true)
		return;

	auto* pMgr = CGameInstance::GetInstance();

	for (auto& Index : this->vecOnReach_CCS_EventDesc)
	{

	}

	this->isOnReachEventWork = true;

	return;
}

void Camera_Keyframe_Data::Reset()
{
	UnBind_CashingData();

	this->eMoveBaseTarget = EObjectEnumTag::Enum::DEFAULT;
	this->iMoveBaseTargetBoneIndex = NONE_BONE_INDEX;
	this->vPosition = Vec3::Zero;
	this->eMoveLerpType = ELerpType::NONE;
	this->eLookAtLerpType = ELerpType::NONE;
	this->eFovLerpType = ELerpType::NONE;
	this->fDuration = 1.f;
	this->fHoldTime = 0.f;
	this->fFov = 60.f;
	this->eLookAtTarget = EObjectEnumTag::Enum::DEFAULT;
	this->vPitchYawRoll = { 0.f, 0.f, 0.f };
	this->iLookAtBoneIndex = NONE_BONE_INDEX;
	this->vLookAtOffset = Vec3::Zero;


	this->vecDepart_CCS_EventDesc.clear();
	this->vecOnReach_CCS_EventDesc.clear();
}

void Camera_Keyframe_Data::Copy_Camera(CCameraMan* pCameraman)
{
	if (pCameraman == nullptr) return;

	CTransform* pTs = pCameraman->Get_Component<CTransform>();	if (pTs == nullptr)	return;
	CCamera* pCamera = pCameraman->Get_Component<CCamera>();		if (pCamera == nullptr) return;

	/* 참조 정보를 초기화해준다 */
	this->eMoveBaseTarget = EObjectEnumTag::Enum::DEFAULT;
	this->eLookAtTarget = EObjectEnumTag::Enum::DEFAULT;


	Matrix CameraManWorldMatirx = pTs->Get_WorldMatrix();

	Vec3 vScale{};
	Quat vQuat{};
	CameraManWorldMatirx.Decompose(vScale, vQuat, vPosition);

	// 1. 일단 라디안 단위의 오일러 각도를 뽑아냄
	Vec3 vEulerRadian = vQuat.ToEuler();
	this->vPitchYawRoll.x = XMConvertToDegrees(vEulerRadian.x); // Pitch
	this->vPitchYawRoll.y = XMConvertToDegrees(vEulerRadian.y); // Yaw
	this->vPitchYawRoll.z = XMConvertToDegrees(vEulerRadian.z); // Roll

	/* Fov 값 */
	this->fFov = XMConvertToDegrees(pCamera->Get_BaseFov());
}

void Camera_Keyframe_Data::UnBind_CashingData()
{
	Safe_Release(this->pCinematicLookAtTarget);
	this->pCinematicLookAtTarget = nullptr;

	Safe_Release(this->pCinematicMoveBaseTarget);
	this->pCinematicMoveBaseTarget = nullptr;
}

void Camera_Keyframe_Data::Reset_EventWork()
{
	this->isOnReachEventWork = false;
	this->isDepartEventWork = false;
}

void Camera_Keyframe_Data::Save_Json(json& SaveJson)
{
	/* Move 관련 */
	SaveJson["MoveBase Target"] = EObjectEnumTag::ToString(this->eMoveBaseTarget);
	SaveJson["Move Lerp Type"] = Engine_Utils::LerpType_ToString(this->eMoveLerpType);
	SaveJson["MoveBase Target Bone Index"] = this->iMoveBaseTargetBoneIndex;
	Engine_Utils::write_vec3_xyz(SaveJson["Position"], this->vPosition);

	/* Look At 관련 */
	SaveJson["Look At Target"] = EObjectEnumTag::ToString(this->eLookAtTarget);
	SaveJson["Look At Lerp Type"] = Engine_Utils::LerpType_ToString(this->eLookAtLerpType);
	SaveJson["Look At Target Bone Index"] = this->iLookAtBoneIndex;
	Engine_Utils::write_vec3_xyz(SaveJson["Look At Offset"], this->vLookAtOffset);
	Engine_Utils::write_vec3_xyz(SaveJson["Pitch Yaw Roll"], this->vPitchYawRoll);

	/* Fov 관련 */
	SaveJson["Fov Lerp Type"] = Engine_Utils::LerpType_ToString(this->eFovLerpType);
	SaveJson["Fov"] = this->fFov;

	SaveJson["Duration"] = this->fDuration;
	SaveJson["HoldTime"] = this->fHoldTime;


	/* Event */
	if (!this->vecDepart_CCS_EventDesc.empty())
	{
		auto& Depart_SaveJson = SaveJson["Depart Event"];
		for (auto& Desc : this->vecOnReach_CCS_EventDesc)
		{
			if (Desc.strSubscriberName.empty())
				continue;
			if (Desc.vecActionNames.empty())
				continue;
			json Desc_SaveJson{json::object()};

			Desc_SaveJson["Subscriber Name"] = Desc.strSubscriberName;
			Desc_SaveJson["Action Names"] = Desc.vecActionNames;
			Depart_SaveJson.push_back(Desc_SaveJson);
		}
	}
	if (!this->vecOnReach_CCS_EventDesc.empty())
	{
		auto& OnReach_SaveJson = SaveJson["On Reach Event"];
		for (auto& Desc : this->vecOnReach_CCS_EventDesc)
		{
			if (Desc.strSubscriberName.empty())
				continue;
			if (Desc.vecActionNames.empty())
				continue;
			json Desc_SaveJson{ json::object() };
			Desc_SaveJson["Subscriber Name"]	= Desc.strSubscriberName;
			Desc_SaveJson["Action Names"]		= Desc.vecActionNames;
			OnReach_SaveJson.push_back(Desc_SaveJson);
		}
	}
}

void Camera_Keyframe_Data::Load_Json(const json& LoadJson)
{

	if (LoadJson.contains("MoveBase Target"))
		this->eMoveBaseTarget = EObjectEnumTag::ToEnum(LoadJson["MoveBase Target"].get<string>());

	if (LoadJson.contains("Move Lerp Type"))
		this->eMoveLerpType = Engine_Utils::LerpType_ToEnum(LoadJson["Move Lerp Type"].get<string>());

	if (LoadJson.contains("MoveBase Target Bone Index"))
		this->iMoveBaseTargetBoneIndex = LoadJson["MoveBase Target Bone Index"].get<_int>();

	if (LoadJson.contains("Position"))
		Engine_Utils::read_vec3_xyz(LoadJson["Position"], this->vPosition);


	/* Look At 관련 */
	if (LoadJson.contains("Look At Target"))
		this->eLookAtTarget = EObjectEnumTag::ToEnum(LoadJson["Look At Target"].get<string>());

	if (LoadJson.contains("Look At Lerp Type"))
		this->eLookAtLerpType = Engine_Utils::LerpType_ToEnum(LoadJson["Look At Lerp Type"].get<string>());

	if (LoadJson.contains("Look At Target Bone Index"))
		this->iLookAtBoneIndex = LoadJson["Look At Target Bone Index"].get<_int>();

	if (LoadJson.contains("Look At Offset"))
		Engine_Utils::read_vec3_xyz(LoadJson["Look At Offset"], this->vLookAtOffset);

	if (LoadJson.contains("Pitch Yaw Roll"))
		Engine_Utils::read_vec3_xyz(LoadJson["Pitch Yaw Roll"], this->vPitchYawRoll);


	/* Fov 관련 */
	if (LoadJson.contains("Fov Lerp Type"))
		this->eFovLerpType = Engine_Utils::LerpType_ToEnum(LoadJson["Fov Lerp Type"].get<string>());

	if (LoadJson.contains("Fov"))
		this->fFov = LoadJson["Fov"].get<_float>();

	/* 시간 관련 */
	if (LoadJson.contains("Duration"))
		this->fDuration = LoadJson["Duration"].get<_float>();

	if (LoadJson.contains("HoldTime"))
		this->fHoldTime = LoadJson["HoldTime"].get<_float>();


	if (LoadJson.contains("On Reach Event"))
	{
		this->vecOnReach_CCS_EventDesc.clear();

		auto& OnReachEvent_LoadJsonArray = LoadJson["On Reach Event"];
		if (OnReachEvent_LoadJsonArray.is_array())
		{
			vecOnReach_CCS_EventDesc.reserve(OnReachEvent_LoadJsonArray.size());
			for(auto& OnReachEvent_LoadJson : OnReachEvent_LoadJsonArray)
			{
				if (OnReachEvent_LoadJson.is_null())
					continue;
				CCS_EVENT_DESC tDesc{};

				if (OnReachEvent_LoadJson.contains("Subscriber Name"))
				{
					tDesc.strSubscriberName = OnReachEvent_LoadJson.at("Subscriber Name").get<string>();
				}
				if (OnReachEvent_LoadJson.contains("Action Names"))
				{
					tDesc.vecActionNames = OnReachEvent_LoadJson.at("Action Names").get<vector<string>>();
				}
				this->vecOnReach_CCS_EventDesc.push_back(tDesc);
			}
		}
	}
	
	if (LoadJson.contains("Depart Event"))
	{
		this->vecOnReach_CCS_EventDesc.clear();

		auto& OnReachEvent_LoadJsonArray = LoadJson["Depart Event"];
		if (OnReachEvent_LoadJsonArray.is_array())
		{
			vecOnReach_CCS_EventDesc.reserve(OnReachEvent_LoadJsonArray.size());
			for (auto& OnReachEvent_LoadJson : OnReachEvent_LoadJsonArray)
			{
				if (OnReachEvent_LoadJson.is_null())
					continue;
				CCS_EVENT_DESC tDesc{};

				if (OnReachEvent_LoadJson.contains("Subscriber Name"))
				{
					tDesc.strSubscriberName = OnReachEvent_LoadJson.at("Subscriber Name").get<string>();
				}
				if (OnReachEvent_LoadJson.contains("Action Names"))
				{
					tDesc.vecActionNames = OnReachEvent_LoadJson.at("Action Names").get<vector<string>>();
				}
				this->vecOnReach_CCS_EventDesc.push_back(tDesc);
			}
		}
	}
}


#pragma endregion









#pragma region Cinematic Camera Sequnce


CinematicCameraSequence::CinematicCameraSequence(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:pDevice(pDevice)
	,pContext(pContext)
	,pGameInstance{CGameInstance::GetInstance()}
	,pBatch{ nullptr }
	,pEffect{ nullptr }
	,pInputLayout{ nullptr }
{
	Safe_AddRef(this->pDevice);
	Safe_AddRef(this->pContext);
	Safe_AddRef(this->pGameInstance);


	Ready_Batch();
}

CinematicCameraSequence::CinematicCameraSequence(const CinematicCameraSequence& rhs)
	: pDevice{ rhs.pDevice }
	, pContext{rhs.pContext}
	, pGameInstance{ rhs.pGameInstance}
	, vecCamKeyFrameDatas{rhs.vecCamKeyFrameDatas}
	, vecBegin_CCS_EventDesc{rhs.vecBegin_CCS_EventDesc}
	, vecEnd_CCS_EventDesc{ rhs.vecEnd_CCS_EventDesc }
	, strName{rhs.strName}
	, pEffect{nullptr}
	, pBatch{ nullptr }
	, pInputLayout{nullptr}
{
	Safe_AddRef(this->pDevice);
	Safe_AddRef(this->pContext);
	Safe_AddRef(this->pGameInstance);

	Ready_Batch();
}

CinematicCameraSequence& CinematicCameraSequence::operator=(const CinematicCameraSequence& rhs)
{
	if (this == &rhs)
		return *this;

	this->strName = rhs.strName;
	this->vecCamKeyFrameDatas = rhs.vecCamKeyFrameDatas;

	Safe_Release(this->pDevice);
	Safe_Release(this->pContext);

	this->pDevice = rhs.pDevice;      Safe_AddRef(pDevice);
	this->pContext = rhs.pContext;      Safe_AddRef(pContext);

	this->vecBegin_CCS_EventDesc = rhs.vecBegin_CCS_EventDesc;
	this->vecEnd_CCS_EventDesc = rhs.vecEnd_CCS_EventDesc;

	if (!this->pBatch)
		this->pBatch = new PrimitiveBatch<VertexPositionColor>(this->pContext);
	if (!this->pEffect)
		this->pEffect = new BasicEffect(this->pDevice);

	this->pEffect->SetVertexColorEnabled(true);

	if (!this->pInputLayout)
	{
		const void* pShaderInput = { nullptr };
		size_t iShaderInputLenght = {};
		this->pEffect->GetVertexShaderBytecode(&pShaderInput, &iShaderInputLenght);
		this->pDevice->CreateInputLayout(VertexPositionColor::InputElements, VertexPositionColor::InputElementCount, pShaderInput, iShaderInputLenght, &this->pInputLayout);
	}

	return *this;
}

CinematicCameraSequence::~CinematicCameraSequence()
{
	/* Batch Debug 관련 */
	Safe_Delete(pBatch);
	Safe_Delete(pEffect);
	Safe_Release(pInputLayout);

	/* Device 및 GameInstance */
	Safe_Release(pDevice);
	Safe_Release(pContext);
	Safe_Release(pGameInstance);
}

HRESULT CinematicCameraSequence::Initialize()
{
	/* Ready */
	if (FAILED(Ready_Batch()))
		return E_FAIL;

	return S_OK;
}

HRESULT CinematicCameraSequence::Ready_Batch()
{
	pBatch = new PrimitiveBatch<VertexPositionColor>(pContext);
	pEffect = new BasicEffect(pDevice);

	const void* pShaderInput = { nullptr };
	size_t iShaderInputLenght = {};
	pEffect->GetVertexShaderBytecode(&pShaderInput, &iShaderInputLenght);
	pDevice->CreateInputLayout(VertexPositionColor::InputElements, VertexPositionColor::InputElementCount, pShaderInput, iShaderInputLenght, &pInputLayout);

	return S_OK;
}


void CinematicCameraSequence::Delete_KeyFrameData(_int iDeleteIndex)
{
	if (iDeleteIndex >= this->vecCamKeyFrameDatas.size()) return;
	this->vecCamKeyFrameDatas.erase(this->vecCamKeyFrameDatas.begin() + iDeleteIndex);
}

void CinematicCameraSequence::Reset_KeyFrameData(_int iResetIndex)
{
	this->vecBegin_CCS_EventDesc.clear();
	this->vecEnd_CCS_EventDesc.clear();

	/* 전체 리셋 */
	if (iResetIndex == -1)
	{
		this->vecCamKeyFrameDatas.clear();
	}
	else
	{
		if (iResetIndex >= this->vecCamKeyFrameDatas.size()) return;

		this->vecCamKeyFrameDatas[iResetIndex].Reset();
	}
}

void CinematicCameraSequence::Copy_Camera_KeyFrameData(class CCameraMan* pCamera, _int iCopyIndex)
{
	if (iCopyIndex == -1)
	{
		this->vecCamKeyFrameDatas.push_back(Camera_Keyframe_Data(pCamera));
		return;
	}

	if (iCopyIndex >= this->vecCamKeyFrameDatas.size())
		return;

	this->vecCamKeyFrameDatas[iCopyIndex].Copy_Camera(pCamera);
}

void CinematicCameraSequence::Add_KeyFrameData(_int iCopyBeforeDataIndex)
{
	/* 현재 Vec가 비어있다면 그냥추가  */
	if (this->vecCamKeyFrameDatas.empty() || iCopyBeforeDataIndex == -1)
		vecCamKeyFrameDatas.push_back(Camera_Keyframe_Data());
	else
	{
		/* Boudinig Check */
		if (iCopyBeforeDataIndex >= this->vecCamKeyFrameDatas.size())
			return;

		this->vecCamKeyFrameDatas.push_back(this->vecCamKeyFrameDatas[iCopyBeforeDataIndex]);
	}
}

void CinematicCameraSequence::Insert_KeyFrameData(_uint iCurIndex, CCameraMan* pCamera)
{
	if (iCurIndex >= this->vecCamKeyFrameDatas.size() && !this->vecCamKeyFrameDatas.empty())
		return;

	Camera_Keyframe_Data newData{};

	// (선택사항) 새로 추가할 때 기존 키프레임의 Fov나 시간 데이터를 기본으로 복사해두면 편합니다.
	if (!this->vecCamKeyFrameDatas.empty())
	{
		newData = this->vecCamKeyFrameDatas[iCurIndex];
		// 이벤트나 불필요한 고유 데이터가 같이 복사되는게 싫다면 여기서 초기화
		newData.vecDepart_CCS_EventDesc.clear();
		newData.vecOnReach_CCS_EventDesc.clear();
	}

	// [수정 1] nullptr 조건문 로직을 올바르게 변경 (!= 로 수정)
	if (pCamera != nullptr)
	{
		Vec3 vScale{};
		Quat vQuat{};
		Matrix WorldMatrix = pCamera->Get_Component<CTransform>()->Get_WorldMatrix();
		WorldMatrix.Decompose(vScale, vQuat, newData.vPosition);

		Vec3 vRoatation = vQuat.ToEuler();
		newData.vPitchYawRoll = { XMConvertToDegrees(vRoatation.x), XMConvertToDegrees(vRoatation.y), XMConvertToDegrees(vRoatation.z) };
	}
	else
	{
		// 카메라가 없다면 선택한 인덱스의 매트릭스 정보를 그대로 가져옵니다.
		Vec3 vScale{};
		Quat vQuat{};
		Matrix WorldMatrix = this->vecCamKeyFrameDatas[iCurIndex].Get_WorldMatrix();
		WorldMatrix.Decompose(vScale, vQuat, newData.vPosition);

		Vec3 vRoatation = vQuat.ToEuler();
		newData.vPitchYawRoll = { XMConvertToDegrees(vRoatation.x), XMConvertToDegrees(vRoatation.y), XMConvertToDegrees(vRoatation.z) };
	}

	// [수정 2] 벡터에 실제로 데이터를 밀어 넣는 로직 추가 (현재 인덱스 바로 '다음'에 삽입)
	auto iter = this->vecCamKeyFrameDatas.begin() + iCurIndex + 1;
	this->vecCamKeyFrameDatas.insert(iter, newData);
}
void CinematicCameraSequence::BroadCast(CCS_BROADCAST_TYPE eType, _int iIndex)
{
	CCS_BROADCAST_DESC tDesc{};
	tDesc.eCCS_BroadCast_Type = eType;
	tDesc.iCurrentCameraKeyFrameIndex = iIndex;
	tDesc.strCCSName = this->strName;

	switch (eType)
	{
	case Engine::CCS_BROADCAST_TYPE::BEGIN_CCS:
	{
		tDesc.vecCCS_Event_Desc = this->vecBegin_CCS_EventDesc;

	}
		break;
	case Engine::CCS_BROADCAST_TYPE::END_CCS:
	{
		tDesc.vecCCS_Event_Desc = this->vecEnd_CCS_EventDesc;
	}
	break;
	case Engine::CCS_BROADCAST_TYPE::DEPART:
	{
		tDesc.vecCCS_Event_Desc = this->vecCamKeyFrameDatas[iIndex].vecDepart_CCS_EventDesc;
	}
		break;
	case Engine::CCS_BROADCAST_TYPE::ON_REACH:
	{
		tDesc.vecCCS_Event_Desc = this->vecCamKeyFrameDatas[iIndex].vecOnReach_CCS_EventDesc;
	}
		break;
	default:
		break;
	}

	CGameInstance::GetInstance()->Broadcast<CCS_EVENT>(tDesc);

}

void CinematicCameraSequence::BroadCast_BeginEvent()
{
	for (auto& CCS_EventDesc : this->vecBegin_CCS_EventDesc)
	{
	}
}

void CinematicCameraSequence::BroadCast_EndEvent()
{
	for (auto& CCS_EventDesc : this->vecEnd_CCS_EventDesc)
	{

	}
}

HRESULT CinematicCameraSequence::Render_Debug(_uint iPassIndex, CModel* pCameraModel, CShader* pShader)
{
	if (!this->pDevice)
	{
		MSG_BOX(" Device is None Setting ");
		return E_FAIL;
	}

	if (!this->pContext)
	{
		MSG_BOX(" Context  is None Setting ");
		return E_FAIL;
	}

	if (!pCameraModel)
	{
		MSG_BOX(" Camera Model is None Setting ");
		return E_FAIL;
	}

	if (!pShader)
	{
		MSG_BOX(" Debug Model is None Setting ");
		return E_FAIL;
	}

	auto pGameInstance = CGameInstance::GetInstance();
	pGameInstance->Setup_ViewProj_ToCBuffer();

	vector<Vec3> vecPosition{};
	vecPosition.reserve(this->vecCamKeyFrameDatas.size());

	for (auto& CamData : this->vecCamKeyFrameDatas)
	{
		/* World Matrix Binding */
		Matrix WorldMatrix = CamData.Get_WorldMatrix();
		pShader->Bind_TransformData(WorldMatrix);

		vecPosition.push_back(WorldMatrix.Translation());

		/* World Matrix Binding */
		_uint iMeshCount = pCameraModel->Get_MeshCount();

		/* Pass 값 세팅 */
		pShader->Set_Pass(iPassIndex);

		for (_uint i = 0; i < iMeshCount; ++i)
		{
			pCameraModel->Bind_Material(pShader, i);
			pCameraModel->Bind_MaterialInstance(pShader, i);
			pShader->Apply();
			pCameraModel->Render(i);
		}
	}

	/* 2개 이상일때 */
	if (this->pBatch && this->pEffect && vecPosition.size() >= 2)
	{

		this->pContext->GSSetShader(nullptr, nullptr, 0);
		this->pContext->RSSetState(nullptr);
		this->pContext->OMSetBlendState(nullptr, nullptr, 0xffffffff);

		this->pEffect->Apply(this->pContext);
		this->pContext->IASetInputLayout(this->pInputLayout);
		this->pBatch->Begin();

		for (size_t i = 0; i < vecPosition.size() - 1; ++i)
		{
			pEffect->SetView(pGameInstance->Get_ViewMatrix());
			pEffect->SetProjection(pGameInstance->Get_ProjMatrix());
			pEffect->SetWorld(Matrix::Identity); // 경로는 월드좌표 기준이므로 Identity

			Vec3 vPos1 = vecCamKeyFrameDatas[i].vPosition;
			Vec3 vPos2 = vecCamKeyFrameDatas[i + 1].vPosition;

			// 색상은 노란색(Yellow) 등 취향껏 설정
			VertexPositionColor v1(vPos1, DirectX::Colors::Magenta);
			VertexPositionColor v2(vPos2, DirectX::Colors::Magenta);

			pBatch->DrawLine(v1, v2);
		}

		pBatch->End();
	}

	return S_OK;
}

void CinematicCameraSequence::Save_Json(json& SaveJson)
{
	/* Event */

	if (!this->vecBegin_CCS_EventDesc.empty())
	{
		auto& BeginEvents_SaveJson = SaveJson["Begin Events"];
		for (auto& Desc : this->vecBegin_CCS_EventDesc)
		{
			if (Desc.strSubscriberName.empty())
				continue;
			if (Desc.vecActionNames.empty())
				continue;
			json Desc_SaveJson = json::object();
			Desc_SaveJson["Subscriber Name"] = Desc.strSubscriberName;
			Desc_SaveJson["Action Names"] = Desc.vecActionNames;
			BeginEvents_SaveJson.push_back(Desc_SaveJson);
		}
	}

	if (!this->vecEnd_CCS_EventDesc.empty())
	{
		auto& EndEvents_SaveJson = SaveJson["End Events"];
		for (auto& Desc : this->vecEnd_CCS_EventDesc)
		{
			if (Desc.strSubscriberName.empty())
				continue;
			if (Desc.vecActionNames.empty())
				continue;
			json Desc_SaveJson = json::object();
			Desc_SaveJson["Subscriber Name"] = Desc.strSubscriberName;
			Desc_SaveJson["Action Names"] = Desc.vecActionNames;
			EndEvents_SaveJson.push_back(Desc_SaveJson);
		}
	}

	if (!this->vecCamKeyFrameDatas.empty())
	{
		auto& CamKeyFrameDatas_SaveJson = SaveJson["Camera Key Frame Datas"];
		for (auto& KeyFrameData : this->vecCamKeyFrameDatas)
		{
			json KeyFrameData_SaveJson = json::object();
			KeyFrameData.Save_Json(KeyFrameData_SaveJson);
			CamKeyFrameDatas_SaveJson.push_back(KeyFrameData_SaveJson);
		}
	}
}

void CinematicCameraSequence::Load_Json(const json& LoadJson)
{
	this->vecCamKeyFrameDatas.clear();
	if (LoadJson.contains("Camera Key Frame Datas"))
	{
		auto& CamKeyFrames_LoadJson = LoadJson["Camera Key Frame Datas"];

		if (CamKeyFrames_LoadJson.is_array())
		{
			for (auto& DataJson : CamKeyFrames_LoadJson)
			{
				if (DataJson.is_null())
					continue;
				Camera_Keyframe_Data tData{};
				tData.Load_Json(DataJson);
				this->vecCamKeyFrameDatas.push_back(tData);
			}
		}
	}


	this->vecBegin_CCS_EventDesc.clear();
	if (LoadJson.contains("Begin Events"))
	{
		auto& BeginEvents_LoadJson = LoadJson["Begin Events"];
		if (BeginEvents_LoadJson.is_array())
		{
			for (auto& Desc_Json : BeginEvents_LoadJson)
			{
				if (Desc_Json.is_null())
					continue;
				CCS_EVENT_DESC tDesc{};
				if (Desc_Json.contains("Subscriber Name"))
				{
					tDesc.strSubscriberName = Desc_Json["Subscriber Name"].get<string>();
				}
				if (Desc_Json.contains("Action Names"))
				{
					tDesc.vecActionNames = Desc_Json["Action Names"].get<vector<string>>();
				}
				this->vecBegin_CCS_EventDesc.push_back(tDesc);
			}
		}
	}

	this->vecEnd_CCS_EventDesc.clear();
	if (LoadJson.contains("End Events"))
	{
		auto& EndEvents_LoadJson = LoadJson["End Events"];

		if (EndEvents_LoadJson.is_array())
		{
			for (auto& Desc_Json : EndEvents_LoadJson)
			{
				if (Desc_Json.is_null())
					continue;
				CCS_EVENT_DESC tDesc{};
				if (Desc_Json.contains("Subscriber Name"))
				{
					tDesc.strSubscriberName = Desc_Json["Subscriber Name"].get<string>();
				}
				if (Desc_Json.contains("Action Names"))
				{
					tDesc.vecActionNames = Desc_Json["Action Names"].get<vector<string>>();
				}
				this->vecEnd_CCS_EventDesc.push_back(tDesc);
			}
		}
	}
}



#pragma endregion