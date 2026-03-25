#ifndef Engine_Struct_h__
#define Engine_Struct_h__
#include "VertexData.h"

namespace DTO
{
	struct TAttackPreset_Data;
}

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

	struct AnimNotifyKey
	{
		EAnimNotifyId eID{ EAnimNotifyId::Hitbox };
		float fTrackPosition{ 0.f };

		unsigned int  iParam0{ 0 };
		unsigned int  iParam1{ 0 };
		unsigned int  iParam2{ 0 };
		unsigned int  iParam3{ 0 };
		float		  fParam0{ 0.0f };
		float		  fParam1{ 0.0f };
		bool		  bParam0{ false };
		bool		  bParam1{ false };
		string		  strParam{ "" };
		string		  strParam2{ "" };
	};

	typedef struct tagCollisionHitInformation
	{
		bool bHasHitPoint{ false };
		unsigned int iCollisionPhase{ COLLISIONEVENT::Enum::END };
		unsigned int iRequester_AttackPresetID{ UINT_MAX };
		unsigned int iOther_AttackPresetID{ UINT_MAX };
		float fDepth{ 0.f };
		SimpleMath::Vector3 vPosition{ SimpleMath::Vector3::Zero };
		SimpleMath::Vector3 vRawNormal{ SimpleMath::Vector3::Zero };
	}COL_HIT_INFO;

	typedef struct tagAttakerDesc
	{
		unsigned int iCollisionType{ COLLISIONEVENT::Enum::END };
		unsigned int iAttackPresetInstanceID{ 0 };
		unsigned int iAttackerLayer{ 0 };
		const DTO::TAttackPreset_Data* pAttackPreset{ nullptr };
		class CGameObject* pAttacker{ nullptr };
	}ATTACKER_DESC;

	typedef struct tagVictimDesc
	{
		bool bHasHitPoint{ false };
		unsigned int iVictimLayer{ 0 };
		float fDepth{ 0.f };
		class CGameObject* pVictim{ nullptr };
		
		SimpleMath::Vector3 vHitPoint{ SimpleMath::Vector3::Zero };
		SimpleMath::Vector3 vHitNormal{ SimpleMath::Vector3::Zero };
		ATTACKER_DESC attackDesc{}; // 지연 처리용

		float			fFinalDamage{ 0.f };
		unsigned int	iDamageFlag{ 0 };
	}HIT_DESC;

	typedef struct tagExtraAttDesc
	{
		// 규칙 rate의 값은 0 ~ 1로 써야함

		enum class ComputeOrder : unsigned int
		{
			Normal_Add
			, Normal_Rate
			, Random_Add
			, Random_Rate
		};

		// clamp 값 : 만약 x < y 일때만 clamp를 함
		SimpleMath::Vector2 vFinalDamege_MinMax = { SimpleMath::Vector2::Zero };

		// 어떤 종류의 damege를 주었는가에 대한 추가 정보
		unsigned int iDamageFlag = { 0 };
		// 예시 : 0번은 일반 공격, 1번은 critical 공격

		// 0. 단순히 추가할 값
		float fAddDamage{ 0.f }; 
		// 연산 규칙 : pre_computed_damage + fAddRate;

		// 1. 기본 데미지에 추가 할 rate
		float fAddRate{ 0.f };	
		// 연산 규칙  : pre_computed_damage * (1.f + fAddRate);

		// 2. random으로 추가 될 값
		float fRandomAdd_Rate{};
		SimpleMath::Vector2 vRandomAdd_MinMax{ SimpleMath::Vector2::Zero };
		// 연산 규칙 : random min max에서 값을 랜덤으로 뽑고
		// rate로 적용 할래 말래 결정
		// -> 적용 : pre_computed_damage + fAddRate;

		// 3. random으로 곱해질 값
		float fRandomMul_Rate{};
		SimpleMath::Vector2 vRandomMul_MinMax{ SimpleMath::Vector2::Zero };
		// 연산 규칙 : random min max에서 값을 랜덤으로 뽑고
		// rate로 적용 할래 말래 결정
		// -> 적용 : pre_computed_damage * (1.f + fAddRate);

		// 넘겨 받은 값들을 통해 연산 순서 맵핑 : ComputeOrder 값 이용
		std::vector<unsigned int> vecCompute_Order;

	}EXTRA_ATTACK_DESC;

	typedef struct tagCollidedDesc
	{
		unsigned int iCollisionType{ COLLISIONEVENT::Enum::END };
		unsigned int iRequesterLayer{ PHYSICSFILTERGROUP::Enum::END };
		unsigned int iOtherLayer{ PHYSICSFILTERGROUP::Enum::END };
		class CGameObject* pRequester{ nullptr };
		class CGameObject* pOther{ nullptr };
		COL_HIT_INFO tHitInfo{};

		// EXTRA_HIT_INFO : 값을 넣기 전에 hit info에 추가로 넣을 정보들.
		// 우리는 이미 attack preset에서 정보를 가져와서 값을 셋팅 해주기 때문에
		// client쪽에서 상황에 따라 기본 값에 어떠한 변화를 주고 싶다면 추가 적인 정보를 넘겨줘야함

		// CJudgementSystem 안에서
		// attack preset + extra data를 통해
		// 최종 damege 를 넘겨준다
		EXTRA_ATTACK_DESC tExtraDesc{};

	}COLLIDED_DESC;

	typedef struct tagSoundMeta
	{
		string strTag{""};
		unsigned int iHash = { 0 };
		ESoundCategory eCategory = { ESoundCategory::SFX };
		unsigned int iVariantCount = { 0 };
	}SOUND_META;

	typedef struct tagBakedSectionBuildInput
	{
		int                    iSectionX{ 0 };
		int                    iSectionZ{ 0 };
		BoundingBox            sectionBounds{};
		SimpleMath::Vector3    vLightDir{ SimpleMath::Vector3::Zero };
		const vector<class CGameObject*>* pStaticCasters{ nullptr };
	}BAKED_SECTION_BUILD_INPUT;

	typedef struct tagBakedSectionBuildResult
	{
		int                    iSectionX{ 0 };
		int                    iSectionZ{ 0 };
		BoundingBox            sectionBounds{};
		BoundingBox            casterBounds{};
		BoundingBox			   receiverBounds{};
		SimpleMath::Matrix     matLightVP{ SimpleMath::Matrix::Identity };
		SimpleMath::Vector4    vShadowParams{};
		vector<class CGameObject*> vecCasters;
		bool                   bValid{ false };
	}BAKED_SECTION_BUILD_RESULT;

	typedef struct tagBakedShadowSection
	{
		int iSectionX{ 0 };
		int iSectionZ{ 0 };
		BoundingBox sectionBounds{};
		BoundingBox casterBounds{};
		SimpleMath::Matrix matLightVP{ SimpleMath::Matrix::Identity };
		// x = bias
		// y = strength
		// z = InvSizeX
		// w = InvSizeY
		SimpleMath::Vector4 vShadowParams{ SimpleMath::Vector4::One };
		unsigned int iArraySlice{ 0 };
		bool bValid{ false };
	}BAKED_SHADOW_SECTION;

	typedef struct tagActiveBakedSet
	{
		BAKED_SHADOW_SECTION sections[ACTIVE_SECTION_MAX]{};
		unsigned int iCount = 0;
	}ACTIVE_BAKED_SET;

	typedef struct tagTimeline
	{
		float fDuration{ 0.f };
		float fElapsed{ 0.f };

		void Start(float _fDuration)
		{
			fDuration = (_fDuration < 0.f) ? 0.f : _fDuration;
			fElapsed = 0.f;
		}
		bool Tick(float fDeltaTime)
		{
			if (fDuration <= 0.f)
				return true;
			fElapsed += fDeltaTime;
			if (fElapsed >= fDuration)
			{
				fElapsed = fDuration;
				return true;
			}

			return false;
		}
		float Get_Alpha() const
		{
			if (fDuration <= 0.f)
				return 1.f;
			float fAlpha = fElapsed / fDuration;
			return (fAlpha < 0.f) ? 0.f : (fAlpha > 1.f ? 1.f : fAlpha);
		}
		float Get_Remain() const
		{
			float fRemain = fDuration - fElapsed;
			return (fRemain < 0.f) ? 0.f : fRemain;
		}
		bool Is_Active() const
		{
			return (fDuration > 0.f) && (fElapsed < fDuration);
		}
		void Clear()
		{
			fDuration = 0.f;
			fElapsed = 0.f;
		}
	}TIME_LINE;

	template<typename T>
	struct TimedValue
	{
		TIME_LINE time{};
		T from{};
		T to{};
		bool bActive{ false };

		void Start(const T& _from, const T& _to, float _fDuration)
		{
			from = _from;
			to = _to;
			time.Start(_fDuration);
			bActive = true;
		}

		void Tick(float _fUnscaledDeltaTime)
		{
			if (bActive == false)
				return;
			if (time.Tick(_fUnscaledDeltaTime) == true)
				bActive = false;
		}
		float Alpha() const { return time.Get_Alpha(); }
	};
