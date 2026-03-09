#include "Engine_pch.h"
#include "GameData_Struct.h"
#include "Engine_Utils.h"
#include "json.hpp"
#include "GameObject.h"
#include "Model.h"
#include "Camera.h"
#include "CameraMan.h"
#include "Shader.h"
#include "GameInstance.h"


namespace Engine
{

#pragma region Texture Splating Save Load Json

	void MIX_RGBA_INFO::Save_Json(json& SaveJson)
	{
		/* 배열로 저장해야함 */
		SaveJson = json::array();
		for (_int i = 0; i < this->iUse_Mix_RGBA_Count; ++i)
		{
			json Element{};

			Element["RGBA Texture"] = this->vecMixRGBATexture[i] == nullptr ? "None" : "Texture_" + Engine_Utils::ToString(this->vecMixRGBATexture[i]->Get_Name());
			this->vecMix_RGBA_Data[i].Save_Json(Element);
			SaveJson.push_back(Element);
		}
	}
	void MIX_RGBA_INFO::Load_Json(const json& LoadJson)
	{
		if (!LoadJson.is_array()) return;
		if (LoadJson.is_null()) return;

		this->vecMixRGBATexture.clear();
		this->vecMix_RGBA_Data.clear();

		_uint i = 0;
		for (const auto& Element : LoadJson)
		{
			string strRGBATextureName = Element.value("RGBA Texture", "None");
			this->vecMixRGBATexture.push_back(strRGBATextureName == "None" ? nullptr : CGameInstance::GetInstance()->GetOrAddTexture(Engine_Utils::ToWString(strRGBATextureName), nullptr));


			MIX_RGBA_DATA tData{};
			tData.Load_Json(Element);
			this->vecMix_RGBA_Data.push_back(tData);
			i++;
		}
		/* 개수 대입 */
		this->iUse_Mix_RGBA_Count = i;
	}

	void TEXTURE_SPLATTING_INFO::Save_Json(json& SaveJson)
	{
		SaveJson["Base Texture"] = this->pBase_Texture == nullptr ? "None" : "Texture_" +  Engine_Utils::ToString(pBase_Texture->Get_Name());
		SaveJson["Mix DH Tile Texture"] = this->pMix_DH_Tile_Texture == nullptr ? "None" : "Texture_" +  Engine_Utils::ToString(this->pMix_DH_Tile_Texture->Get_Name());
		SaveJson["Mix NBR Tile Texture"] = this->pMix_NBR_Tile_Texture == nullptr ? "None" : "Texture_" + Engine_Utils::ToString(this->pMix_NBR_Tile_Texture->Get_Name());

		json& Save_MixRGBAInfo_Json = SaveJson["Mix RGBA Info"];
		this->tMix_RGBA_Info.Save_Json(Save_MixRGBAInfo_Json);
	}
	void TEXTURE_SPLATTING_INFO::Load_Json(const json& LoadJson)
	{
		string TextureName = LoadJson.value("Base Texture", "None");
		this->pBase_Texture = TextureName == "None" ? nullptr : CGameInstance::GetInstance()->GetOrAddTexture(Engine_Utils::ToWString(TextureName), nullptr);

		TextureName = LoadJson.value("Mix DH Tile Texture", "None");
		this->pMix_DH_Tile_Texture = TextureName == "None" ? nullptr : CGameInstance::GetInstance()->GetOrAddTexture(Engine_Utils::ToWString(TextureName), nullptr);

		TextureName = LoadJson.value("Mix NBR Tile Texture", "None");
		this->pMix_NBR_Tile_Texture = TextureName == "None" ? nullptr : CGameInstance::GetInstance()->GetOrAddTexture(Engine_Utils::ToWString(TextureName), nullptr);


		if (LoadJson.contains("Mix RGBA Info"))
			this->tMix_RGBA_Info.Load_Json(LoadJson["Mix RGBA Info"]);

		return;
	}

