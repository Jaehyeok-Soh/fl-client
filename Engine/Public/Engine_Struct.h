#ifndef Engine_Struct_h__
#define Engine_Struct_h__
#include "VertexData.h"

namespace Engine
{
#pragma region Engine
	typedef struct tagEngineDesc
	{
		WINMODE eIsWindow;
		unsigned int iWinCX;
		unsigned int iWinCY;
		unsigned int iLevelCount;
		unsigned int iCollideLayerCount;
		HWND hWnd;
		HINSTANCE hInst;
	}ENGINE_DESC;
	typedef struct tagToolDesc
	{
	}TOOL_DESC;
#pragma endregion

#pragma region Shader_ConstantBuffer
	typedef struct tagShaderGlobalDesc
	{
		Matrix matView = Matrix::Identity;
		Matrix matProj = Matrix::Identity;
		Matrix matVP = Matrix::Identity;
	}SHADER_GLOBALDESC;

	typedef struct tagShaderInvDesc
	{
		Matrix matInvView = Matrix::Identity;
		Matrix matInvProj = Matrix::Identity;
	}SHADER_INVDESC;

	typedef struct tagShaderTransformDesc
	{
		Matrix matWorld = Matrix::Identity;
	}SHADER_TRANSFORMDESC;

	typedef struct tagLightDesc
	{
		LIGHT_TYPE				eType = { LIGHT_TYPE::END };
		SimpleMath::Vector4		vDiffuse = { 0.f, 0.f, 0.f, 0.f };
		SimpleMath::Vector4		vAmbient = { 0.f, 0.f, 0.f, 0.f };
		SimpleMath::Vector4		vSpecular = { 0.f, 0.f, 0.f, 0.f };

		SimpleMath::Vector3		vDirection = { 0.f, 0.f, 0.f };
		SimpleMath::Vector4		vPosition = { 0.f, 0.f, 0.f, 0.f };
		float					fRange = { 1.f };
	}LIGHT_DESC;

	typedef struct tagShaderLightDesc
	{
		SimpleMath::Vector4 vAmbient = { 1.f, 1.f, 1.f, 1.f };
		SimpleMath::Vector4 vDiffuse = { 1.f, 1.f, 1.f, 1.f };
		SimpleMath::Vector4 vSpecular = { 1.f, 1.f, 1.f, 1.f };

		SimpleMath::Vector4 vPosition = { 0.f, 0.f, 0.f, 1.f };
		SimpleMath::Vector3 vDirection = { 0.f, 0.f, 0.f };
		float fRange = 1.f;
	}SHADER_LIGHTDESC;

	typedef struct tagShaderMaterialDesc
	{
		SimpleMath::Vector4 vAmbient = { 0.5f, 0.5f, 0.5f, 1.f };
		SimpleMath::Vector4 vDiffuse = { 1.f, 1.f, 1.f, 1.f };
		SimpleMath::Vector4 vSpecular = { 1.f, 1.f, 1.f, 1.f };
		SimpleMath::Vector4 vEmissive = { 0.f, 1.f, 0.f, 1.f };
	}SHADER_MATERIALDESC;

	typedef struct tagShaderMaterialInstanceDesc
	{
		SimpleMath::Vector4 vTintColor = { 1.f, 1.f,1.f, 1.f };
		SimpleMath::Vector4 vAmbient = { 0.5f, 0.5f, 0.5f, 1.f };
		SimpleMath::Vector4 vSpecular = { 1.f, 1.f,1.f, 1.f };
		SimpleMath::Vector4 vEmissive = { 1.f, 1.f,1.f, 1.f };
		SimpleMath::Vector3 vPadding = {};
		float fEmissivePower = { 1.f };
	}SHADER_MI_DESC;