#pragma region Shader_ConstantBuffer
	typedef struct tagShaderGlobalDesc
	{
		Matrix matView = Matrix::Identity;
		Matrix matProj = Matrix::Identity;
		Matrix matVP = Matrix::Identity;
		float fAccTime = { 0.f };
		SimpleMath::Vector3 vPadding = { SimpleMath::Vector3::Zero };
	}SHADER_GLOBALDESC;

	typedef struct tagShaderInvDesc
	{
		Matrix matCamView = Matrix::Identity;
		Matrix matCamProj = Matrix::Identity;
		Matrix matInvView = Matrix::Identity;
		Matrix matInvProj = Matrix::Identity;
	}SHADER_INVDESC;

	typedef struct tagShaderTransformDesc
	{
		Matrix matWorld = Matrix::Identity;
	}SHADER_TRANSFORMDESC;

	typedef struct tagShaderRenderFxDesc
	{
		unsigned int iFlags = 0;
		float fEmissiveIntensity = 0.f;
		float fOffsetX = 0.f;
		float fOffsetY = 0.f;

		SimpleMath::Vector3   vEmissiveColor = { SimpleMath::Vector3::One};
		float fReserved0 = 0.f;
	}SHADER_RENDER_FX_DESC;

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
		SimpleMath::Vector3 vPadding = { SimpleMath::Vector3::Zero };
		float fEmissivePower = { 1.f };
	}SHADER_MI_DESC;

	typedef struct tagShaderObjectInfoDesc
	{
		unsigned int iObjectID{ 0 };
		unsigned int Flags8{ 0 };
		SimpleMath::Vector2 vPadding{ SimpleMath::Vector2::Zero};
	}SHADER_OBJECTINFO_DESC;

	typedef struct tagShaderSSAOKernelDesc
	{
		SimpleMath::Vector4 vKernel[16]{ SimpleMath::Vector4::Zero};
		SimpleMath::Vector2 vNoiseScale{ SimpleMath::Vector2::Zero};
		SimpleMath::Vector2 vPadding{ SimpleMath::Vector2::Zero };
	}SHADER_SSAOKERNEL_DESC;

	typedef struct tagShaderSSAOParamDesc
	{
		float fRadius{ 0.f };
		float fBias{ 0.f };
		float fPower{ 0.f };
		float fIntensity{ 0.f };
		float fFadeStart{ 0.f };
		float fFadeEnd{ 0.f };
		SimpleMath::Vector2 vInvSize{ 1.f, 1.f };
	}SHADER_SSAOPARAM_DESC;

	typedef struct tagShaderHDRParamDesc
	{
		float fExposure{1.5f};
		float fGamma{2.2f};
		SimpleMath::Vector2 vPadding{ SimpleMath::Vector2::Zero };
	}SHADER_HDRPARAM_DESC;

	typedef struct tagShaderBloomParamDesc
	{
		SimpleMath::Vector2 vInvSize{ 1.f, 1.f };
		float fThreshold{ 0.f };
		float fKnee{ 0.f };
		float fIntensity{ 0.f };
		SimpleMath::Vector3 vPadding{ SimpleMath::Vector3::Zero };
	}SHADER_BLOOMPARAM_DESC;

	typedef struct tagShaderOutlineParamDesc
	{
		SimpleMath::Vector4 vColor{ SimpleMath::Vector4::Zero };
		SimpleMath::Vector2 vInvSize{ SimpleMath::Vector2::Zero };
		float fThicknessPx{0.f};
		float fOpacity{0.f};
		float fNormalThreshold{0.f};
		float fDepthThreshold{0.f};
		float fNormalStrength{0.f};
		float fDepthStrength{0.f};
		float fFadeStart{0.f};
		float fFadeEnd{0.f};
		SimpleMath::Vector2 vPadding{ SimpleMath::Vector2::Zero };
	}SHADER_OUTLINE_DESC;

	typedef struct tagShaderCascadeShadowDesc
	{
		// Light View × Proj 
		SimpleMath::Matrix matLightVP[SHADOW_CASCADE_COUNT]{ SimpleMath::Matrix::Identity };
		// ViewZ 기준 분할 거리
		float fCascadeEnd0 = { 8.0f };
		float fCascadeEnd1 = { 40.f };
		float  fShadowBias = { 0.0002f };
		float  fNormalBias = { 0.04f };

		SimpleMath::Vector2 vShadowMapInvSize = { SimpleMath::Vector2::Zero };
		float fShadowStrength = { 0.4f };
		float fCascadeIndex = { 0.f };
	}SHADER_CASCADE_SHADOW_DESC;

	typedef struct tagShaderBakedShadowDesc
	{
		SimpleMath::Matrix matLightVP{ SimpleMath::Matrix::Identity };
		SimpleMath::Vector2 vShadowMapInvSize = { SimpleMath::Vector2::Zero };
		float fShadowBias = { 0.0002f };
		float fShadowStrength = { 0.4f };
	}SHADER_BAKED_SHADOW_DESC;

	typedef struct tagShaderFogParamDesc
	{
		SimpleMath::Vector4 vColor = { 0.6f, 0.75f, 0.9f, 1.f };
		SimpleMath::Vector4 vHighColor = { 0.7f, 0.8f, 0.95f, 1.f };

		float fFogStart = { 15.f };
		float fFogEnd = { 80.f };
		float fFogDensity = { 0.f };
		float fFogHeightFalloff = { 0.15f };

		float fFogBaseHeight = { 0.f };
		float fFogMaxOpacity = { 0.85f };
		float fFogHeightDensity = { 0.02f };
		float fFogNoiseScale = { 0.f };

		float fFogNoiseSpeed = { 0.f };
		SimpleMath::Vector3 vPad = { SimpleMath::Vector3::Zero };
	}SHADER_FOG_DESC;

	typedef struct tagShaderToonParamDesc
	{
		float fWrap = { 0.25f };
		float fShadowMid = { 0.55f };
		float fShadowSoftness = { 0.10f };
		float fShadowStrength{ 0.85f };
		
		float fRimThreshold = { 0.65f };
		float fRimSoftness = { 0.10f };
		float fRimStrength = { 0.0f };
		float fPad = { 0.f };

		float fDiffuseStrength = { 1.0f };
		SimpleMath::Vector3 vPad { 0.f, 0.f, 0.f };
	}SHADER_TOON_DESC;

	typedef struct tagShaderBakedSection
	{
		SimpleMath::Matrix matLightVP{ SimpleMath::Matrix::Identity };
		SimpleMath::Vector4 vShadowParams{ SimpleMath::Vector4::Zero };
		SimpleMath::Vector4 vBoundsMin{ SimpleMath::Vector4::Zero };
		SimpleMath::Vector4 vBoundsMax{ SimpleMath::Vector4::Zero };
		unsigned int iArraySlice{ 0 };
		SimpleMath::Vector3 vPadding{ SimpleMath::Vector3::Zero };
	}SHADER_BAKED_SECTION;

	typedef struct tagShaderBakedSectionDesc
	{
		unsigned int iActiveCount{ 0 };
		SimpleMath::Vector3 vPadding{ SimpleMath::Vector3::Zero };
		SHADER_BAKED_SECTION sections[ACTIVE_SECTION_MAX]{};
	}SHADER_BAKED_SECTION_DESC;

	typedef struct tagShaderEffectDesc
	{
		// Row 0
		unsigned int iTextureFlags;
		unsigned int iRenderFlags;
		unsigned int iSamplerStateFlags;
		float iDiscardValue;

		// Row 1
		unsigned int iOperatorFlags;
		unsigned int iRotationFlags;
		SimpleMath::Vector2 vUVOffset;

		// Row 2
		SimpleMath::Vector2 vPadding0;
		float fGlowPower;
		float fLifeRatio;

		// Row 3
		SimpleMath::Vector2 vScrollOffset;
		SimpleMath::Vector2 vDistortionScale;

		// Row 4
		SimpleMath::Vector4 vEffectColor; // 위치 사수

		// Row 5
		SimpleMath::Vector2 DiffuseTexture_ScrollWeight;
		SimpleMath::Vector2 NoiseTexture_ScrollWeight;

		// Row 6
		SimpleMath::Vector2 MaskingTexture_ScrollWeight;
		SimpleMath::Vector2 GradationTexture_ScrollWeight;

		// Row 7
		SimpleMath::Vector2 DissolveTexture_ScrollWeight;
		SimpleMath::Vector2 GlowTexture_ScrollWeight;

		// Row 8
		SimpleMath::Vector2 CurveTexture_ScrollWeight;
		SimpleMath::Vector2 SubMaskingTexture_ScrollWeight;

		// Row 9
		SimpleMath::Vector4 DiffuseTexture_SpriteInfo;
		// Row 10
		SimpleMath::Vector4 NoiseTexture_SpriteInfo;
		// Row 11
		SimpleMath::Vector4 GradationTexture_SpriteInfo;
		// Row 12
		SimpleMath::Vector4 DissolveTexture_SpriteInfo;
		// Row 13
		SimpleMath::Vector4 GlowTexture_SpriteInfo;
		// Row 14
		SimpleMath::Vector4 CurveTexture_SpriteInfo;
		// Row 15
		SimpleMath::Vector4 MaskTexture_SpriteInfo;
		// Row 16
		SimpleMath::Vector4 SubMaskTexture_SpriteInfo;

	} SHADER_EFFECT_DESC;

	typedef struct tagShaderBoneDesc
	{
		SimpleMath::Matrix transforms[MAX_BONE_TRANSFORMS]{ SimpleMath::Matrix::Identity};
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

	typedef struct tagShaderRGBColor
	{
		SimpleMath::Vector4 vColorR = { 1.f, 1.f,1.f, 1.f };
		SimpleMath::Vector4 vColorG = { 1.f, 1.f,1.f, 1.f };
		SimpleMath::Vector4 vColorB = { 1.f, 1.f,1.f, 1.f };
	}SHADER_RGBCOLOR_DESC;

	typedef struct tagPlayerInfo
	{
		Matrix		PlayerMatrix{Matrix::Identity};		/* 플레이어 월드 메트릭스 */
		float		fCollisionRange{ 1.f };				/* 플레이어의 충돌범위 */
		float		fCollisionHeight{1.f};				/* 플레이어의 충돔높이 */

		float		fCurSpeed{0.f};						/* 가속도? */
		float		fMaxSpeed{5.f};						/* 시각적 효과를 위한 MaxSpeed 조절가능 */
	}SHADER_PLAYER_INFO;

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
		// Slot 1
		float				fTimeDelta = { 0.f };		// 시간 값
		float				fTotalTime = { 0.f };
		float				fDuration = { 0.f };
		float				fStartDelay = { 0.f };

		// Slot 2
		unsigned int		iMoveState = { 0 };
		int					bIsLoop = { 0 };
		unsigned int		iTimeFlag = {};
		float				fPadding4 = { 9.8f };

		// Slot 3
		SimpleMath::Vector3 vFinalGravity = { 0.f, 0.f, 0.f };
		float				fExternalStrength = { 0.f };

		// Slot 4
		SimpleMath::Vector3	vPivot = {};	// Spread시 기준점
		unsigned int		iEmissionType = {};

		// Slot 5
		SimpleMath::Vector3 vLook = {};		// Straight시 방향
		float Padding2 = {};

		// Slot 6
		float				fStartSpeed = { 0.f };
		float				fSpiralRadius = { 0.f };
		float				fSpiralSpeed = { 0.f };
		int					UseContinueFlag = {};

		// Slot 7
		SimpleMath::Vector3 vRange = {};
		unsigned int		iRandomSeed = {};

	}EFFECT_PARTICLE_MU_ELEMENT;


	typedef struct tagEffect_CurveInfo
	{
		unsigned int g_iGravityKeyCount = {0};
		SimpleMath::Vector3 g_vPadding = {0.f, 0.f, 0.f};
	}EFFECT_CURVEINFO;