	void MIX_RGBA_DATA::Save_Json(json& SaveJson)
	{

		SaveJson["R"]["RGBA Mix Force"] = this->fRGBA_Mix_Forces[CHANNEL_R];
		SaveJson["R"]["RGBA Mix Height Force"] = this->fRGBA_Mix_Height_Forces[CHANNEL_R];
		SaveJson["R"]["RGBA Connected Tile Index"] = this->iRGBA_Connected_Tile_Index[CHANNEL_R];
		SaveJson["R"]["Use Flags"] = this->iUseFlags[CHANNEL_R];

		SaveJson["G"]["RGBA Mix Force"] = this->fRGBA_Mix_Forces[CHANNEL_G];
		SaveJson["G"]["RGBA Mix Height Force"] = this->fRGBA_Mix_Height_Forces[CHANNEL_G];
		SaveJson["G"]["RGBA Connected Tile Index"] = this->iRGBA_Connected_Tile_Index[CHANNEL_G];
		SaveJson["G"]["Use Flags"] = this->iUseFlags[CHANNEL_G];

		SaveJson["B"]["RGBA Mix Force"] = this->fRGBA_Mix_Forces[CHANNEL_B];
		SaveJson["B"]["RGBA Mix Height Force"] = this->fRGBA_Mix_Height_Forces[CHANNEL_B];
		SaveJson["B"]["RGBA Connected Tile Index"] = this->iRGBA_Connected_Tile_Index[CHANNEL_B];
		SaveJson["B"]["Use Flags"] = this->iUseFlags[CHANNEL_B];

		SaveJson["A"]["RGBA Mix Force"] = this->fRGBA_Mix_Forces[CHANNEL_A];
		SaveJson["A"]["RGBA Mix Height Force"] = this->fRGBA_Mix_Height_Forces[CHANNEL_A];
		SaveJson["A"]["RGBA Connected Tile Index"] = this->iRGBA_Connected_Tile_Index[CHANNEL_A];
		SaveJson["A"]["Use Flags"] = this->iUseFlags[CHANNEL_A];

	}