	typedef struct tagShaderEffectDesc
	{
		unsigned int iTextureFlags = { 0 };
		unsigned int iRenderFlags = { 0 };
		unsigned int  iSamplerStateFlags = {0};
		float iDiscardValue = { 0.f };

		unsigned int iOperatorFlags = { 0 };
		unsigned int iRotationFlags = { 0 };
		SimpleMath::Vector2 vPadding1 = { 0.f, 0.f };

		// 스프라이트 정보 추가
		unsigned int SpriteColCount = {};		// 가로 프레임 수
		unsigned int SpriteRowCount = {};		// 세로 프레임 수
		unsigned int CurSpriteIndex = {};		// 현재 스프라이트 인덱스
		float		 Padding2 = {};

		SimpleMath::Vector2 vScrollOffset = { 0.f, 0.f };
		SimpleMath::Vector2 vDistortionScale = { 0.f, 0.f };
		SimpleMath::Vector4 vEffectColor = { 0.f, 0.f, 0.f, 0.f };
	}SHADER_EFFECT_DESC;

	typedef struct tagShaderBoneDesc
	{
		SimpleMath::Matrix transforms[MAX_BONE_TRANSFORMS];
	}SHADER_BONEDESC;

	struct AnimationData
	{
		SimpleMath::Vector2 vSpriteOffset;
		SimpleMath::Vector2 vSpriteSize;
		SimpleMath::Vector2 vTextureSize;
		float fUseAnimation;
		float fPadding;
	};

	typedef struct tagKeyFrame
	{
		SimpleMath::Vector3 vScale = { 0.f, 0.f, 0.f };
		SimpleMath::Vector4 vQuaterion = { 0.f, 0.f, 0.f, 1.f };
		SimpleMath::Vector3 vTranslation = { 0.f, 0.f, 0.f };
		float fTrackPosition = { 0.f };
	}KEYFRAME;

	typedef struct tagLocalSRTData
	{
		SimpleMath::Vector3 vScale = { 0.f, 0.f, 0.f };
		SimpleMath::Vector4 vQuaterion = { 0.f, 0.f, 0.f, 1.f };
		SimpleMath::Vector3 vTranslation = { 0.f, 0.f, 0.f };
	}LOCALSRT;

	// Animation
	typedef struct tagShaderKeyframeDesc
	{
		int iAnimIndex = 0;
		unsigned int iCurrFrame = 0;
		unsigned int iNextFrame = 0;
		float fRatio = 0.f;
		float fSumTime = 0.f;
		float fSpeed = 1.f;
		SimpleMath::Vector2 padding;
	}SHADER_KEYFRAMEDESC;

	typedef struct tagShaderTweenDesc
	{
		float fTweenDuration = { 1.0f };
		float fTweenRatio = { 0.f };
		float fTweenSumTime = { 0.f };
		float padding = { 0.f };
		tagShaderKeyframeDesc curr = {};
		tagShaderKeyframeDesc next = {};

		tagShaderTweenDesc()
		{
			curr.iAnimIndex = { 0 };
			next.iAnimIndex = { -1 };
		}

		void ClearNextAnim()
		{
			next.iAnimIndex = { -1 };
			next.iCurrFrame = { 0 };
			next.iNextFrame = { 0 };
			next.fSumTime = { 0.f };
			fTweenSumTime = { 0.f };
			fTweenRatio = { 0.f };
		}
	}SHADER_TWEENDESC;

	typedef struct tagShaderInstancedTweenDesc
	{
		tagShaderTweenDesc tweens[MAX_MODEL_INSTANCE];
	}SHADER_INST_TWEENDESC;
#pragma endregion

#pragma region Shader_StructuredBuffer

	// 불변 데이터 (한번 바인딩 하면 바뀌지 않는 데이터)
	typedef struct tagEffect_Particle_IMMU_ELEMENT
	{
		SimpleMath::Vector4		vRight = { 0.f, 0.f, 0.f, 0.f };
		SimpleMath::Vector4		vUp = { 0.f, 0.f, 0.f, 0.f };
		SimpleMath::Vector4		vLook = { 0.f, 0.f, 0.f, 0.f };
		SimpleMath::Vector4		vTranslation = { 0.f, 0.f, 0.f, 1.f };
		SimpleMath::Vector2		vParticle_LifeTime = { 0.f, 0.f };

		// 연산용 Speed 함수
		float fSpeed = { 1.f };
		float fPadding = { 1.f };
		SimpleMath::Matrix vParticle_OriginMatrix = {};
	}EFFECT_PARTICLE_IMMU_ELEMENT;