#pragma endregion

#pragma region Model_ComShader_Structures

#pragma region BONEFIANL_CS

	typedef struct tagBoneMeshCB
	{
		unsigned int				iAffectBoneNums = { 0 };
		unsigned int				iTotalBoneNums = { 0 };
		SimpleMath::Vector2			Padding0 = {};
	}CS_CB_MU_BONEMESH;

	typedef struct tagBoneMeshIMMU
	{
		unsigned int				iAffectBoneIndex = { 0 };
		SimpleMath::Vector3			Padding0 = {};

		SimpleMath::Matrix			matOffsetTransform = {};
	}CS_IMMU_BONEMESH;

#pragma endregion

#pragma region BONEMove_CS

	typedef struct tagBoneMoveCB
	{
		int							iMovingIdx	= { -1 };
		unsigned int				iBoneNums	= { 0 };
		float						fRatio		= {0.f};
		float						Padding0	= {};

		SimpleMath::Matrix			matOffset = { Matrix::Identity };
	}CS_CB_MU_BONEMOVE;

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


		unsigned int    iKeyStart = { 0 };              // 키프레임 시작 위치
		unsigned int    iKeyCount = { 0 };              // 키프레임 개수

		float  Padding0 = { 0.f };
	}CS_IMMU_ANIM_CHANNELDATA;

	// 가변 데이터 : cpu
	typedef struct tagBone_Mu_Track
	{
		float   fCurTrackPosition = { 0.f };
		unsigned int iChannelCount = { 0 };

		int     iRootMotionBoneIndex = { -1 };   // root motion일 경우 tralation을 0으로 만들기 위함

		float  Padding0 = {0.f};
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

#pragma endregion

#pragma region ANIM_MIX_CS
	// 가변 데이터
	typedef struct tagAnimMix_Immu_Ratio
	{
		float fMixRatio = 0.f;

		SimpleMath::Vector3 Padding0 = {};
	}CS_IMMU_ANIMMIX;

	// 가변 데이터
	typedef struct tagAnimMix_Mu
	{
		float   fCurTrackPosition = { 0.f };
		unsigned int   iChannelCount = { 0 };

		int     iRootMotionBoneIndex = { -1 }; // root motion일 경우 tralation을 0으로 만들기 위함

		float     iFirst = { true };

		unsigned int         iMixType = { 0 }; // 0 : bone mix, 1 : addtive

		SimpleMath::Vector3  Padding0 = {};
	}CS_MU_ANIMMIX;

#pragma endregion

#pragma region PART_COMBINEBONE
	// 가변 데이터
	typedef struct tagPartCombineBone
	{
		unsigned int						iParentIdx = { 0 }; // root motion일 경우 tralation을 0으로 만들기 위함
		unsigned int						iBoneNums = { 0 }; // root motion일 경우 tralation을 0으로 만들기 위함
		SimpleMath::Vector2		Padding0 = {};
	}CS_IMMU_PARTBONE;

#pragma endregion

#pragma region RAGDOLL_CS
	// 가변 데이터
	typedef struct tagIMMU_RAGDOLL
	{
		unsigned int    iBoneIndex;         // 몇번째 뼈인지

		unsigned int    iTotalBoneNums;     // 총 뼈 개수
		unsigned int    iRagDollBoneNums;   // 래그돌 할 개수
 
		unsigned int    Padding0;
	}CS_IMMU_RAGDOLL;

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
		float fMinTime = { 0.f }; // 최소 시간(ex 상태를 이 시간은 꼭 유지시켜야한다)

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

		float CountMinTime(const float fTimeDelta) // TimeCount 함수
		{
			// 카운트를 하지 않을 거라면 음수 반환
			if (!bCountTime)
				return -1.f;

			// 타임 누적
			fTimeAcc += fTimeDelta;

			// 넘었는지 검사
			if (fTimeAcc >= fMinTime)
			{
				fTimeAcc = bTimeReset ? 0.f : fMinTime; // timeacc 리셋
				return 1.f;
			}

			// 비율 값으로 반환
			return fTimeAcc / fMinTime;
		}

		float Get_Rate() 
		{ 
			if (fMaxTime <= 0.f)
			{
				return 1.f;
			}

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
		class CGameObject* pOwner = { nullptr };
		bool bIsPlayer = { false };

		EPhysicsCCTType eType = { EPhysicsCCTType::CAPSULE };
		const Matrix* pOwnerMatrix = { nullptr };
		float fRadius = {};
		float fHeight = {};
		SimpleMath::Vector3 vExtens = {};
		//PxCapsuleControllerDesc capsuleDesc{};
		//PxBoxControllerDesc boxDesc{};
		float fContactOffset = 0.3f;
		float fStepOffset = 0.4f;
		float fSlopeLimit = 0.7f;

		DirectX::SimpleMath::Vector3 vLocalOffset = {};
		DirectX::SimpleMath::Vector3 vWorldOffset = {};

		bool bIsHover = { false };
		float fHoverOffset = { 1.f };

		////////////////
		/// Material ///
		////////////////
		PHYSICSMATERIAL_DESC tMaterial = {};

		////////////////////////
		/// Collision Filter ///
		////////////////////////
		unsigned int eFilterLayer = PHYSICSFILTERGROUP::Enum::NONE;
		unsigned int iFilterMask = {};

		//////////////////
		/// Move State ///
		//////////////////
		bool bGravity = { true };
		float fGravity = { -35.f };
		SimpleMath::Vector2 MSpeed = { 0.f, 8.f };
		SimpleMath::Vector2 MAccelRate = { 0.f, 10.f };
		SimpleMath::Vector2 MDeAccelRate = { 0.f, 5.f };
	}PHYSICSCCT_DESC;

	typedef struct tagPhysicsSRT
	{
		SimpleMath::Vector3 vScale = {};
		SimpleMath::Quaternion vQuat = {};
		SimpleMath::Vector3 vPosition = {};
	}PHYSICS_SRT;

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

		Matrix* pOwnerMatrix = { nullptr };
		vector<Matrix> pOwnerMatrices;
		vector<PHYSICS_SRT> vecSRT{};
	}PHYSICSRIGIDBODY_DESC;

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
		bool bIsSkillTrigger = { false };
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
		bool bSetOnlyFilter = { false };
		unsigned int eFilterLayer = PHYSICSFILTERGROUP::Enum::NONE;
		unsigned int iFilterMask = {};

		////////////////////
		/// Attack Preset///
		////////////////////
		string strAttackPresetTag = { "" };
		unsigned int iAttackPresetID = { UINT_MAX };
	}PHYSICSCOLLIDER_DESC;

	typedef struct tagRagdollBoneDesc
	{
		RAGDOLLJOINT::Enum eJoint = { RAGDOLLJOINT::END };
		RAGDOLLJOINT::Enum eParentJoint = { RAGDOLLJOINT::END };
		
		unsigned int iBoneIndex = {};
		int iParentIndex = { -1 };

		float fRadius = 0.05f;
		float fHeight = 0.05f;
		float fMass = 0.1f;

		Matrix matLocalTransform = { Matrix::Identity };
		PxTransform matOffsetTransform = {PxTransform(PxIdentity)};
	}RAGDOLLBONEDESC;

	typedef struct tagRagdollElements
	{
		using LinkIdentity = std::pair<PxArticulationLink*, RAGDOLLBONEDESC>;

		PxArticulationReducedCoordinate* pArticulation = { nullptr };
		vector<LinkIdentity> vecPhysicsLink;
		vector<Matrix> vecRagdollLiveTransform;
	}RAGDOLLELEMENTS;