	void MIX_RGBA_DATA::Load_Json(const json& LoadJson)
	{

		if (LoadJson.contains("R"))
		{
			auto& LoadJson_R = LoadJson["R"];
			this->fRGBA_Mix_Forces[CHANNEL_R] = LoadJson_R.value("RGBA Mix Force", 1.f);
			this->fRGBA_Mix_Height_Forces[CHANNEL_R] = LoadJson_R.value("RGBA Mix Height Force", 1.f);
			this->iRGBA_Connected_Tile_Index[CHANNEL_R] = LoadJson_R.value("RGBA Connected Tile Index", 0);
			this->iUseFlags[CHANNEL_R] = LoadJson_R.value("Use Flags", 1);
		}


		if (LoadJson.contains("G"))
		{
			auto& LoadJson_G = LoadJson["G"];
			this->fRGBA_Mix_Forces[CHANNEL_G] = LoadJson_G.value("RGBA Mix Force", 1.f);
			this->fRGBA_Mix_Height_Forces[CHANNEL_G] = LoadJson_G.value("RGBA Mix Height Force", 1.f);
			this->iRGBA_Connected_Tile_Index[CHANNEL_G] = LoadJson_G.value("RGBA Connected Tile Index", 0);
			this->iUseFlags[CHANNEL_G] = LoadJson_G.value("Use Flags", 1);
		}


		if (LoadJson.contains("B"))
		{
			auto& LoadJson_B = LoadJson["B"];
			this->fRGBA_Mix_Forces[CHANNEL_B] = LoadJson_B.value("RGBA Mix Force", 1.f);
			this->fRGBA_Mix_Height_Forces[CHANNEL_B] = LoadJson_B.value("RGBA Mix Height Force", 1.f);
			this->iRGBA_Connected_Tile_Index[CHANNEL_B] = LoadJson_B.value("RGBA Connected Tile Index", 0);
			this->iUseFlags[CHANNEL_B] = LoadJson_B.value("Use Flags", 1);
		}


		if (LoadJson.contains("A"))
		{
			auto& LoadJson_A = LoadJson["A"];
			this->fRGBA_Mix_Forces[CHANNEL_A] = LoadJson_A.value("RGBA Mix Force", 1.f);
			this->fRGBA_Mix_Height_Forces[CHANNEL_A] = LoadJson_A.value("RGBA Mix Height Force", 1.f);
			this->iRGBA_Connected_Tile_Index[CHANNEL_A] = LoadJson_A.value("RGBA Connected Tile Index", 0);
			this->iUseFlags[CHANNEL_A] = LoadJson_A.value("Use Flags", 1);
		}
	}

#pragma endregion


#pragma region Camera Cenematic



#pragma region Camera_Keyframe_Data
	Camera_Keyframe_Data::Camera_Keyframe_Data()
		: eMoveBaseTarget(ECinematicTarget::NONE)
		, iMoveBaseTargetBoneIndex(NONE_BONE_INDEX)
		, vPosition(Vec3::Zero)
		, eMoveLerpType(ELerpType::Linear)
		, eLookAtLerpType(ELerpType::Linear)
		, eFovLerpType(ELerpType::NONE)
		, fDuration(1.f)
		, fHoldTime(0.f)
		, fFov(60.f)
		, eLookAtTarget(ECinematicTarget::NONE)
		, vPitchYawRoll(0.f, 0.f, 0.f)
		, iLookAtBoneIndex(NONE_BONE_INDEX)
		, vLookAtOffset(Vec3::Zero)
		, pCinematicMoveBaseTarget(nullptr)
		, pCinematicMoveBaseModel(nullptr)
		, pCinematicLookAtTarget(nullptr)
		, pCinematicLookAtModel(nullptr)
	{
	}
	Camera_Keyframe_Data::Camera_Keyframe_Data(CCameraMan* pCameraman)
		: eMoveBaseTarget(ECinematicTarget::NONE)
		, iMoveBaseTargetBoneIndex(NONE_BONE_INDEX)
		, vPosition(Vec3::Zero)
		, eMoveLerpType(ELerpType::Linear)
		, eLookAtLerpType(ELerpType::Linear)
		, eFovLerpType(ELerpType::NONE)
		, fDuration(1.f)
		, fHoldTime(0.f)
		, fFov(60.f)
		, eLookAtTarget(ECinematicTarget::NONE)
		, vPitchYawRoll(0.f, 0.f, 0.f)
		, iLookAtBoneIndex(NONE_BONE_INDEX)
		, vLookAtOffset(Vec3::Zero)
		, pCinematicMoveBaseTarget(nullptr)
		, pCinematicMoveBaseModel(nullptr)
		, pCinematicLookAtTarget(nullptr)
		, pCinematicLookAtModel(nullptr)
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
	{
		/* 내 현재 Cashing 데이터가 있다면 Unbind Cashing 해줘야한다 */
		this->UnBind_CashingData();

		this->pCinematicMoveBaseTarget = rhs.pCinematicMoveBaseTarget;
		Safe_AddRef(this->pCinematicMoveBaseTarget);

		this->pCinematicMoveBaseModel = rhs.pCinematicMoveBaseModel;
		Safe_AddRef(this->pCinematicMoveBaseModel);

		this->pCinematicLookAtTarget = rhs.pCinematicLookAtTarget;
		Safe_AddRef(this->pCinematicLookAtTarget);

		this->pCinematicLookAtModel = rhs.pCinematicLookAtModel;
		Safe_AddRef(this->pCinematicLookAtModel);

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



		/* 내 현재 Cashing 데이터가 있다면 Unbind Cashing 해줘야한다 */
		this->UnBind_CashingData();

		this->pCinematicMoveBaseTarget = rhs.pCinematicMoveBaseTarget;
		Safe_AddRef(this->pCinematicMoveBaseTarget);

		this->pCinematicMoveBaseModel = rhs.pCinematicMoveBaseModel;
		Safe_AddRef(this->pCinematicMoveBaseModel);

		this->pCinematicLookAtTarget = rhs.pCinematicLookAtTarget;
		Safe_AddRef(this->pCinematicLookAtTarget);

		this->pCinematicLookAtModel = rhs.pCinematicLookAtModel;
		Safe_AddRef(this->pCinematicLookAtModel);

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

	void Camera_Keyframe_Data::Reset()
	{
		UnBind_CashingData();
		
		eMoveBaseTarget				= ECinematicTarget::NONE;
		iMoveBaseTargetBoneIndex	= NONE_BONE_INDEX;
		vPosition					= Vec3::Zero;
		eMoveLerpType				= ELerpType::NONE;
		eLookAtLerpType				= ELerpType::NONE;
		eFovLerpType				= ELerpType::NONE;
		fDuration					= 1.f;
		fHoldTime					= 0.f;
		fFov						= 60.f;
		eLookAtTarget				= ECinematicTarget::NONE;
		vPitchYawRoll				= { 0.f, 0.f, 0.f };
		iLookAtBoneIndex			= NONE_BONE_INDEX;
		vLookAtOffset				= Vec3::Zero;
	}

	void Camera_Keyframe_Data::Copy_Camera(CCameraMan* pCameraman)
	{
		if (pCameraman == nullptr) return;

		CTransform* pTs = pCameraman->Get_Component<CTransform>();	if (pTs == nullptr)	return;
		CCamera* pCamera = pCameraman->Get_Component<CCamera>();		if (pCamera == nullptr) return;

		/* 참조 정보를 초기화해준다 */
		this->eMoveBaseTarget	= ECinematicTarget::NONE;
		this->eLookAtTarget		= ECinematicTarget::NONE;


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
		this->fFov = XMConvertToDegrees(pCamera->Get_Fov());
	}

	void Camera_Keyframe_Data::UnBind_CashingData()
	{
		Safe_Release(this->pCinematicLookAtModel);
		this->pCinematicLookAtModel = nullptr;
		Safe_Release(this->pCinematicLookAtTarget);
		this->pCinematicLookAtTarget = nullptr;
		Safe_Release(this->pCinematicMoveBaseModel);
		this->pCinematicMoveBaseModel = nullptr;
		Safe_Release(this->pCinematicMoveBaseTarget);
		this->pCinematicMoveBaseTarget = nullptr;
	}

#pragma endregion

#pragma region Camera_Cinematic_Sequence
	Camera_Cinematic_Sequence::Camera_Cinematic_Sequence(ID3D11Device* pDevice , ID3D11DeviceContext* pContext)
		: strDataName{}
		, vecCamKeyFrameData{}
		, pDevice{ pDevice }
		, pContext{ pContext }
		, pBatch{nullptr}
		, pEffect{nullptr}
		, pInputLayout{nullptr}
		, isDebugRender{true}
		, vecStartCinematic_GlobalEventIndex{}
		, vecEndCinematic_GlobalEventIndex{}
	{
		Safe_AddRef(pDevice);
		Safe_AddRef(pContext);

		if(!pBatch)
			pBatch = new PrimitiveBatch<VertexPositionColor>(pContext);
		if(!pEffect)
			pEffect = new BasicEffect(pDevice);

		pEffect->SetVertexColorEnabled(true);

		if (!pInputLayout)
		{
			const void* pShaderInput = { nullptr };
			size_t iShaderInputLenght = {};
			pEffect->GetVertexShaderBytecode(&pShaderInput, &iShaderInputLenght);
			pDevice->CreateInputLayout(VertexPositionColor::InputElements, VertexPositionColor::InputElementCount, pShaderInput, iShaderInputLenght, &pInputLayout);
		}
	}

	Camera_Cinematic_Sequence::Camera_Cinematic_Sequence(const Camera_Cinematic_Sequence& rhs)
		: strDataName{rhs.strDataName}
		, vecCamKeyFrameData{rhs.vecCamKeyFrameData }
		, pDevice{ rhs.pDevice }
		, pContext{ rhs.pContext }
		, vecStartCinematic_GlobalEventIndex{ rhs.vecStartCinematic_GlobalEventIndex }
		, vecEndCinematic_GlobalEventIndex{ rhs.vecEndCinematic_GlobalEventIndex }
		, pBatch{ nullptr }
		, pEffect{ nullptr }
		, pInputLayout{ nullptr }
		, isDebugRender{ true }
	{
		Safe_AddRef(this->pDevice);
		Safe_AddRef(this->pContext);

		if (!pBatch)
			this->pBatch = new PrimitiveBatch<VertexPositionColor>(this->pContext);
		if (!pEffect)
			this->pEffect = new BasicEffect(pDevice);

		this->pEffect->SetVertexColorEnabled(true);

		if (!pInputLayout)
		{
			const void* pShaderInput = { nullptr };
			size_t iShaderInputLenght = {};
			pEffect->GetVertexShaderBytecode(&pShaderInput, &iShaderInputLenght);
			pDevice->CreateInputLayout(VertexPositionColor::InputElements, VertexPositionColor::InputElementCount, pShaderInput, iShaderInputLenght, &this->pInputLayout);
		}
	}

	Camera_Cinematic_Sequence& Camera_Cinematic_Sequence::operator=(const Camera_Cinematic_Sequence& rhs)
	{
		if (this == &rhs)
			return *this;

		this->isDebugRender			= rhs.isDebugRender;
		this->strDataName			= rhs.strDataName;
		this->vecCamKeyFrameData	= rhs.vecCamKeyFrameData;

		Safe_Release(this->pDevice);
		Safe_Release(this->pContext);

		this->pDevice				= rhs.pDevice;		Safe_AddRef(pDevice);
		this->pContext				= rhs.pContext;		Safe_AddRef(pContext);

		this->vecStartCinematic_GlobalEventIndex	= rhs.vecStartCinematic_GlobalEventIndex;
		this->vecEndCinematic_GlobalEventIndex		= rhs.vecEndCinematic_GlobalEventIndex;

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

	Camera_Cinematic_Sequence::~Camera_Cinematic_Sequence()
	{
		/* Debug Render 관련 Delete */
		Safe_Delete(pBatch);
		Safe_Delete(pEffect);
		Safe_Release(pInputLayout);


		Safe_Release(this->pDevice);
		Safe_Release(this->pContext);
	}
	HRESULT Camera_Cinematic_Sequence::Render_Debug(_uint iPassIndex, CModel* pCameraModel, CShader* pShader)
	{
		if (isDebugRender == false) return S_OK; 

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
		vecPosition.reserve(this->vecCamKeyFrameData.size());

		for (auto& CamData : this->vecCamKeyFrameData)
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
		if (this->pBatch && this->pEffect &&  vecPosition.size() >= 2 )
		{

			this->pContext->GSSetShader(nullptr, nullptr, 0);
			this->pContext->RSSetState(nullptr);
			this->pContext->OMSetBlendState(nullptr, nullptr, 0xffffffff);

			this->pEffect->Apply(this->pContext);
			this->pContext->IASetInputLayout(this->pInputLayout);
			this->pBatch->Begin();

			for (size_t i = 0; i < vecPosition.size() - 1 ; ++i)
			{
				pEffect->SetView(pGameInstance->Get_ViewMatrix());
				pEffect->SetProjection(pGameInstance->Get_ProjMatrix());
				pEffect->SetWorld(Matrix::Identity); // 경로는 월드좌표 기준이므로 Identity

				Vec3 vPos1 = vecCamKeyFrameData[i].vPosition;
				Vec3 vPos2 = vecCamKeyFrameData[i + 1].vPosition;

				// 색상은 노란색(Yellow) 등 취향껏 설정
				VertexPositionColor v1(vPos1, DirectX::Colors::Magenta);
				VertexPositionColor v2(vPos2, DirectX::Colors::Magenta);

				pBatch->DrawLine(v1, v2);
			}

			pBatch->End();

		}

		return S_OK;

	}



	void Camera_Keyframe_Data::Save_Json(json& SaveJson)
	{
		/* Move 관련 */
		SaveJson["MoveBase Target"]				= CinematicTarget_ToString(this->eMoveBaseTarget);
		SaveJson["Move Lerp Type"]				= LerpType_ToString(this->eMoveLerpType);
		SaveJson["MoveBase Target Bone Index"]	= this->iMoveBaseTargetBoneIndex;
		Engine_Utils::write_vec3_xyz(SaveJson["Position"] , this->vPosition);

		/* Look At 관련 */
		SaveJson["Look At Target"]				= CinematicTarget_ToString(this->eLookAtTarget);
		SaveJson["Look At Lerp Type"]			= LerpType_ToString(this->eLookAtLerpType);
		SaveJson["Look At Target Bone Index"]	= this->iLookAtBoneIndex;
		Engine_Utils::write_vec3_xyz(SaveJson["Look At Offset"], this->vLookAtOffset);
		Engine_Utils::write_vec3_xyz(SaveJson["Pitch Yaw Roll"], this->vPitchYawRoll);

		/* Fov 관련 */
		SaveJson["Fov Lerp Type"]				= LerpType_ToString(this->eFovLerpType);	
		SaveJson["Fov"]							= this->fFov;

		SaveJson["Duration"]					= this->fDuration;
		SaveJson["HoldTime"]					= this->fHoldTime;
	}

	void Camera_Keyframe_Data::Load_Json(const json& LoadJson)
	{/* Move 관련 */
		if (LoadJson.contains("MoveBase Target"))
			this->eMoveBaseTarget = CinematicTarget_ToEnum(LoadJson["MoveBase Target"].get<string>());

		if (LoadJson.contains("Move Lerp Type"))
			this->eMoveLerpType = LerpType_ToEnum(LoadJson["Move Lerp Type"].get<string>());

		if (LoadJson.contains("MoveBase Target Bone Index"))
			this->iMoveBaseTargetBoneIndex = LoadJson["MoveBase Target Bone Index"].get<_int>();

		if (LoadJson.contains("Position"))
			Engine_Utils::read_vec3_xyz(LoadJson["Position"], this->vPosition);


		/* Look At 관련 */
		if (LoadJson.contains("Look At Target"))
			this->eLookAtTarget = CinematicTarget_ToEnum(LoadJson["Look At Target"].get<string>());

		if (LoadJson.contains("Look At Lerp Type"))
			this->eLookAtLerpType = LerpType_ToEnum(LoadJson["Look At Lerp Type"].get<string>());

		if (LoadJson.contains("Look At Target Bone Index"))
			this->iLookAtBoneIndex = LoadJson["Look At Target Bone Index"].get<_int>();

		if (LoadJson.contains("Look At Offset"))
			Engine_Utils::read_vec3_xyz(LoadJson["Look At Offset"], this->vLookAtOffset);

		if (LoadJson.contains("Pitch Yaw Roll"))
			Engine_Utils::read_vec3_xyz(LoadJson["Pitch Yaw Roll"], this->vPitchYawRoll);


		/* Fov 관련 */
		if (LoadJson.contains("Fov Lerp Type"))
			this->eFovLerpType = LerpType_ToEnum(LoadJson["Fov Lerp Type"].get<string>());

		if (LoadJson.contains("Fov"))
			this->fFov = LoadJson["Fov"].get<_float>();

		/* 시간 관련 */
		if (LoadJson.contains("Duration"))
			this->fDuration = LoadJson["Duration"].get<_float>();

		if (LoadJson.contains("HoldTime"))
			this->fHoldTime = LoadJson["HoldTime"].get<_float>();
	}


	void Camera_Cinematic_Sequence::Delete(_int iDeleteIndex)
	{
		if (iDeleteIndex >= this->vecCamKeyFrameData.size()) return;
		this->vecCamKeyFrameData.erase(this->vecCamKeyFrameData.begin() + iDeleteIndex);
	}

	void Camera_Cinematic_Sequence::Reset(_int iResetIndex)
	{
		
		/* 전체 리셋 */
		if (iResetIndex == -1)
		{
			this->vecCamKeyFrameData.clear();
		}
		else
		{
			if (iResetIndex >= this->vecCamKeyFrameData.size()) return;

			this->vecCamKeyFrameData[iResetIndex].Reset();
		}
	}

	void Camera_Cinematic_Sequence::Copy_Camera(CCameraMan* pCamera, _int iCopyIndex)
	{
		if (iCopyIndex == -1)
		{
			this->vecCamKeyFrameData.push_back(Camera_Keyframe_Data(pCamera));
			return;
		}

		if (iCopyIndex >= this->vecCamKeyFrameData.size())
			return;

		this->vecCamKeyFrameData[iCopyIndex].Copy_Camera(pCamera);
	}

	void Camera_Cinematic_Sequence::Add_KeyFrameData(_int iCopyBeforeDataIndex)
	{
		/* 현재 Vec가 비어있다면 그냥추가  */
		if (vecCamKeyFrameData.empty() || iCopyBeforeDataIndex == -1)
			vecCamKeyFrameData.push_back(Camera_Keyframe_Data());
		else
		{
			/* Boudinig Check */
			if (iCopyBeforeDataIndex >= vecCamKeyFrameData.size())
				return;
			vecCamKeyFrameData.push_back(vecCamKeyFrameData[iCopyBeforeDataIndex]);
		}
	}

	void Camera_Cinematic_Sequence::Save_Json(json& SaveJson)
	{
		if (!this->vecStartCinematic_GlobalEventIndex.empty())
		{
			auto& StartGlobalEventName_SaveJson = SaveJson["Start Global Event Names"];
			StartGlobalEventName_SaveJson = this->vecStartCinematic_GlobalEventIndex;
		}
		if (!this->vecEndCinematic_GlobalEventIndex.empty())
		{
			auto& StartGlobalEventName_SaveJson = SaveJson["End Global Event Names"];
			StartGlobalEventName_SaveJson = this->vecEndCinematic_GlobalEventIndex;
		}


		/* Key Frame Data 저장 */
		auto& CamkeyFrame_SaveJson = SaveJson["Camera Keyframe Data"];
		for (auto& CamKeyframeData : this->vecCamKeyFrameData)
		{
			json CamKeyFrameData_SaveJson{};
			CamKeyframeData.Save_Json(CamKeyFrameData_SaveJson);
			if (CamKeyFrameData_SaveJson.is_null())
				continue;
			CamkeyFrame_SaveJson.push_back(CamKeyFrameData_SaveJson);
		}
	}

	void Camera_Cinematic_Sequence::Load_Json(const json& LoadJson)
	{
		// 1. 현재 이 함수에 들어온 JSON 전체 구조를 로그로 출력
		OutputDebugStringA(LoadJson.dump(4).c_str());

		if (LoadJson.contains("Start Global Event Names"))
		{
			const auto& JsonArray = LoadJson["Start Global Event Names"];

			for (auto& Json : JsonArray)
			{
				if (Json.is_null())
					continue;
				this->vecStartCinematic_GlobalEventIndex.push_back(Json);
			}
		}

		if (LoadJson.contains("End Global Event Names"))
		{
			const auto& JsonArray = LoadJson["End Global Event Names"];

			for (auto& Json : JsonArray)
			{
				if (Json.is_null())
					continue;
				this->vecEndCinematic_GlobalEventIndex.push_back(Json);
			}
		}



		if (LoadJson.contains("Camera Keyframe Data"))
		{
			this->vecCamKeyFrameData.clear();
			const json& CameraKeyFrameData_LoadJsonArray = LoadJson["Camera Keyframe Data"];
			_uint iSize = static_cast<_uint>(CameraKeyFrameData_LoadJsonArray.size());
			this->vecCamKeyFrameData.reserve(iSize);
			for (auto& CameraKeyFrameData_LoadJson : CameraKeyFrameData_LoadJsonArray)
			{
				if (CameraKeyFrameData_LoadJson.is_null())
					continue;
				Camera_Keyframe_Data tCamkeyFrameData{};
				tCamkeyFrameData.Load_Json(CameraKeyFrameData_LoadJson);
				vecCamKeyFrameData.push_back(tCamkeyFrameData);
			}
		}
	}

	void Camera_Cinematic_Sequence::BroadCast(_bool isStart) const
	{
		auto* pGameInstnace = CGameInstance::GetInstance();
		if (pGameInstnace == nullptr) return;

		if (isStart == true)
		{
			for (auto& Index : this->vecStartCinematic_GlobalEventIndex)
				pGameInstnace->BroadCaset_RegisterGlobalEvent(Index);
		}
		else
		{
			for (auto& Index : this->vecEndCinematic_GlobalEventIndex)
				pGameInstnace->BroadCaset_RegisterGlobalEvent(Index);
		}
	}

#pragma endregion

#pragma endregion
}