	// 가변 데이터 (CS에서 값이 계속 바뀌는 것들)
	typedef struct tagEffect_Particle_MU_ELEMENT
	{
		// 시간 제어 관련
		float				fTimeDelta = { 0.f };		// 시간 값
		float				fTotalTime = { 0.f };
		float				fDuration = { 0.f };
		float				fStartDelay = { 0.f };

		// 상태 플래그
		unsigned int		iMoveState = { 0 };
		int					bIsLoop = { 0 };
		unsigned int		iTimeFlag = {};
		float				fGravity = { 9.8f };

		// 위치 및 방향
		SimpleMath::Vector3	vPivot = {};	// Spread시 기준점
		float Padding1 = {};
		SimpleMath::Vector3 vLook = {};		// Straight시 방향
		float Padding2 = {};

		float				fStartSpeed = { 0.f };
		float				fSpiralRadius = { 0.f };
		float				fSpiralSpeed = { 0.f };
		float				Padding3 = {};

		// 

	}EFFECT_PARTICLE_MU_ELEMENT;
#pragma endregion

#pragma region Model_ComShader_Structures

#pragma region BONEFIANL_CS

	typedef struct tagBoneMeshCB
	{
		unsigned int				iAffectBoneNums = { 0 };
		unsigned int				iTotalBoneNums = { 0 };
		SimpleMath::Vector2			Padding0 = {};
	}CS_CB_ME_BONEMESH;

	typedef struct tagBoneMeshIMMU
	{
		unsigned int				iAffectBoneIndex = { 0 };
		SimpleMath::Vector3			Padding0 = {};

		SimpleMath::Matrix			matOffsetTransform = {};
	}CS_IMMU_BONEMESH;

#pragma endregion

#pragma region BONECOM_CS

	// output
	typedef struct tagSRT
	{
		SimpleMath::Vector3         vScale;
		float						Padding0;
	
		SimpleMath::Vector4			vQuat;

		SimpleMath::Vector3			vTranslation;
		float						Padding1;
	}CS_SRT;
	 
	// 불변 데이터
	typedef struct tagBone_Immu_Element
	{
		int			iParentIndex = { -1 };
		SimpleMath::Vector3 Padding0 = {};

		SimpleMath::Matrix matPreTransform = {};
	}CS_IMMU_BONE;

	// 가변 데이터 : cpu
	typedef struct tagBone_Mu_Element
	{
		int			iMyIdx = { -1 };

		SimpleMath::Vector3 Padding0 = {};
	}CS_MU_BONEIDX;

	// 가변 데이터 : cpu
	typedef struct tagBone_Mu_Group
	{
		unsigned int		iGroupBoneNums = {0};

		SimpleMath::Vector3 Padding0 = {};
	}CS_MU_GROUPNUMS;

	// output : 만약 바로 vs로 넘긴다면 필요 없지만
	// 충돌이나 여기 저기에서 사용할 수 있어서 일단 만들어 둠
	typedef struct tagBone_Output
	{
		SimpleMath::Matrix matCombinedTransform = { };
	}CS_OUT_BONE;
#pragma endregion

#pragma region ANIM_EVAL_CS
	// 불변 데이터
	typedef struct tagAnimE_Immu_KeyFrame
	{
		SimpleMath::Vector3  vScale = { 1.f,1.f,1.f };
		float   fTrackPosition = { 0.f };

		SimpleMath::Vector4  vQuat = {};

		SimpleMath::Vector3  vTranslation = {};
		float   fPadding0 = { 0.f };
	}CS_IMMU_ANIM_KEYFRAME;

	// 불변 데이터 : cpu
	typedef struct tagAnimE_Immu_ChannelData
	{
		int     iBoneIndex = { -1 };             // 내 bone transform을 잘 업데이트 하기 위함
		int     iRootMotionBoneIndex = { -1 };   // root motion일 경우 tralation을 0으로 만들기 위함

		unsigned int    iKeyStart = { 0 };              // 키프레임 시작 위치
		unsigned int    iKeyCount = { 0 };              // 키프레임 개수
	}CS_IMMU_ANIM_CHANNELDATA;