#pragma endregion

	typedef struct tagOctreeDesc
	{
		BoundingBox rootBounds;
		int iMaxDepth{ 5 };
		float fLooseFactor{ 1.3f };
		float fMinNodeSizeXZ{ 2.0f }; // Extents와 비교할거라 중심에서의 거리
		size_t iMaxItemsPerLeaf{ 128 };
	}OCTREE_DESC;

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

#pragma region Font
	typedef struct tagFontDesc
	{
		EFontShaderType eFontShaderType;
		std::wstring strFontTag;
		std::wstring strText;
		DirectX::SimpleMath::Vector2 vPosition;
		DirectX::SimpleMath::Vector4 vColor;
		EFontPivotType ePivot;
		float fRotate;
		float fScale;
	}FONT_DESC;

#pragma endregion

#pragma region EFFECT
	typedef struct tagEffectSpawnDesc 
	{
		enum class E_VFX_COLORMODE
		{
			COLOR_NONCHANGE,
			COLOR_CHANGE,
		};

		enum class E_VFX_SIMULTYPE
		{
			VFX_LOCAL = 0,
			VFX_WORLD = 1,
		};
	public:
		SimpleMath::Matrix matWorld = {};							// 계산된 최종 행렬
		const SimpleMath::Matrix** pTargetBoneMatrix = { nullptr };	// 실시간 로컬용 부모 본 행렬 주소
		const SimpleMath::Matrix** pTransformMatrix = { nullptr };	// 실시간 로컬용 부모 행렬 주소
		int iBoneFlag;
		int iSimulationType = (int)E_VFX_SIMULTYPE::VFX_WORLD;		// LOCAL(0) or WORLD(1)
		bool bUseChildBone = { false };

	public:
		float				VFX_fSpeed = { 1.f };					// 전체적인 스피드 조절
		E_VFX_COLORMODE		VFX_COLORTYPE = E_VFX_COLORMODE::COLOR_NONCHANGE;
		SimpleMath::Vector3 VFX_Color = {};
	} EFFECT_SPAWN_DESC;

	typedef struct tagWarningEffectDesc : public EFFECT_SPAWN_DESC
	{
		SimpleMath::Vector3 VFX_Target_Position = { 0.f, 0.f, 0.f };
		//SimpleMath::Vector3 VFX_Attacker_Position = { 0.f, 0.f, 0.f };
		SimpleMath::Vector3 VFX_Scale = { 1.f, 1.f, 1.f };
		SimpleMath::Vector3 VFX_Rotation = { 0.f, 0.f, 0.f };
	}EFFECT_WARNING_DESC;

	typedef struct tagEnvironmentEffectDesc : public EFFECT_SPAWN_DESC
	{
		typedef struct tagENV_PARTEFFECT_DESC
		{
			unsigned int	iPartsIndex = {};

			SimpleMath::Vector3 /*Particle Position*/	VFX_Position_Parts;
			SimpleMath::Vector3 /*Particle Scale*/		VFX_Scale_Parts;
			SimpleMath::Vector3 /*Particle Rotation*/	VFX_Rotation_Parts;

			int											VFX_ParticleCount_Parts;
			float										VFX_ParticleLifeTime_Parts;
			float										VFX_ParticleDuration_Parts;
			SimpleMath::Vector3 /*Range*/				VFX_ParticleRange_Parts;
		}ENV_PART_DESC;

		// 부모 이펙트
		SimpleMath::Vector3								VFX_Target_Position = { 0.f, 0.f, 0.f };
		SimpleMath::Vector3								VFX_Scale = { 1.f, 1.f, 1.f };
		SimpleMath::Vector3								VFX_Rotation = { 0.f, 0.f, 0.f };

		std::vector<ENV_PART_DESC>						VFX_PartsDescList = {};
	}EFFECT_ENV_DESC;

