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

#pragma region Shader
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

	typedef struct tagShaderSkillEffetctDesc
	{
		unsigned int iFlags = { 0 };
		float fIntensity = { 0.8f };
		float fDissolveThreshold = { 0.5f };
		float fEdgeWidth = { 0.05f };
		SimpleMath::Vector2 vUVOffset = { 0.f, 0.f };
		SimpleMath::Vector2 vGradationMap = { 0.f, 0.f }; // Index, Count
		SimpleMath::Vector2 vPatternScale = { 0.f, 0.f }; // Scale X,Y
		SimpleMath::Vector2 vPatternOffset = { 0.f, 0.f }; // Offset X,Y
		float fGradationHeight = { 0.1f };
		SimpleMath::Vector3 vPadding = {};
	}SHADER_SKILLEFFECT_DESC;

	typedef struct tagShaderEffectDesc
	{
		unsigned int iTextureFlags = { 0 };
		SimpleMath::Vector3 vPadding = {};

		unsigned int iRenderFlags = { 0 };
		SimpleMath::Vector3 vPadding_2 = {};

		unsigned int  iSamplerStateFlags = {0};
		SimpleMath::Vector3 vPadding_3 = {};

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

	typedef struct tagModelPartsPrototypeTags
	{
		wstring wstrHead = { L"" };
		wstring wstrFace = { L"" };
		wstring wstrOneset = { L"" };
		wstring wstrUpper = { L"" };
		wstring wstrLower = { L"" };
	}MODELPARTS_PROTOTYPETAGS;

	typedef struct tagShaderSnowBillboardDesc
	{
		SimpleMath::Vector4 vColor = { 1.f, 1.f, 1.f, 1.f };

		SimpleMath::Vector3 vVelocity = { 0.f, -5.f, 0.f };
		float fDrawDistance = { 0.f };

		SimpleMath::Vector3 vOrigin = { 0.f, 0.f, 0.f };
		float fTurbulence = { 5.f };

		SimpleMath::Vector3 vExtent = { 0.f, 0.f, 0.f };
		float fTime = { 0.f };

	}SHADER_SNOWBBDESC;
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

	typedef struct tagEffectPresetSnapShot
	{
		string wstrTextureTag[5]
		{
			"Texture_Default",
			"",
			"",
			"",
			""
		};
		bool bPattern = { false };
		bool bUVScroll = { false };
		bool bDissovle = { false };
		bool bDistortion = { false };
		bool bGradation = { false };
		bool bGradationMap = { false };
		float fIntensity = { 1.f };
		float fThreshold = { 0.f };
		float fEdgieWidth = { 0.05f };
		float fUVSpeedX = { 0.f };
		float fUVSpeedY = { 0.f };
		int iPatternCols = { 1 };
		int iPatternRows = { 1 };
		int iGradationMapIndex = { 0 };
		int iGradationMapCount = { 0 };
		float fGradationHeight = { 0.f };
		SimpleMath::Vector4 vTintColor = { 1.f, 1.f, 1.f, 1.f };
		float fEmissivePower = { 1.f };
		wstring wstrMaterialTag = L"";
		wstring wstrMaterialInstanceTag = L"";
	}EFFECT_PRESET_SNAPSHOT;

	typedef struct tagTransformSaveData
	{
		SimpleMath::Vector3 vPos = {};
		SimpleMath::Vector4 vQuaternion = {};
		SimpleMath::Vector3 vScale = { 1.f, 1.f, 1.f };
	}TRANSFORM_SAVEDATA;

	typedef struct tagTriggerBoxSaveData
	{
		string strPolygonName = { "" };
		vector<string> vecMonsterNames;
		vector<SimpleMath::Vector3> vecPositions;
	}TRIGGERBOX_SAVEDATA;

	typedef struct tagModelSaveData
	{
		std::string fileName = { "" };
		vector<EMaterialInstanceType> vecMI;
		vector<string> vecMaskTextureTags;
		vector<int> vecShaderPassesByMesh;
	}MODEL_SAVEDATA;

	typedef struct tagLightSaveData
	{
		LIGHT_TYPE eType;
		SimpleMath::Vector4 vDiffuse = { 1.f, 1.f, 1.f, 1.f };
		SimpleMath::Vector4 vAmbient = { 1.f, 1.f, 1.f, 1.f };
	}LIGHT_SAVEDATA;

	typedef struct tagColliderSaveData
	{
		string shape = { "" };
		float fRadius = {};
		SimpleMath::Vector3 vCenter = {};
		SimpleMath::Vector3 vExtents = {};
		SimpleMath::Vector3 vEuler = {};
	}COLLIDER_SAVEDATA;

	typedef struct tagCellSaveData
	{
		array<SimpleMath::Vector3, ENUM_TO_UINT(EPOINT::END)> arrPoints;
		array<SimpleMath::Vector3, ENUM_TO_UINT(ELINE::END)> arrNormals;
		array<int, ENUM_TO_UINT(ELINE::END)> arrNeighbors;
		int	iIndex = { -1 };
	}CELL_SAVEDATA;

	typedef struct tagPolygonSaveData
	{
		vector<CELL_SAVEDATA> vecCells;
		vector<SimpleMath::Vector3> vecVertices;
		vector<unsigned short> vecIndices;
	}POLYGON_SAVEDATA;

	typedef struct tagMeshEffectPreviewSaveData
	{
		int iPresetID = -1;
		int iShaderPass = 0;
		std::string strModelTag = "";
	}MESHEFFECT_PREVIEW_SAVEDATA;

	typedef struct tagMeshEffectPresetSaveData
	{
		int iPresetID = -1;
		string strName = { "" };
		tagEffectPresetSnapShot snapShot;
	}MESHEEFFECT_PRESET_SAVEDATA;

	typedef struct tagMapObjectSaveData
	{
		std::string type = { "" };
		std::string name = { "" };

		std::optional<TRANSFORM_SAVEDATA> transform;
		std::optional<MODEL_SAVEDATA> model;
		std::optional<POLYGON_SAVEDATA> polygon;
		std::optional<LIGHT_SAVEDATA> light;
		std::optional<TRIGGERBOX_SAVEDATA> trigger;
		vector<COLLIDER_SAVEDATA> vecColliders;

		std::optional<MESHEFFECT_PREVIEW_SAVEDATA> meshEffect;
	}MAPOBJECT_SAVEDATA;

	typedef struct tagMapFileData
	{
		std::string mapName;
		int iVersion = 1;
		std::vector<MAPOBJECT_SAVEDATA> objects;
		std::vector<MESHEEFFECT_PRESET_SAVEDATA> meshEffectPresets;
	}MAPFILE_DATA;

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

}


#endif // Engine_Struct_h__