	// 가변 데이터 : cpu
	typedef struct tagBone_Mu_Track
	{
		float   fCurTrackPosition = { 0.f };
		unsigned int iChannelCount = { 0 };

		SimpleMath::Vector2  Padding0 = {};
	}CS_MU_TRACK;
#pragma endregion

#pragma region ANIM_Blendd_CS
	// 가변 데이터
	typedef struct tagAnimB_Immu_Ratio
	{
		int						iRootMotionBoneIndex = { -1 }; // root motion일 경우 tralation을 0으로 만들기 위함
		float					fRatio = { 0.f };

		unsigned int     iBoneCount = {0};
		float  Padding0 = { 0.f };
	}CS_MU_ANIMB;

	// output
	//typedef struct tagBone_Output
	//{
	//	float3              vScale;
	//	uint                iCurKeyFrameIndex;

	//	float4              vQuat;

	//	float3              vTranslation;
	//	uint                iAnimIndex
	//}CS_OUT_ANIME;
#pragma endregion

#pragma endregion


	union COLLIDER_ID
	{
		struct
		{
			unsigned int iLeft_ID;
			unsigned int iRight_ID;
		};
		static unsigned int Get_LeftID(unsigned long long _ID)
		{
			// 좌측에 있는거니 32비트 만큼 오른쪽 쉬프트하면 Left
			return (unsigned int)(_ID >> 32);
		}
		static unsigned int Get_RightID(unsigned long long _ID)
		{
			// 우측에 있는거니 그대로 32비트만큼만 & 연산
			return (unsigned int)(_ID & 0xFFFF'FFFF);
		}

		unsigned long long ID;
	};

	struct DelegateHandle
	{
		uint64_t iID = { 0 };
		// 0 미사용, 1 기본
		// 원래는 Slot 재사용하면서 Generation++ 해야하지만 지금은 ID 관리
		uint32_t iGeneration = { 0 };

		bool operator == (const DelegateHandle& rhs) const noexcept
		{
			return (iID == rhs.iID) && (iGeneration == rhs.iGeneration);
		}

		bool operator != (const DelegateHandle& rhs) const noexcept
		{
			return !(*this == rhs);
		}
	};

	typedef struct tagAttackDesc
	{
		int iAttackType = { -1 };
		unsigned int iDamage = { 0 };
		bool bSpecialSkill = { false };
		bool bImpact = { false };
		float fForceAbs = { 0.f };
		float  fDragK = { 0.f };
		SimpleMath::Vector3 vColliderCenter = { 0.f, 0.f, 0.f };
	}ATTACK_DESC;

#pragma region Util
	typedef struct tagTimeCounter
	{
		float fTimeAcc = { 0.f }; // 누적 타임
		float fMaxTime = { 0.f }; // 최대 시간

		bool bCountTime = { true }; // 타임 카운트를 할래?
		bool bTimeReset = { true }; // 한 주기가 끝나고 acc를 0으로 다시 맞출건지

		float CountTime(const float fTimeDelta) // TimeCount 함수
		{
			// 카운트를 하지 않을 거라면 음수 반환
			if (!bCountTime)
				return -1.f;

			// 타임 누적
			fTimeAcc += fTimeDelta;

			// 넘었는지 검사
			if (fTimeAcc >= fMaxTime)
			{
				fTimeAcc = bTimeReset ? 0.f : fMaxTime; // timeacc 리셋
				return 1.f;
			}

			// 비율 값으로 반환
			return fTimeAcc / fMaxTime;
		}

	}TIME_COUNTER;
#pragma endregion

#pragma region Physics
	typedef struct tagPhysicsMaterial
	{
		////////////////
		/// Material ///
		////////////////
		EPhysicsMaterial eMaterial = { EPhysicsMaterial::DEFAULT };

		// custom 시 필요
		// 정치 마찰력 // 멈춰있는 물체를 밀 때 필요한 힘
		float fStaticFriction = {};
		// 운동 마찰력 // 미끄러지고 있는 도중에 작용하는 마찰 (높을수록 금방 멈춘다)
		float fDynamicFriction = {};
		// 반발 계수
		float fRestitution = {};

	}PHYSICSMATERIAL_DESC;

