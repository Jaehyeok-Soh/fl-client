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
		XMFLOAT4X4 matView;
		XMFLOAT4X4 matProj;
		XMFLOAT4X4 matVP;
		tagShaderGlobalDesc()
		{
			::XMStoreFloat4x4(&matView, ::XMMatrixIdentity());
			::XMStoreFloat4x4(&matProj, ::XMMatrixIdentity());
			::XMStoreFloat4x4(&matVP, ::XMMatrixIdentity());
		}
		tagShaderGlobalDesc(const XMFLOAT4X4& view, const XMFLOAT4X4& Proj, const XMFLOAT4X4& vp, const XMFLOAT4X4& invView, const XMFLOAT4X4& invProj)
			: matView(view)
			, matProj(Proj)
			, matVP(vp)
		{}
		tagShaderGlobalDesc(const tagShaderGlobalDesc& rhs)
			: matView(rhs.matView)
			, matProj(rhs.matProj)
			, matVP(rhs.matVP)
		{}
	}SHADER_GLOBALDESC;

	typedef struct tagShaderInvDesc
	{
		XMFLOAT4X4 matInvView;
		XMFLOAT4X4 matInvProj;
		tagShaderInvDesc()
		{
			::XMStoreFloat4x4(&matInvView, ::XMMatrixIdentity());
			::XMStoreFloat4x4(&matInvProj, ::XMMatrixIdentity());
		}
	}SHADER_INVDESC;

	typedef struct tagShaderTransformDesc
	{
		XMFLOAT4X4 matWorld;
		tagShaderTransformDesc()
		{
			::XMStoreFloat4x4(&matWorld, ::XMMatrixIdentity());
		}
		tagShaderTransformDesc(const XMFLOAT4X4& world)
			: matWorld(world)
		{}
		tagShaderTransformDesc(const tagShaderTransformDesc& rhs)
			: matWorld(rhs.matWorld)
		{}
	}SHADER_TRANSFORMDESC;

	typedef struct tagLightDesc
	{
		LIGHT_TYPE		eType = { LIGHT_TYPE::END };
		XMFLOAT4		vDiffuse = { 0.f, 0.f, 0.f, 0.f };
		XMFLOAT4		vAmbient = { 0.f, 0.f, 0.f, 0.f };
		XMFLOAT4		vSpecular = { 0.f, 0.f, 0.f, 0.f };

		XMFLOAT3		vDirection = { 0.f, 0.f, 0.f };
		XMFLOAT4		vPosition = { 0.f, 0.f, 0.f, 0.f };
		float			fRange = { 1.f };
	}LIGHT_DESC;

	typedef struct tagShaderLightDesc
	{
		XMFLOAT4 vAmbient = { 1.f, 1.f, 1.f, 1.f };
		XMFLOAT4 vDiffuse = { 1.f, 1.f, 1.f, 1.f };
		XMFLOAT4 vSpecular = { 1.f, 1.f, 1.f, 1.f };

		XMFLOAT4 vPosition = { 0.f, 0.f, 0.f, 1.f };
		XMFLOAT3 vDirection = { 0.f, 0.f, 0.f };
		float fRange = 1.f;
	}SHADER_LIGHTDESC;

	typedef struct tagShaderMaterialDesc
	{
		XMFLOAT4 vAmbient = { 0.5f, 0.5f, 0.5f, 1.f };
		XMFLOAT4 vDiffuse = { 1.f, 1.f, 1.f, 1.f };
		XMFLOAT4 vSpecular = { 1.f, 1.f, 1.f, 1.f };
		XMFLOAT4 vEmissive = { 0.f, 1.f, 0.f, 1.f };
	}SHADER_MATERIALDESC;

	typedef struct tagShaderMaterialInstanceDesc
	{
		XMFLOAT4 vTintColor = { 1.f, 1.f,1.f, 1.f };
		XMFLOAT4 vAmbient = { 0.5f, 0.5f, 0.5f, 1.f };
		XMFLOAT4 vSpecular = { 1.f, 1.f,1.f, 1.f };
		XMFLOAT4 vEmissive = { 1.f, 1.f,1.f, 1.f };
		XMFLOAT3 vPadding = {};
		float fEmissivePower = { 1.f };
	}SHADER_MI_DESC;

	typedef struct tagShaderSkillEffetctDesc
	{
		unsigned int iFlags = { 0 };
		float fIntensity = { 0.8f };
		float fDissolveThreshold = { 0.5f };
		float fEdgeWidth = { 0.05f };
		XMFLOAT2 vUVOffset = { 0.f, 0.f };
		XMFLOAT2 vGradationMap = { 0.f, 0.f }; // Index, Count
		XMFLOAT2 vPatternScale = { 0.f, 0.f }; // Scale X,Y
		XMFLOAT2 vPatternOffset = { 0.f, 0.f }; // Offset X,Y
		float fGradationHeight = { 0.1f };
		XMFLOAT3 vPadding = {};
	}SHADER_SKILLEFFECT_DESC;

	typedef struct tagShaderBoneDesc
	{
		XMFLOAT4X4 transforms[MAX_BONE_TRANSFORMS];
	}SHADER_BONEDESC;

	struct AnimationData
	{
		XMFLOAT2 vSpriteOffset;
		XMFLOAT2 vSpriteSize;
		XMFLOAT2 vTextureSize;
		float fUseAnimation;
		float fPadding;
	};

	typedef struct tagKeyFrame
	{
		XMFLOAT3 vScale = { 0.f, 0.f, 0.f };
		XMFLOAT4 vQuaterion = { 0.f, 0.f, 0.f, 1.f };
		XMFLOAT3 vTranslation = { 0.f, 0.f, 0.f };
		float fTrackPosition = { 0.f };
	}KEYFRAME;

	typedef struct tagLocalSRTData
	{
		XMFLOAT3 vScale = { 0.f, 0.f, 0.f };
		XMFLOAT4 vQuaterion = { 0.f, 0.f, 0.f, 1.f };
		XMFLOAT3 vTranslation = { 0.f, 0.f, 0.f };
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
		XMFLOAT2 padding;
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
		XMFLOAT4 vColor = { 1.f, 1.f, 1.f, 1.f };

		XMFLOAT3 vVelocity = { 0.f, -5.f, 0.f };
		float fDrawDistance = { 0.f };

		XMFLOAT3 vOrigin = { 0.f, 0.f, 0.f };
		float fTurbulence = { 5.f };

		XMFLOAT3 vExtent = { 0.f, 0.f, 0.f };
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
		XMFLOAT3 vColliderCenter = { 0.f, 0.f, 0.f };
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
		XMFLOAT4 vTintColor = { 1.f, 1.f, 1.f, 1.f };
		float fEmissivePower = { 1.f };
		wstring wstrMaterialTag = L"";
		wstring wstrMaterialInstanceTag = L"";
	}EFFECT_PRESET_SNAPSHOT;

	typedef struct tagTransformSaveData
	{
		XMFLOAT3 vPos = {};
		XMFLOAT4 vQuaternion = {};
		XMFLOAT3 vScale = { 1.f, 1.f, 1.f };
	}TRANSFORM_SAVEDATA;

	typedef struct tagTriggerBoxSaveData
	{
		string strPolygonName = { "" };
		vector<string> vecMonsterNames;
		vector<XMFLOAT3> vecPositions;
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
		XMFLOAT4 vDiffuse = { 1.f, 1.f, 1.f, 1.f };
		XMFLOAT4 vAmbient = { 1.f, 1.f, 1.f, 1.f };
	}LIGHT_SAVEDATA;

	typedef struct tagColliderSaveData
	{
		string shape = { "" };
		float fRadius = {};
		XMFLOAT3 vCenter = {};
		XMFLOAT3 vExtents = {};
		XMFLOAT3 vEuler = {};
	}COLLIDER_SAVEDATA;

	typedef struct tagCellSaveData
	{
		array<XMFLOAT3, ENUM_TO_UINT(EPOINT::END)> arrPoints;
		array<XMFLOAT3, ENUM_TO_UINT(ELINE::END)> arrNormals;
		array<int, ENUM_TO_UINT(ELINE::END)> arrNeighbors;
		int	iIndex = { -1 };
	}CELL_SAVEDATA;

	typedef struct tagPolygonSaveData
	{
		vector<CELL_SAVEDATA> vecCells;
		vector<XMFLOAT3> vecVertices;
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
}


#endif // Engine_Struct_h__