#pragma endregion

#pragma region Global Event Desc



#pragma endregion

#pragma region CameraShaking_Data

	typedef struct tagCameraEventDataBase
	{
		
	}CAM_EVENTDATA_BASE;

#pragma endregion




#pragma region 

	typedef struct CinematicCameraSequence_EventDesc
	{
		/* 저장된 string 데이터를 Client에서 사용할때는 ToHahs 함수를 사용하여 미리 컴파일 단계때 해쉬를 생성해서 사용해준다 */
		string          strSubscriberName;   // 예: "UI_Tutorial", "Boss_Haier"
		vector<string>  vecActionNames;      // 예: "Show_Popup", "Roar"
	}CCS_EVENT_DESC;

#pragma endregion


#pragma region Cinemaic Camera Sequence Desc

	typedef struct CinematicCameraSequence_BroadCast_Desc
	{
		string					strCCSName{""};
		CCS_BROADCAST_TYPE		eCCS_BroadCast_Type{ CCS_BROADCAST_TYPE::END };
		int						iCurrentCameraKeyFrameIndex{-1};
		vector<CCS_EVENT_DESC>	vecCCS_Event_Desc{};
	}CCS_BROADCAST_DESC;

#pragma endregion


#pragma endregion

}





#pragma endregion

#endif // Engine_Struct_h__