	typedef struct tagPhysicsCCT
	{
		EPhysicsCCTType eType = { EPhysicsCCTType::CAPSULE };
		const Matrix* pOwnerMatrix = { nullptr };
		float fRadius = {};
		float fHeight = {};
		SimpleMath::Vector3 vExtens = {};
		//PxCapsuleControllerDesc capsuleDesc{};
		//PxBoxControllerDesc boxDesc{};

		////////////////
		/// Material ///
		////////////////
		PHYSICSMATERIAL_DESC tMaterial = {};

		class CGameObject* pOwner = { nullptr };
	}PHYSICSCCT_DESC;

	typedef struct tagPhysicsRigidBody
	{
		EPhysicsActorType eType = { EPhysicsActorType::STATIC };
		EPhysicsCollisionDetection detection = { EPhysicsCollisionDetection::DISCRETE };
		float fDensity = { 10.f };
		float fMass = { 1.f };
		bool bUseGravity = { true };
		bool bIsKinematic = { false };
		float fLinearDamping = {};
		float fAngularDamping = {};

		vector<Matrix> pOwnerMatrices;
		vector<SimpleMath::Vector3> vScale_Isolated;
	}PHYSICSRIGIDBODY_DESC;

	typedef struct tagPass
	{
		ID3DX11EffectPass* pPass = { nullptr };
		wstring wstrName = L"";
		D3DX11_PASS_DESC tDesc = {};
		D3DX11_PASS_SHADER_DESC tVertexShaderDesc = {};
		D3DX11_EFFECT_SHADER_DESC tEffectVsDesc = {};
		vector<D3D11_SIGNATURE_PARAMETER_DESC> vecSignatureDescs;
	} PASS;

	typedef struct tagTechnique
	{
		ID3DX11EffectTechnique* pTechnique = { nullptr };
		wstring wstrName = L"";
		D3DX11_TECHNIQUE_DESC tDesc = {};
		vector<tagPass> vecPasses;
	} TECHNIQUE;

	typedef struct tagPhysicsCollider
	{
		///////////////////////////
		/// Prototype Parameter ///
		///////////////////////////
		unsigned int iPrototypeLevelIndex = {};
		wstring wstrModelPrototypeTag = { L"" };
		wstring wstrFilePath = { L"" };

		///////////////
		/// Details ///
		///////////////
		bool bIsTrigger = { false };
		SimpleMath::Vector3 vCenter = {};
		float fRadius = {};
		float fHeight = {};
		SimpleMath::Vector3 vExtents = {};
		SimpleMath::Vector3 vXAxis = { 0.f, 0.f, 1.f };

		////////////////
		/// Geometry ///
		////////////////
		EPhysicsShape eShape = { EPhysicsShape::BOX };

		///////////////////////
		/// Geometry Convex ///
		///////////////////////
		bool bIsConvex = { false };
		EPhysicsConvexShape eConvexShape = { EPhysicsConvexShape::END };
		float fMargin = {};

		/////////////////////////
		/// Geometry Triangle ///
		/////////////////////////
		// convex가 아니면 trimesh
		//bool bIsTriMesh = { false };

		////////////////
		/// Material ///
		////////////////
		PHYSICSMATERIAL_DESC tMaterial = {};

		//////////////////
		/// Properties ///
		//////////////////
		bool bIsActive = { true };

		////////////////////////
		/// Collision Filter ///
		////////////////////////
		PHYSICSFILTERGROUP eFilterGroup = PHYSICSFILTERGROUP::NONE;
		unsigned int iFilterMask = {};
	}PHYSICSCOLLIDER_DESC;

	typedef struct tagPhysicsFilterShader
	{
	}PHYSICSFILTERSHADER_DESC;
#pragma endregion

}


#endif // Engine_Struct_h__
